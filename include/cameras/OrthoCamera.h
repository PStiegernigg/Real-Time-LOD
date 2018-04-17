/* 
 * File:   OrthoCamera.h
 * Author: tport
 *
 * Created on 4. März 2014, 23:29
 */

#pragma once

#include "cameras/Camera.h"

namespace camera {

class OrthoCamera : public Camera {
public:
    OrthoCamera(glm::vec3 cop, unsigned int mFrustumWidth, unsigned int mFrustumHeight, float near, float far);
    virtual ~OrthoCamera();
private:
    unsigned int mFrustumWidth;
    unsigned int mFrustumHeight;

    void updateProjectionMatrix() override;
};

} // namespace camera