#include "lights/SpotLight.h"

light::SpotLight::SpotLight(float nearClip, float farClip)
    : mCutoffAngle{90.f}
    , mSpotExponent{15.f}
    // scale camera VP by 0.5 and translate from [-0.5, 0.5] to [0, 1] (UV range)
    , mBiasMatrix{
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    }
{
    mLightCamera = std::make_shared<PerspectiveCamera>(
        glm::vec3(0),
        mCutoffAngle,
        1.f,
        nearClip,
        farClip
        );

    mShadowMatrix = mBiasMatrix * mLightCamera->getViewProjection();
}

light::SpotLight::~SpotLight() {
}

light::LightType light::SpotLight::getType() const {
    return LightType::SPOTLIGHT;
}

void light::SpotLight::setPosition(const glm::vec3& newPos) {
    Light::setPosition(newPos);
    mLightCamera->setCOP(newPos);
    mShadowMatrix = mBiasMatrix * mLightCamera->getViewProjection();
}

void light::SpotLight::setOrientation(const glm::quat& orientation) {
    Light::setOrientation(orientation);
    mLightCamera->setOrientation(orientation);
}

void light::SpotLight::transferSettings(const std::shared_ptr<Light> other) {
    Light::transferSettings(other);
    
    auto otherSpotlight = dynamic_cast<SpotLight*>(other.get());
    if (otherSpotlight != nullptr) {
        mCutoffAngle = otherSpotlight->mCutoffAngle;
        mSpotExponent = otherSpotlight->mSpotExponent;
    }
}

void light::SpotLight::addGUIVariables(std::shared_ptr<GUIBar> lightBar) { 
    Light::addGUIVariables(lightBar);

    lightBar->addField<float>(GUIBarOptions("Cutoff angle").group("Spot").min(0).step(0.05), [this](){ return mCutoffAngle; }, [this](float value) { mCutoffAngle = value; });
    lightBar->addField<float>(GUIBarOptions("Exponent").group("Spot").min(0).step(0.05), [this](){ return mSpotExponent; }, [this](float value) { mSpotExponent = value; });
}

std::shared_ptr<Camera> light::SpotLight::getCamera() const {
    return mLightCamera;
}

void light::SpotLight::passUniforms(std::shared_ptr<Shader> shader) const {
    Light::passUniforms(shader);

    glm::vec3 direction = glm::normalize(mOrientation * glm::vec3(0.f, 0.f, 1.f));
    shader->setUniform("light.direction", direction);
    shader->setUniform("light.cosCutoffAngle", (float) cos(mCutoffAngle * (float)M_PI / 180.f / 2.f));
    shader->setUniform("light.exponent", mSpotExponent);
    shader->setUniform("light.biasedVP", mShadowMatrix);
}
