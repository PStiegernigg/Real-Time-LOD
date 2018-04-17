#include <iostream>
#include <memory>
#include <cstring>
#include <algorithm>

#include "geometry/LODGenerator.h"
#include "rendering/Shader.h"
#include "util/FloatToIntConversion.h"
#include "util/Util.h"

LODGenerator::LODGenerator()
: buffers {
},
shaders{
    {std::make_unique<Shader>("shaders/initEdges.shdr"),
        std::make_unique<Shader>("shaders/initQuadricMetrics.shdr"),
        std::make_unique<Shader>("shaders/quadricErrorOptimization.shdr"),
        std::make_unique<Shader>("shaders/parallelEdgeCollapseInit.shdr"),
        std::make_unique<Shader>("shaders/parallelEdgeCollapse.shdr"),
        std::make_unique<Shader>("shaders/updateFaces.shdr"),
        std::make_unique<Shader>("shaders/updateEdges.shdr"),
        std::make_unique<Shader>("shaders/updateEdgesHierarchy.shdr"),
        std::make_unique<Shader>("shaders/resetValues.shdr"),
        std::make_unique<Shader>("shaders/compactEdges.shdr")}
},

validEdgeCount{0},
decimationPass{0},
levelFinished{false},
swapEdgeBuffers{false}

{
    initShaders();
}

LODGenerator::~LODGenerator() {

}

void LODGenerator::initShaders() {

    for (auto &shader : shaders) {
        shader->init();
    }

}

void LODGenerator::initBuffers(const std::shared_ptr<Mesh>& mesh) {

    numVertices = mesh->mVertices[0].size() / 3;
    numFaces = mesh->mIndices[0].size() / 3;
    numEdges = mesh->mIndices[0].size();
    totalEdgeCount = numEdges;

    GLuint zero = 0;

    glGenBuffers(1, &validEdgeCount);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, validEdgeCount);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof (GLuint), &zero, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

    glGenBuffers(GLsizei(buffers.size()), buffers.data());

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[EDGESONE]);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
            numEdges * sizeof (Edge),
            nullptr,
            GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[PLACEMENT]);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
            numEdges * sizeof (OptimalPlacement),
            nullptr,
            GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[COLLAPSEPLACEMENT]);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
            numVertices * sizeof (OptimalPlacement),
            nullptr,
            GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[EDGESTWO]);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
            numEdges * sizeof (Edge),
            nullptr,
            GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[VERTEX]);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
            numVertices * sizeof (Vertex),
            nullptr,
            GL_DYNAMIC_COPY);

    Vertex *vertexPtr = (Vertex*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    Vertex vertex;

    for (uint i = 0; i < numVertices; i++) {
        vertex.collapseTarget = i;
        memcpy(vertexPtr, &vertex, sizeof (Vertex));
        vertexPtr++;
    }

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[FACECOLLAPSE]);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
            numVertices * sizeof (FaceCollapse),
            nullptr,
            GL_DYNAMIC_COPY);

    FaceCollapse *faceCollapsePtr = (FaceCollapse*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    FaceCollapse faceCollapse;

    for (uint i = 0; i < numVertices; i++) {
        memcpy(faceCollapsePtr, &faceCollapse, sizeof (FaceCollapse));
        faceCollapsePtr++;
    }

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[FACE]);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
            numFaces * sizeof (Face),
            nullptr,
            GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, validEdgeCount);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[EDGESONE]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffers[EDGESTWO]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buffers[VERTEX]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, buffers[FACE]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, buffers[PLACEMENT]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, mesh->buffers[VERTICES]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, mesh->buffers[INDICES]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, buffers[FACECOLLAPSE]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, buffers[COLLAPSEPLACEMENT]);
}

void LODGenerator::initEdges() {
    shaders[INITEDGES]->useProgram();
    shaders[INITEDGES]->setUniform("workSize", numFaces);

    //Run compute shader for all faces to initialize edges
    //One additional workgroup if size is not an multiple
    glDispatchCompute(numFaces / WORKGROUPSIZE + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void LODGenerator::initQuadricMetrics() {
    shaders[INITQUADRICS]->useProgram();
    shaders[INITQUADRICS]->setUniform("workSize", numFaces);

    glDispatchCompute(numFaces / WORKGROUPSIZE + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void LODGenerator::optimizeQuadricErrors(GLuint errorThreshold) {
    shaders[OPTIMIZEQUADRICS]->useProgram();
    shaders[OPTIMIZEQUADRICS]->setUniform("workSize", numEdges);
    shaders[OPTIMIZEQUADRICS]->setUniform("errorThresholdSquared", errorThreshold);

    glDispatchCompute(numEdges / WORKGROUPSIZE + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void LODGenerator::collapseEdgesInit() {
    shaders[COLLAPSEEDGESINIT]->useProgram();
    shaders[COLLAPSEEDGESINIT]->setUniform("workSize", numEdges);

    glDispatchCompute(numEdges / WORKGROUPSIZE + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void LODGenerator::collapseEdges() {
    shaders[COLLAPSEEDGES]->useProgram();
    shaders[COLLAPSEEDGES]->setUniform("workSize", numEdges);
    shaders[COLLAPSEEDGES]->setUniform("decimationPass", decimationPass++);

    glDispatchCompute(numEdges / WORKGROUPSIZE + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void LODGenerator::updateFaceConnectivity() {
    shaders[UPDATEFACES]->useProgram();
    shaders[UPDATEFACES]->setUniform("workSize", numFaces);

    glDispatchCompute(numFaces / WORKGROUPSIZE + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void LODGenerator::updateEdgeConnectivity() {
    shaders[UPDATEEDGES]->useProgram();
    shaders[UPDATEEDGES]->setUniform("workSize", numEdges);

    glDispatchCompute(numEdges / WORKGROUPSIZE + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void LODGenerator::updateEdgeConnectivityHierarchy() {
    shaders[UPDATEEDGESHIERARCHY]->useProgram();
    shaders[UPDATEEDGESHIERARCHY]->setUniform("workSize", numEdges);

    glDispatchCompute(numEdges / WORKGROUPSIZE + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void LODGenerator::resetValues() {
    shaders[RESETVALUES]->useProgram();
    shaders[RESETVALUES]->setUniform("workSize", numVertices);

    glDispatchCompute(numVertices / WORKGROUPSIZE + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void LODGenerator::compactEdges() {
    shaders[COMPACTEDGES]->useProgram();
    shaders[COMPACTEDGES]->setUniform("workSize", numEdges);

    //TODO find out why this reinitialization is necessary to prevent segfault 
    GLuint zero = 0;
    GLuint* ptr;

    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, validEdgeCount);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof (GLuint), &zero, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

    glDispatchCompute(numEdges / WORKGROUPSIZE + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, validEdgeCount);
    ptr = (GLuint*) glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof (GLuint),
            GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);

    GLuint validEdges = ptr[0];

    if (numEdges == validEdges) {
        levelFinished = true;
        return;
    }

    numEdges = validEdges;
    ptr[0] = 0;

    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

    if (swapEdgeBuffers) {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[EDGESONE]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffers[EDGESTWO]);
        swapEdgeBuffers = false;
    } else {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[EDGESTWO]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffers[EDGESONE]);
        swapEdgeBuffers = true;
    }


}

void LODGenerator::createDetailLevels(const std::shared_ptr<Mesh>& mesh,
        GLuint errMin,
        GLuint errMax,
        GLuint numLevels) {

    GLuint errorThreshold = errMin;
    GLuint errorStep = (errMax - errMin) / (numLevels - 1);

    initBuffers(mesh);
    initEdges();
    initQuadricMetrics();

    for (uint i = 1; i <= numLevels; ++i) {

        if (i == numLevels) {
            errorThreshold = errMax;
        }

        while (!levelFinished) {
            optimizeQuadricErrors(errorThreshold);
            collapseEdgesInit();
            collapseEdges();
            updateEdgeConnectivityHierarchy();
            updateFaceConnectivity();
            compactEdges();
            resetValues();
        }

        saveCurrentLevel(mesh);
        levelFinished = false;

        errorThreshold += errorStep;
    }

    saveCollapseHierarchy(mesh);
    mesh->initBuffers(0);
}

void LODGenerator::saveCurrentLevel(const std::shared_ptr<Mesh>& mesh) {
    std::vector<GLfloat> vertexVector;
    std::vector<GLuint> indexVector;
    std::vector<GLfloat> normalVector;
    std::vector<GLuint> vertexMapping(numVertices);

    GLuint* indices = new GLuint[numFaces * 3];
    GLfloat* vertices = new GLfloat[numVertices * 3];
    Face* faces = new Face[numFaces];
    Vertex* vertexInfo = new Vertex[numVertices];
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[VERTEX]);
    Vertex* vertexInfoPtr = (Vertex*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    memcpy(vertexInfo, vertexInfoPtr, numVertices * sizeof (Vertex));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mesh->buffers[VERTICES]);
    GLfloat* vertexPtr = (GLfloat*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    memcpy(vertices, vertexPtr, 3 * numVertices * sizeof(GLfloat));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    
    for (uint i = 0; i < numVertices; ++i) {
        if(vertexInfo[i].valid) {
            vertexMapping[i] = vertexVector.size() / 3;
            vertexVector.push_back(vertices[3 * i]);
            vertexVector.push_back(vertices[3 * i + 1]);
            vertexVector.push_back(vertices[3 * i + 2]);
            normalVector.push_back(mesh->mNormals[0][3 * i]);
            normalVector.push_back(mesh->mNormals[0][3 * i + 1]);
            normalVector.push_back(mesh->mNormals[0][3 * i + 2]);
        }
    }
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mesh->buffers[INDICES]);
    GLuint* indexPtr = (GLuint*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

    memcpy(indices, indexPtr, numFaces * 3 * sizeof (GLuint));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[FACE]);
    Face* facePtr = (Face*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    memcpy(faces, facePtr, numFaces * sizeof (Face));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    for (uint i = 0; i < numFaces; ++i) {
        if (faces[i].valid) {
            indexVector.push_back(indices[i * 3]);
            indexVector.push_back(indices[i * 3 + 1]);
            indexVector.push_back(indices[i * 3 + 2]);
        }
    }
    
    //Remap vertex indices
    for(auto& index : indexVector) {
        index = vertexMapping[index];
    }

    delete [] indices;
    delete [] vertices;
    delete [] faces;
    delete [] vertexInfo;

    mesh->setVertices(std::move(vertexVector));
    mesh->setIndices(std::move(indexVector));
    mesh->setNormals(std::move(normalVector));

    float errorThreshold = 1.0f - ((float) numEdges / (float) totalEdgeCount);
    mesh->addLevelError(errorThreshold);
}

struct CompareVerticesID : std::binary_function<size_t, size_t, bool> {

    CompareVerticesID(const std::vector<Vertex>& vertices)
    : mVertices(vertices) {
    }

    bool operator()(size_t Lhs, size_t Rhs) const {
        return mVertices[Lhs].minEdgeID < mVertices[Rhs].minEdgeID;
    }

    const std::vector<Vertex>& mVertices;
};

struct CompareVerticesCost : std::binary_function<size_t, size_t, bool> {

    CompareVerticesCost(const std::vector<Vertex>& vertices)
    : mVertices(vertices) {
    }

    bool operator()(size_t Lhs, size_t Rhs) const {
        return mVertices[Lhs].minEdgeCost < mVertices[Rhs].minEdgeCost;
    }

    const std::vector<Vertex>& mVertices;
};

struct CompareVerticesCollapseTarget : std::binary_function<size_t, size_t, bool> {

    CompareVerticesCollapseTarget(const std::vector<Vertex>& vertices)
    : mVertices(vertices) {
    }

    bool operator()(size_t Lhs, size_t Rhs) const {
        return mVertices[Lhs].collapseTarget < mVertices[Rhs].collapseTarget;
    }

    const std::vector<Vertex>& mVertices;
};

void LODGenerator::saveCollapseHierarchy(const std::shared_ptr<Mesh>& mesh) {
    std::vector<Vertex> vertices(numVertices);
    std::vector<FaceCollapse> faceCollapses(numVertices);
    std::vector<OptimalPlacement> placement(numVertices);
    std::vector<Face> faces(numFaces);

    //Read back the vertex data from the GPU
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[VERTEX]);
    Vertex* vertexPtr = (Vertex*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    memcpy(vertices.data(), vertexPtr, numVertices * sizeof (Vertex));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    //Read back collapse placement from the GPU
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[COLLAPSEPLACEMENT]);
    OptimalPlacement* placementPtr = (OptimalPlacement*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    memcpy(placement.data(), placementPtr, numVertices * sizeof (OptimalPlacement));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    //Read back the face collapse data from the GPU
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[FACECOLLAPSE]);
    FaceCollapse* faceCollapsePtr = (FaceCollapse*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    memcpy(faceCollapses.data(), faceCollapsePtr, numVertices * sizeof (FaceCollapse));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    //Read back the face data from the GPU
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[FACE]);
    Face* facePtr = (Face*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    memcpy(faces.data(), facePtr, numFaces * sizeof (Face));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    //Init vertex order data
    std::vector<GLuint> vertexOrder(numVertices);

    for (uint i = 0; i < numVertices; ++i) {
        vertexOrder[i] = i;
    }

    //Sort vertex index by decimation pass to maintain collapse order;
    std::sort(vertexOrder.begin(), vertexOrder.end(), CompareVerticesID(vertices));

    //Add vertices in reordered sequence 
    std::vector<GLfloat> reorderVertices;
    std::vector<GLuint> vertexMapping(numVertices);

    for (uint index = 0; index < numVertices; ++index) {
        reorderVertices.push_back(mesh->mVertices[0][vertexOrder[index] * 3]);
        reorderVertices.push_back(mesh->mVertices[0][vertexOrder[index] * 3 + 1]);
        reorderVertices.push_back(mesh->mVertices[0][vertexOrder[index] * 3 + 2]);
        vertexMapping[vertexOrder[index]] = index;
    }
    
    
    // Create placement order to extract all positions 
    std::vector<GLuint> placementOrder(vertexOrder);
    std::vector<GLuint> placementOrderPtr(numVertices);

    std::stable_sort(placementOrder.begin(), placementOrder.end(), CompareVerticesCollapseTarget(vertices));

    GLuint currentTarget = vertices[placementOrder[0]].collapseTarget;
    placementOrderPtr[vertices[placementOrder[0]].collapseTarget] = 0;

    for (uint i = 0; i < numVertices; ++i) {
        if (currentTarget != vertices[placementOrder[i]].collapseTarget) {
            currentTarget = vertices[placementOrder[i]].collapseTarget;
            placementOrderPtr[vertices[placementOrder[i]].collapseTarget] = i;
        }

    }

    //Build collapse hierarchy
    GLuint current;
    std::vector<GLuint> collapseHierarchy;
    std::vector<HierarchyInformation> hierarchyInformation;
    HierarchyInformation currentInfo;

    for (uint i = 0; i < numVertices; ++i) {
        current = vertexOrder[i];

        uint index = 0;
        uint start = placementOrderPtr[current];

        currentInfo.startPtr = collapseHierarchy.size();

        while (current == vertices[placementOrder[start + index]].collapseTarget) {

            collapseHierarchy.push_back(vertexMapping[placementOrder[start + index]]);

            ++index;

            if (start + index == placementOrder.size()) break;
        }

        collapseHierarchy.push_back(vertexMapping[current]);

        while (vertices[current].collapseTarget != current) {

            start = placementOrderPtr[vertices[current].collapseTarget];
            index = 0;

            while (vertices[current].collapseTarget == vertices[placementOrder[start + index]].collapseTarget) {

                if (vertexMapping[placementOrder[start + index]] > vertexMapping[current]) {

                    collapseHierarchy.push_back(vertexMapping[placementOrder[start + index]]);
                }

                ++index;

                if (start + index == placementOrder.size()) break;
            }

            collapseHierarchy.push_back(vertexMapping[vertices[current].collapseTarget]);

            current = vertices[current].collapseTarget;
        }

        currentInfo.currentCollapse = 0;
        hierarchyInformation.push_back(currentInfo);

    }
    
    std::vector<GLuint> reorderIndices;
    std::vector<GLuint> reorderIndicesInvalid;
    
    //Reverse order and append all collapsed faces  
    std::reverse(vertexOrder.begin(), vertexOrder.end());
    GLuint faceIndex;
    
    for (uint index = 0; index < numVertices; ++index) {
        if (!vertices[vertexOrder[index]].valid) {
            
            faceIndex = faceCollapses[vertexOrder[index]].faceOne;
            faces[faceIndex].valid = 2;
            reorderIndicesInvalid.push_back(mesh->mIndices[0][3 * faceIndex]);
            reorderIndicesInvalid.push_back(mesh->mIndices[0][3 * faceIndex + 1]);
            reorderIndicesInvalid.push_back(mesh->mIndices[0][3 * faceIndex + 2]);

            faceIndex = faceCollapses[vertexOrder[index]].faceTwo;
            faces[faceIndex].valid = 2;
            reorderIndicesInvalid.push_back(mesh->mIndices[0][3 * faceIndex]);
            reorderIndicesInvalid.push_back(mesh->mIndices[0][3 * faceIndex + 1]);
            reorderIndicesInvalid.push_back(mesh->mIndices[0][3 * faceIndex + 2]);
        }
    }
    
     //Insert valid faces which did not collapse
    for (uint i = 0; i < numFaces; ++i) {
        if (faces[i].valid != 2) {
            reorderIndices.push_back(mesh->mIndices[0][3 * i]);
            reorderIndices.push_back(mesh->mIndices[0][3 * i + 1]);
            reorderIndices.push_back(mesh->mIndices[0][3 * i + 2]);
        }
    }
    
    reorderIndices.insert(reorderIndices.end(), reorderIndicesInvalid.begin(),
    reorderIndicesInvalid.end());

    //Update face indices
    for (auto& i : reorderIndices) {
        i = vertexMapping[i];
    }

    //Reorder collapse placement
    std::vector<GLfloat> reorderPlacement(3 * numVertices);

    for (uint i = 0; i < numVertices; ++i) {
        reorderPlacement[3 * vertexMapping[i]] = placement[i].posX;
        reorderPlacement[3 * vertexMapping[i] + 1] = placement[i].posY;
        reorderPlacement[3 * vertexMapping[i] + 2] = placement[i].posZ;
    }

    //Reorder normals
    if (mesh->hasNormals) {
        std::vector<GLfloat> reorderNormals(3 * numVertices);

        for (uint i = 0; i < numVertices; ++i) {
            reorderNormals[3 * vertexMapping[i]] = mesh->mNormals[0][3 * i];
            reorderNormals[3 * vertexMapping[i] + 1] = mesh->mNormals[0][3 * i + 1];
            reorderNormals[3 * vertexMapping[i] + 2] = mesh->mNormals[0][3 * i + 2];
        }
        
        mesh->mNormals[0] = reorderNormals;
    }

    //Assign reordered vertices/faces and collapse information to mesh
    mesh->mVertices[0] = reorderVertices;
    mesh->mIndices[0] = reorderIndices;
    mesh->mCollapseHierarchy = collapseHierarchy;
    mesh->mHierarchyInformation = hierarchyInformation;
    mesh->mCollapsePlacement = reorderPlacement;

    mesh->initContinuousLOD();
}



