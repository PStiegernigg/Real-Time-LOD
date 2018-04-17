/* 
 * File:   LODGenerator.h
 * Author: Philipp Stiegernigg <p.stiegernigg@student.uibk.ac.at>
 *
 * Created on June 4, 2015, 1:26 PM
 */

#pragma once

#include <GL/glew.h>
#include <memory>
#include <climits>

#include "config.h"
#include "geometry/Mesh.h"
#include "rendering/Shader.h"

struct Edge {
    GLuint from;
    GLuint to;
    GLuint face;
    GLuint cost;
    GLuint state;
    GLuint valid;
};

struct Vertex {
    GLfloat quadric[10] {0};
    GLuint minEdgeCost {UINT_MAX};
    GLuint minEdgeID {UINT_MAX};
    GLuint collapseTarget{UINT_MAX};
    GLuint valid {1};
};

struct OptimalPlacement {
    GLfloat posX;
    GLfloat posY;
    GLfloat posZ;
};

struct Face {
    GLuint valid;
};

struct FaceCollapse {
    GLuint faceOne{0};
    GLuint faceTwo{0};
    GLuint faceOneSet{0};
};

enum EdgeState {
    NOOPERATION = 0,
    COLLAPSE = 1,
};

enum ShaderIndex {
    INITEDGES = 0,
    INITQUADRICS = 1,
    OPTIMIZEQUADRICS = 2,
    COLLAPSEEDGESINIT = 3,
    COLLAPSEEDGES = 4,
    UPDATEFACES = 5,
    UPDATEEDGES = 6,
    UPDATEEDGESHIERARCHY = 7,
    RESETVALUES = 8,
    COMPACTEDGES = 9,
};

enum BufferIndex {
    EDGESONE = 0,
    EDGESTWO = 1,
    VERTEX = 2,
    FACE = 3,
    FACECOLLAPSE = 4,
    PLACEMENT = 5,
    COLLAPSEPLACEMENT = 6
};

class LODGenerator {
public:
    LODGenerator();
    virtual ~LODGenerator();
    
    void createDetailLevels(const std::shared_ptr<Mesh>&, GLuint errMin, GLuint errMax, GLuint numLevels);
    
private:
    std::array<GLuint, 7> buffers;
    std::array<std::unique_ptr<Shader>, 10> shaders;
    
    GLuint numVertices;
    GLuint numFaces;
    GLuint numEdges;
    GLuint validEdgeCount;
    GLuint totalEdgeCount;
    GLuint decimationPass;
    bool levelFinished;
    bool swapEdgeBuffers;
    
    void initShaders();
    void initBuffers(const std::shared_ptr<Mesh>&);
    void initEdges();
    void initQuadricMetrics();
    void optimizeQuadricErrors(const GLuint errorThreshold);
    void collapseEdgesInit();
    void collapseEdges();
    void updateQuadrics();
    void updateFaceConnectivity();
    void updateEdgeConnectivity();
    void updateEdgeConnectivityHierarchy();
    void resetValues();
    void compactEdges();
    
    void saveCurrentLevel(const std::shared_ptr<Mesh>&);
    void saveCollapseHierarchy(const std::shared_ptr<Mesh>&);
};

