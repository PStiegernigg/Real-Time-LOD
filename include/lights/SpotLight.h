/* 
 * File:   SpotLight.h
 * Author: tport
 *
 * Created on April 9, 2014, 11:18 PM
 */

#pragma once
#define _USE_MATH_DEFINES
#include "lights/Light.h"
#include "cameras/PerspectiveCamera.h"
using camera::Camera;
using camera::PerspectiveCamera;

namespace light {

class SpotLight : public Light {
public:
    SpotLight(float nearClip, float farClip);
    ~SpotLight();

    LightType getType() const override;

    void setPosition(const glm::vec3& newPos) override;
    void setOrientation(const glm::quat& orientation) override;

    void passUniforms(std::shared_ptr<Shader> shader) const override;
    void transferSettings(const std::shared_ptr<Light> other) override;
    void addGUIVariables(std::shared_ptr<GUIBar> lightBar) override;
    std::shared_ptr<Camera> getCamera() const override;

protected:
    float mCutoffAngle;
    float mSpotExponent;

private:
    glm::mat4 mBiasMatrix;
    glm::mat4 mShadowMatrix;
    std::shared_ptr<Camera> mLightCamera;
};

} // namespace light

