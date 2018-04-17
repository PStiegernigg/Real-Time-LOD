/* 
 * File:   Plane.h
 * Author: tport
 *
 * Created on March 22, 2014, 5:28 PM
 */

#ifndef PLANE_H
#define	PLANE_H

#include <glm/glm.hpp>

class Plane {
public:
    Plane();
    Plane(float dist, glm::vec3 normal);
    Plane(const Plane& orig);
    virtual ~Plane();
    const float& getDistance() const;
    const glm::vec3& getNormal() const;
private:
    float dist;
    glm::vec3 normal;
};

#endif	/* PLANE_H */

