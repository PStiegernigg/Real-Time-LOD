#pragma once

#include "cameras/Camera.h"

namespace camera {

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(glm::vec3 cop, float fov, float aspect, float near, float far);
    virtual ~PerspectiveCamera();
private:
    float aspectRatio;
    void updateProjectionMatrix() override;
};

} // namespace camera