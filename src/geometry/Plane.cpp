/* 
 * File:   Plane.cpp
 * Author: tport
 * 
 * Created on March 22, 2014, 5:28 PM
 */

#include "geometry/Plane.h"

Plane::Plane()
: dist{0.f},
normal{0.f} {
    
}

Plane::Plane(float dist, glm::vec3 normal)
: dist{dist},
normal{normal} {
}

Plane::Plane(const Plane& orig) {
}

Plane::~Plane() {
}

const float& Plane::getDistance() const {
    return dist;
}

const glm::vec3& Plane::getNormal() const {
    return normal;
}