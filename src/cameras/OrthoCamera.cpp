/* 
 * File:   OrthoCamera.cpp
 * Author: tport
 * 
 * Created on 4. März 2014, 23:29
 */

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include "cameras/OrthoCamera.h"

camera::OrthoCamera::OrthoCamera(glm::vec3 cop, unsigned int frustumWidth, unsigned int frustumHeight, float near, float far)
    : Camera(cop, near, far)
    , mFrustumWidth(frustumWidth)
    , mFrustumHeight(frustumHeight)
{
    updateProjectionMatrix();
    updateCameraMatrix();
}

camera::OrthoCamera::~OrthoCamera() {
}

void camera::OrthoCamera::updateProjectionMatrix() {
    projectionMatrix = glm::ortho<float>(
        (float)mFrustumWidth / -2.f,
        (float)mFrustumWidth / 2.f,
        (float)mFrustumHeight / -2.f,
        (float)mFrustumHeight / 2.f,
        nearPlane,
        farPlane);

    viewProjection = projectionMatrix * cameraMatrix;
    invViewProjection = glm::inverse(viewProjection);
    updateCullingPlanes();
}