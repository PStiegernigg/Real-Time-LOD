/**
 * A shadow-mapped directionial light source.
 *
 * @author Philip Salzmann
 */

#pragma once

#include "lights/Light.h"
#include "cameras/OrthoCamera.h"
using camera::OrthoCamera;
#include "rendering/Shader.h"

namespace light {

class DirectionalLight : public Light {
public:
    DirectionalLight();
    virtual ~DirectionalLight();

    LightType getType() const override;
    virtual void passUniforms(std::shared_ptr<Shader> shader) const override;
    std::shared_ptr<Camera> getCamera() const override;
private:
    glm::mat4       mBiasMatrix;
    glm::mat4       mShadowMatrix;
    std::shared_ptr<Camera> mLightCamera;
};

} // namespace light
