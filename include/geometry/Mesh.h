/* 
 * @author Philipp Stiegernigg
 * Based on original implementation by tport and Philipp Salzmann.
 */

#pragma once

#include <vector>
#include <array>
#include <memory>
#include <GL/glew.h>
#include "geometry/BoundingSphere.h"
#include "geometry/BoundingBox.h"
#include "rendering/Shader.h"

enum MeshBufferIndex {
    INDICES = 0,
    VERTICES = 1,
    NORMALS = 2,
    TEXCOORD = 3,
    TANGENTS = 4,
    BITANGENTS = 5,
};

struct HierarchyInformation {
    GLuint startPtr;
    GLuint currentCollapse;
};

class Mesh {
    friend class LODGenerator;
    
public:
    Mesh();
    virtual ~Mesh();
    
    unsigned long draw();
    unsigned long drawDetailLevel(uint detailLevel);
    unsigned long drawThresholdLevel(GLuint minPolygons);
    unsigned long drawContinuousLevel(GLuint detailLevel);
    unsigned long drawViewDependentLevel(GLuint detailLevel);
    void cleanUp();
    bool hasNormals, hasTexCoords, hasLOD, cleaned;
    const BoundingSphere& getBoundingSphere() const;
    const BoundingBox& getBoundingBox() const;
    void initBuffers(uint detailLevel);
    void initContinuousLOD();

    void computeBoundingVolumes();

    void setVertices(const std::vector<float>& vertices);
    void setNormals(const std::vector<float>& normals);
    void setTexCoords(const std::vector<float>& texCoords);
    void setIndices(const std::vector<unsigned int>& indices);
    void setTangents(const std::vector<float>& tangents);
    void setBitangents(const std::vector<float>& bitangents);
    void addLevelError(float error);
    unsigned long getPolygonCount();
    unsigned long getMinPolygons();

private:
    std::vector<std::vector<float>> mVertices;
    std::vector<std::vector<float>> mNormals;
    std::vector<std::vector<float>> mTexCoords;
    std::vector<std::vector<float>> mTangents;
    std::vector<std::vector<float>> mBitangents;
    std::vector<std::vector<unsigned int>> mIndices;
    
    std::vector<float> mLevelErrors;
    std::vector<GLuint> mCollapseHierarchy;
    std::vector<HierarchyInformation> mHierarchyInformation;
    std::vector<GLfloat> mCollapsePlacement;
    
    GLuint mVAO;
    /*
     * [0] indices
     * [1] vertices
     * [2] normals
     * [3] texCoords
     * [4] tangents
     * [5] bitangents 
     */
    std::array<GLuint, 6> buffers;
    std::array<GLuint, 5> continuousLODBuffers;
    BoundingSphere mBoundingSphere;
    BoundingBox mBoundingBox;
    uint mCurrentDetailLevel;
};
