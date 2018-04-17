#include "lights/DirectionalLight.h"

light::DirectionalLight::DirectionalLight()
    // bias matrix for transforming from NDC to UV coordinates
    : mBiasMatrix{
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    }
{
    mLightCamera = std::make_shared<OrthoCamera>(
        glm::vec3(0.f, 0.f, 10.f),
        // FIXME: hardcoded frustum & clipping
        // sponza: 30x30
        // virtamed_1: 15x15
        15,
        15,
        0.1f,
        300.f
        );

    mShadowMatrix = mBiasMatrix * mLightCamera->getViewProjection();
}

light::DirectionalLight::~DirectionalLight() {}

light::LightType light::DirectionalLight::getType() const {
    return LightType::DIRECTIONALLIGHT;
}

void light::DirectionalLight::passUniforms(std::shared_ptr<Shader> shader) const {
    Light::passUniforms(shader);

    glm::vec3 direction = glm::normalize(mLightCamera->getOrientation() * glm::vec3(0.f, 0.f, 1.f));
    shader->setUniform("light.direction", direction);

    // FIXME: Since DirectionalLight does not move the light camera, mShadowMatrix is never updated.
    //shader->setUniform("light.biasedVP", mShadowMatrix);
    shader->setUniform("light.biasedVP", mBiasMatrix * mLightCamera->getViewProjection());
}

std::shared_ptr<Camera> light::DirectionalLight::getCamera() const {
    return mLightCamera;
}
