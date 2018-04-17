/* 
 * File:   Instance.cpp
 * Author: tport
 * 
 * Created on March 11, 2014, 7:11 PM
 */

#include <iostream>
#include "geometry/Instance.h"

#define GLM_FORCE_RADIANS
#include "glm/gtc/quaternion.hpp"
#include "config.h"

Instance::Instance(std::shared_ptr<Mesh> mesh)
: mMesh {
    mesh
}
, mModelMatrix{1.f}
, mNormalMatrix{1.f}
, mBoundingSphere{mesh->getBoundingSphere()}
, mBoundingBox{mesh->getBoundingBox()}
, mDistance{0.0f}
, mVecToCamera{glm::vec3(0.0f)}
{
}

/**
 * Move constructor, needed since we have unique_ptr as member
 */
Instance::Instance(Instance&& orig)
: mMesh {
    orig.mMesh
}

, mMaterial(std::move(orig.mMaterial))
, mModelMatrix {
    orig.mModelMatrix
}
, mNormalMatrix{orig.mNormalMatrix}
, mBoundingSphere{orig.mBoundingSphere}
, mDistance{0.0f}
, mVecToCamera{glm::vec3(0.0f)}
, mCurrentLevel{0}
{
}

Instance::~Instance() {
}

void Instance::setModelMatrix(glm::mat4 m) {
    mModelMatrix = m;
    mNormalMatrix = glm::transpose(glm::inverse(m));
    mBoundingSphere.setTransformationMatrix(mModelMatrix);
    mBoundingBox.transform(mModelMatrix);
}

const glm::mat4& Instance::getModelMatrix() const {
    return mModelMatrix;
}

const glm::mat4& Instance::getNormalMatrix() const {
    return mNormalMatrix;
}

const std::shared_ptr<Material> Instance::getMaterial() const {
    return mMaterial;
}

const std::shared_ptr<Mesh> Instance::getMesh() const {
    return mMesh;
}

void Instance::setMaterial(std::shared_ptr<Material> material) {
    mMaterial = material;
}

const BoundingSphere& Instance::getBoundingSphere() const {
    return mBoundingSphere;
}

const BoundingBox& Instance::getBoundingBox() const {
    return mBoundingBox;
}

void Instance::cleanUp() {
    mMesh->cleanUp();
}

void Instance::updateCameraAttributes(const glm::mat4& projectionViewMatrix) {

    glm::vec3 cameraPos = glm::vec3{glm::inverse(projectionViewMatrix) * glm::vec4 {0, 0, 0, 1}};
    glm::vec3 objectPos = glm::vec3{mModelMatrix * glm::vec4 {mBoundingSphere.getCenter(), 1}};
    glm::vec3 vecToCamera = cameraPos - objectPos;

    mVecToCamera = glm::normalize(vecToCamera);
    mDistance = sqrt(glm::dot(vecToCamera, vecToCamera)) - mBoundingSphere.getRadius();

    float maxError = (Config::PIXEL_ERROR_THRESHOLD * mDistance * tan(Config::FIELD_OF_VIEW / 2))
            / Config::SCREEN_RESOLUTION;

    if (maxError > Config::MAX_MODEL_ERROR) {
        maxError = Config::MAX_MODEL_ERROR;
    }

    GLuint level = maxError * getMesh()->getPolygonCount() / 2;
    GLuint maxLevel = (getMesh()->getPolygonCount() - getMesh()->getMinPolygons()) / 2;

    if (level > maxLevel) {
        mCurrentLevel = maxLevel;
    } else {
        mCurrentLevel = level;
    }
}

double Instance::getDistancefromViewer() {
    return mDistance;
}

glm::vec3 Instance::getVecToCamera() {
    return mVecToCamera;
}

GLuint Instance::getCurrentLevel() {
    return mCurrentLevel;
}
