/**
 * @author tport
 */

#pragma once

#define _USE_MATH_DEFINES
#include <array>
#include <memory>
#include <GL/glew.h>

#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "geometry/Plane.h"
#include "geometry/BoundingSphere.h"
#include "rendering/Shader.h"

namespace camera {

class Camera {
public:
    Camera(glm::vec3 cop, float near, float far);
    const glm::mat4& getViewProjection() const;
    const glm::mat4& getProjection() const;
    const glm::mat4& getView() const;

    /**
     * Moves the camera according to its local coordinate space.
     */
    void move(glm::vec3 moveVector);
    void rotate(float rotSpeed, float deltaX, float deltaY);
    bool inFrustum(const BoundingSphere& bSphere) const;
    const glm::vec3 getCOP() const;
    virtual void setCOP(const glm::vec3& newCOP);
    const glm::quat getOrientation() const;
    void setOrientation(glm::quat orientation);
    const float getNear() const;
    const float getFar() const;
    void passUniforms(std::shared_ptr<Shader> shader) const;
protected:
    glm::quat mOrientation;
    glm::vec3 centerOfProjection;

    float nearPlane, farPlane, fieldOfView;

    glm::mat4 cameraMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 viewProjection;
    glm::mat4 invViewProjection;

    std::array<Plane, 6> cullingPlanes;
    std::array<glm::vec3, 8> frustumCorners;

    void updateCameraMatrix();
    void updateCullingPlanes();
    virtual void updateProjectionMatrix() = 0;
};

} // namespace camera
