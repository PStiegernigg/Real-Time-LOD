/* 
 * File:   BoundingSphere.h
 * Author: tport
 *
 * Created on March 21, 2014, 1:19 PM
 */

#pragma once
#include <glm/glm.hpp>

class BoundingSphere {
public:
    BoundingSphere(glm::vec3 center, float radius);
    BoundingSphere(const BoundingSphere& orig);
    virtual ~BoundingSphere();

    const glm::vec3& getCenter() const;
    float getRadius() const;
    void setTransformationMatrix(const glm::mat4& m);
private:
    glm::vec3 mCenter, mTransformedCenter;
    float mRadius, mTransformedRadius;
    glm::mat4 mTransformationMatrix;
};
