#include "cameras/PerspectiveCamera.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

camera::PerspectiveCamera::PerspectiveCamera(glm::vec3 cop, float fov, float aspect, float near, float far)
    : Camera(cop, near, far)
    , aspectRatio(aspect)
{
    fieldOfView = glm::radians(fov);
    
    updateProjectionMatrix();
    updateCameraMatrix();
}

camera::PerspectiveCamera::~PerspectiveCamera() {
}

void camera::PerspectiveCamera::updateProjectionMatrix() {
    projectionMatrix = glm::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);

    viewProjection = projectionMatrix * cameraMatrix;
    invViewProjection = glm::inverse(viewProjection);
    updateCullingPlanes();
}
