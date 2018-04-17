/**
 * An Instance is made up of a Mesh, a Material and a transformation matrix.
 *
 * @author Philipp Stiegernigg
 * Based on original implementation by tport and Philip Salzmann.
 */

#pragma once

#include <memory>
#include "geometry/Mesh.h"
#include "materials/Material.h"

class Instance {
public:
    Instance(std::shared_ptr<Mesh> mesh);
    Instance(Instance&& orig);
    virtual ~Instance();
    void setModelMatrix(glm::mat4 m);
    void setMaterial(std::shared_ptr<Material> material);
    const glm::mat4& getModelMatrix() const;
    const glm::mat4& getNormalMatrix() const;
    const std::shared_ptr<Material> getMaterial() const;
    void cleanUp();
    const BoundingSphere& getBoundingSphere() const;
    const BoundingBox& getBoundingBox() const;
    const std::shared_ptr<Mesh> getMesh() const;
    void updateCameraAttributes(const glm::mat4& projectionViewMatrix);
    double getDistancefromViewer();
    GLuint getCurrentLevel();
    glm::vec3 getVecToCamera();
    
private:
    std::shared_ptr<Mesh> mMesh;
    std::shared_ptr<Material> mMaterial;
    glm::mat4 mModelMatrix, mNormalMatrix;
    BoundingSphere mBoundingSphere;
    BoundingBox mBoundingBox;
    
    double mDistance;
    glm::vec3 mVecToCamera;
    GLuint mCurrentLevel;
};
