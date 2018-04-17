#include <iostream>

#include "config.h"
#include "geometry/Mesh.h"
#include "rendering/Shader.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <memory>

Mesh::Mesh()
: hasNormals{
    false
}
, hasTexCoords{false}
, hasLOD{false}
, cleaned{false}

, mVertices()
, mNormals()
, mTexCoords()
, mTangents()
, mBitangents()
, mIndices()
, buffers()
, continuousLODBuffers()
, mBoundingSphere {
    glm::vec3(0.f), 0.f
}

{
    // Create VAO
    glGenVertexArrays(1, &mVAO);

    // Create VBOs for all attributes
    glGenBuffers(GLsizei(buffers.size()), buffers.data());
}

Mesh::~Mesh() {
}

const BoundingSphere& Mesh::getBoundingSphere() const {
    return mBoundingSphere;
}

const BoundingBox& Mesh::getBoundingBox() const {
    return mBoundingBox;
}

void Mesh::initBuffers(uint detailLevel) {
    glBindVertexArray(mVAO);

    // indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[0]);
    glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            mIndices[detailLevel].size() * sizeof (mIndices[detailLevel][0]),
            mIndices[detailLevel].data(),
            GL_STATIC_DRAW
            );

    // vertices
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(
            GL_ARRAY_BUFFER,
            mVertices[detailLevel].size() * sizeof (mVertices[detailLevel][0]),
            mVertices[detailLevel].data(),
            GL_STATIC_DRAW
            );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
            0, // attribute 0 (must match layout in shader)
            3, // size of one attribute (3 float = 1 vertex)
            GL_FLOAT, // type in buffer
            GL_FALSE, // normalized?
            0, // stride
            0 // array buffer offset (0 = start)
            );

    // normals
    if (hasNormals) {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
        glBufferData(
                GL_ARRAY_BUFFER,
                mNormals[detailLevel].size() * sizeof (mNormals[detailLevel][0]),
                mNormals[detailLevel].data(),
                GL_STATIC_DRAW
                );

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    // texCoords
    if (hasTexCoords) {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
        glBufferData(
                GL_ARRAY_BUFFER,
                mTexCoords[detailLevel].size() * sizeof (mTexCoords[detailLevel][0]),
                mTexCoords[detailLevel].data(),
                GL_STATIC_DRAW
                );

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    // compute tangents and bitangents
    if (hasNormals && hasTexCoords) {
        // tangents
        glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
        glBufferData(
                GL_ARRAY_BUFFER,
                mTangents[detailLevel].size() * sizeof (mTangents[detailLevel][0]),
                mTangents[detailLevel].data(),
                GL_STATIC_DRAW
                );

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // biTangents
        glBindBuffer(GL_ARRAY_BUFFER, buffers[5]);
        glBufferData(
                GL_ARRAY_BUFFER,
                mBitangents[detailLevel].size() * sizeof (mBitangents[detailLevel][0]),
                mBitangents[detailLevel].data(),
                GL_STATIC_DRAW
                );

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    glBindVertexArray(0);

    mCurrentDetailLevel = detailLevel;
}

void Mesh::initContinuousLOD() {

    glGenBuffers(GLsizei(continuousLODBuffers.size()), continuousLODBuffers.data());

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, continuousLODBuffers[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
            mCollapseHierarchy.size() * sizeof (GLuint),
            mCollapseHierarchy.data(),
            GL_STATIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, continuousLODBuffers[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
            mHierarchyInformation.size() * sizeof (HierarchyInformation),
            mHierarchyInformation.data(),
            GL_STATIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, continuousLODBuffers[2]);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
            mCollapsePlacement.size() * sizeof (GLfloat),
            mCollapsePlacement.data(),
            GL_STATIC_DRAW);
}

void Mesh::setVertices(const std::vector<float>& vertices) {
    mVertices.push_back(vertices);
}

void Mesh::setNormals(const std::vector<float>& normals) {
    mNormals.push_back(normals);
    if (mNormals.size() > 0) {
        hasNormals = true;
    }
}

void Mesh::setTexCoords(const std::vector<float>& texCoords) {
    mTexCoords.push_back(texCoords);
    if (mTexCoords.size() > 0) {
        hasTexCoords = true;
    }
}

void Mesh::setIndices(const std::vector<unsigned int>& indices) {
    mIndices.push_back(indices);
}

void Mesh::setTangents(const std::vector<float>& tangents) {
    mTangents.push_back(tangents);
}

void Mesh::setBitangents(const std::vector<float>& bitangents) {
    mBitangents.push_back(bitangents);
}

void Mesh::addLevelError(float error) {
    mLevelErrors.push_back(error);
}

unsigned long Mesh::draw() {
    return drawDetailLevel(0);
}

unsigned long Mesh::drawDetailLevel(uint detailLevel) {
    
    if(detailLevel > mVertices.size() - 1) {
        detailLevel = mVertices.size() - 1;
    }
    
    if (mCurrentDetailLevel == detailLevel) {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[NORMALS]);
        glBindVertexArray(mVAO);

        glDrawElements(
                GL_TRIANGLES,
                GLsizei(mIndices[detailLevel].size()),
                GL_UNSIGNED_INT,
                nullptr
                );

        glBindVertexArray(0);

        return mIndices[detailLevel].size() / 3;

    } 
    
    else {
        initBuffers(detailLevel);
        return 0;
    }
}

unsigned long Mesh::drawThresholdLevel(GLuint minPolygons) {

    uint level;

    for (uint i = 0; i < mIndices.size(); i++) {

        if ((mIndices[i].size() / 3) > minPolygons) {
            level = i;
        } else {
            break;
        }
    }

    return drawDetailLevel(level);
}

unsigned long Mesh::drawContinuousLevel(GLuint detailLevel) {

    if (mCurrentDetailLevel == 0) {

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, continuousLODBuffers[0]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, continuousLODBuffers[1]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, continuousLODBuffers[2]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, buffers[NORMALS]);

        glBindVertexArray(mVAO);

        GLuint numIndices = mIndices[0].size() - 6 * detailLevel;

        //Remove two faces for every level
        glDrawElements(
                GL_TRIANGLES,
                GLsizei(numIndices),
                GL_UNSIGNED_INT,
                nullptr
                );

        glBindVertexArray(0);

        return numIndices / 3;

    } else {
        initBuffers(0);
        return 0;
    }

}

unsigned long Mesh::drawViewDependentLevel(GLuint detailLevel) {

    if (mCurrentDetailLevel == 0) {

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, continuousLODBuffers[0]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, continuousLODBuffers[1]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, continuousLODBuffers[2]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, buffers[NORMALS]);

        glBindVertexArray(mVAO);

        glDrawElements(
                GL_TRIANGLES,
                GLsizei(mIndices[0].size()),
                GL_UNSIGNED_INT,
                nullptr
                );

        glBindVertexArray(0);

        return 0; //TODO Return correct polygon count?

    } else {
        initBuffers(0);
        return 0;
    }
}

unsigned long Mesh::getPolygonCount() {
    return mIndices[0].size() / 3;
}

unsigned long Mesh::getMinPolygons() {
    return mIndices.back().size() / 3;
}

void Mesh::cleanUp() {
    if (!cleaned) {
        glDeleteBuffers(GLsizei(buffers.size()), buffers.data());
        glDeleteVertexArrays(1, &mVAO);
        cleaned = true;
    }
}

void Mesh::computeBoundingVolumes() {
    float xMin = std::numeric_limits<float>::max();
    float xMax = std::numeric_limits<float>::min();
    float yMin = std::numeric_limits<float>::max();
    float yMax = std::numeric_limits<float>::min();
    float zMin = std::numeric_limits<float>::max();
    float zMax = std::numeric_limits<float>::min();

    int vCount = (unsigned int) mVertices.size();
    for (int i = 0; i < vCount; i += 3) {
        glm::vec3 v{mVertices[0][i], mVertices[0][i + 1], mVertices[0][i + 2]};
        if (v.x < xMin) xMin = v.x;
        if (v.x > xMax) xMax = v.x;
        if (v.y < yMin) yMin = v.y;
        if (v.y > yMax) yMax = v.y;
        if (v.z < zMin) zMin = v.z;
        if (v.z > zMax) zMax = v.z;
    }

    glm::vec3 center;
    center.x = (xMin + xMax) / 2.f;
    center.y = (yMin + yMax) / 2.f;
    center.z = (zMin + zMax) / 2.f;

    // if we would take the minima and maxima for our bounding sphere radius calculation,
    // we would actually calculate the bounding sphere for the bounding box - which is not what we want.
    float maxLen = 0;
    glm::vec3 maxVec;
    for (int i = 0; i < vCount; i += 3) {
        glm::vec3 p{mVertices[0][i], mVertices[0][i + 1], mVertices[0][i + 2]};
        auto v = p - center;

        // use squared distance
        float len = v.x * v.x + v.y * v.y + v.z * v.z;
        if (len > maxLen) {
            maxLen = len;
            maxVec = v;
        }
    }

    float radius = glm::length(maxVec);

    mBoundingSphere = BoundingSphere(center, radius);
    auto minCorner = glm::vec3(xMin, yMin, zMin);
    auto maxCorner = glm::vec3(xMax, yMax, zMax);
    mBoundingBox = BoundingBox(minCorner, maxCorner - minCorner);
}
