#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "cameras/Camera.h"

camera::Camera::Camera(glm::vec3 cop, float near, float far)
    : centerOfProjection(cop)
    , nearPlane(near)
    , farPlane(far)
    , fieldOfView{ 0.f }
    , cameraMatrix{1.f}
    , projectionMatrix{ 1.f }
    , viewProjection{ 1.f }
    , invViewProjection{ 1.f }
{
}

const glm::mat4& camera::Camera::getViewProjection() const {
    return viewProjection;
}

const glm::mat4& camera::Camera::getProjection() const {
    return projectionMatrix;
}

const glm::mat4& camera::Camera::getView() const {
    return cameraMatrix;
}

void camera::Camera::updateCameraMatrix() {
    cameraMatrix = glm::translate(glm::mat4(1.f), centerOfProjection);
    cameraMatrix = cameraMatrix * glm::toMat4(mOrientation);
    cameraMatrix = glm::inverse(cameraMatrix);

    viewProjection = projectionMatrix * cameraMatrix;
    invViewProjection = glm::inverse(viewProjection);
    updateCullingPlanes();
}

void camera::Camera::updateCullingPlanes() {
    // Frustum corners in world space
    frustumCorners[0] = glm::vec3{-1.f, -1.f, -1.f};
    frustumCorners[1] = glm::vec3{-1.f, -1.f, 1.f};
    frustumCorners[2] = glm::vec3{-1.f, 1.f, -1.f};
    frustumCorners[3] = glm::vec3{1.f, -1.f, -1.f};
    frustumCorners[4] = glm::vec3{1.f, 1.f, -1.f};
    frustumCorners[5] = glm::vec3{1.f, -1.f, 1.f};
    frustumCorners[6] = glm::vec3{-1.f, 1.f, 1.f};
    frustumCorners[7] = glm::vec3{1.f, 1.f, 1.f};

    glm::mat4 ndcToWorld = invViewProjection;
    for (unsigned int i = 0; i < 8; i++) {
        glm::vec4 tmp{frustumCorners[i], 1.f};
        tmp = ndcToWorld * tmp;
        tmp /= tmp.w;
        frustumCorners[i] = glm::vec3{tmp.x, tmp.y, tmp.z};
    }
    // leftPlane
    glm::vec3 e0{frustumCorners[6] - frustumCorners[1]};
    glm::vec3 e1{frustumCorners[0] - frustumCorners[1]};
    glm::vec3 normal = glm::normalize(glm::cross(e0, e1));
    float dist = glm::dot(frustumCorners[1], normal);
    cullingPlanes[0] = Plane{dist, normal};

    // rightPlane
    e0 = glm::vec3{frustumCorners[4] - frustumCorners[3]};
    e1 = glm::vec3{frustumCorners[5] - frustumCorners[3]};
    normal = glm::normalize(glm::cross(e0, e1));
    dist = glm::dot(frustumCorners[3], normal);
    cullingPlanes[1] = Plane{dist, normal};

    // bottomPlane
    e0 = glm::vec3{frustumCorners[0] - frustumCorners[1]};
    e1 = glm::vec3{frustumCorners[5] - frustumCorners[1]};
    normal = glm::normalize(glm::cross(e0, e1));
    dist = glm::dot(frustumCorners[1], normal);
    cullingPlanes[2] = Plane{dist, normal};

    // topPlane
    e0 = glm::vec3{frustumCorners[4] - frustumCorners[7]};
    e1 = glm::vec3{frustumCorners[6] - frustumCorners[7]};
    normal = glm::normalize(glm::cross(e0, e1));
    dist = glm::dot(frustumCorners[7], normal);
    cullingPlanes[3] = Plane{dist, normal};

    // nearPlane
    e0 = glm::vec3{frustumCorners[7] - frustumCorners[5]};
    e1 = glm::vec3{frustumCorners[1] - frustumCorners[5]};
    normal = glm::normalize(glm::cross(e0, e1));
    dist = glm::dot(frustumCorners[5], normal);
    cullingPlanes[4] = Plane{dist, normal};

    // farPlane
    e0 = glm::vec3{frustumCorners[2] - frustumCorners[0]};
    e1 = glm::vec3{frustumCorners[3] - frustumCorners[0]};
    normal = glm::normalize(glm::cross(e0, e1));
    dist = glm::dot(frustumCorners[0], normal);
    cullingPlanes[5] = Plane{dist, normal};
}

void camera::Camera::move(glm::vec3 moveVector) {
    centerOfProjection += mOrientation * moveVector;
    updateCameraMatrix();
}

void camera::Camera::rotate(float rotSpeed, float deltaX, float deltaY) {
    mOrientation *= glm::angleAxis(rotSpeed, glm::vec3(-deltaY, 0, 0));
    mOrientation = glm::angleAxis(rotSpeed, glm::vec3(0, -deltaX, 0)) * mOrientation;
    mOrientation = glm::normalize(mOrientation);
    updateCameraMatrix();
}

bool camera::Camera::inFrustum(const BoundingSphere& bSphere) const {
    for (unsigned int i = 0; i < 6; i++) {
        const Plane& p = cullingPlanes[i];
        float dist = glm::dot(bSphere.getCenter(), p.getNormal()) - p.getDistance();
        if (dist < -bSphere.getRadius())
            return false;
    }
    return true;
}

const glm::vec3 camera::Camera::getCOP() const {
    return centerOfProjection;
}

const glm::quat camera::Camera::getOrientation() const {
    return mOrientation;
}

void camera::Camera::setOrientation(glm::quat orientation) {
    mOrientation = orientation;
}

void camera::Camera::setCOP(const glm::vec3& newCOP) {
    centerOfProjection = newCOP;
    updateCameraMatrix();
}

const float camera::Camera::getNear() const {
    return nearPlane;
}

const float camera::Camera::getFar() const {
    return farPlane;
}

void camera::Camera::passUniforms(std::shared_ptr<Shader> shader) const {
    shader->setUniform("camera.cop", centerOfProjection);
    shader->setUniform("camera.invVP", invViewProjection);
}
