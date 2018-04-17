/**
 * An axis-aligned bounding box (AABB).
 * @author Philip Salzmann
 */

#pragma once

#include "geometry/BoundingSphere.h"

class BoundingBox {
public:
    BoundingBox();
    BoundingBox(const glm::vec3& minCorner, const glm::vec3& dimensions);
    BoundingBox(const BoundingBox& orig);
    virtual ~BoundingBox();

    // increases the size of the bounding box (if necessary),
    // in order to include the given box
    void include(const BoundingBox& bBox);

    // creates a "new" bounding box by first transforming
    // the existing one, and then resizing it to wrap itself.
    // (in order to stay axis-aligned!)
    void transform(const glm::mat4& matrix);
    glm::vec3 getMinCorner() const;
    glm::vec3 getDimensions() const;
private:
    glm::vec3 mMinCorner;
    glm::vec3 mDimensions;
};
