#include "lights/Light.h"

light::Light::Light()
    : mPosition{0.f}
    , mColor{1.f}
    , mAmbientColor{1.f}
    , mPower{2.f}
    , mAmbientPower{0.f}
{
}

light::Light::~Light() {}

bool light::Light::isAtCameraPosition() const {
    return mAtCameraPosition;
}

void light::Light::setAtCameraPosition(bool value) {
    mAtCameraPosition = value;
}

void light::Light::transferSettings(const std::shared_ptr<Light> other) {
    mOrientation = other->mOrientation;
    mPosition = other->mPosition;
    mColor = other->mColor;
    mAmbientColor = other->mAmbientColor;
    mPower = other->mPower;
    mAmbientPower = other->mAmbientPower;
    mAtCameraPosition = other->mAtCameraPosition;
}

float light::Light::getPower() const {
    return mPower;
}

float light::Light::getAmbientPower() const {
    return mAmbientPower;
}

void light::Light::setPower(float value) {
    mPower = value;
}

void light::Light::setAmbientPower(float value) {
    mAmbientPower = value;
}

bool light::Light::init() {
    return true;
}

void light::Light::setPosition(const glm::vec3& position) {
    mPosition = position;
}

glm::vec3 light::Light::getPosition() const {
    return mPosition;
}

void light::Light::setOrientation(const glm::quat& orientation) {
    mOrientation = orientation;
}

glm::quat light::Light::getOrientation() const {
    return mOrientation;
}

void light::Light::addGUIVariables(std::shared_ptr<GUIBar> lightBar) {
    lightBar->addField<bool>(GUIBarOptions("Light at Camera"), [this](){ return mAtCameraPosition; }, [this](bool value) { mAtCameraPosition = value; });
    lightBar->addField<float>(GUIBarOptions("Light Power").min(0).step(0.05), [this](){ return mPower; }, [this](float value) { mPower = value; });
    lightBar->addField<float>(GUIBarOptions("Ambient Power").min(0).step(0.005), [this](){ return mAmbientPower; }, [this](float value) { mAmbientPower = value; });
}

void light::Light::passUniforms(std::shared_ptr<Shader> shader) const {
    shader->setUniform("light.power", mPower);
    shader->setUniform("light.ambientPower", mAmbientPower);
    shader->setUniform("light.color", mColor);
    shader->setUniform("light.ambientColor", mAmbientColor);
    shader->setUniform("light.position", mPosition);
    auto camera = getCamera();
    if (camera != nullptr) {
        shader->setUniform("light.view", camera->getView());
    }
}

std::shared_ptr<Camera> light::Light::getCamera() const {
    return nullptr;
}
