/* 
 * File:   BoundingSphere.cpp
 * Author: tport
 * 
 * Created on March 21, 2014, 1:19 PM
 */

#include "geometry/BoundingSphere.h"

BoundingSphere::BoundingSphere(glm::vec3 center, float radius)
    : mCenter {center}
    , mRadius{radius}
{
    setTransformationMatrix(glm::mat4(1.f));
}

BoundingSphere::BoundingSphere(const BoundingSphere& orig)
    : mCenter {orig.mCenter}
    , mTransformedCenter{orig.mTransformedCenter}
    , mRadius{orig.mRadius}
    , mTransformedRadius{orig.mTransformedRadius}
    , mTransformationMatrix{orig.mTransformationMatrix}
    
{    
} 

BoundingSphere::~BoundingSphere() {
}

const glm::vec3& BoundingSphere::getCenter() const {
    return mTransformedCenter;
}
float BoundingSphere::getRadius() const {
    return mTransformedRadius;
}

void BoundingSphere::setTransformationMatrix(const glm::mat4& m) {
    mTransformedCenter = glm::vec3(m * glm::vec4(mCenter, 1.f));

    glm::vec4 point = m * glm::vec4(glm::normalize(glm::vec3{ 1.f, 1.f, 1.f }) * mRadius, 0.f);
    mTransformedRadius = glm::length(point);
}
