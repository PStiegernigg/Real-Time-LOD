/* 
 * Abstract base class for all lights.
 *
 * @author Philip Salzmann
 * Based on original implementation by tport.
 */

#pragma once

#include "cameras/Camera.h"
using camera::Camera;
#include <ui/GUIBar.h>
#include "geometry/Scene.h"
#include "util/Enumerators.h"

namespace light {

class Light {
public:
    Light();
    virtual ~Light() = 0;

    virtual LightType getType() const = 0;

    // copies the settings (color, power..) from *other* to this light
    virtual void transferSettings(const std::shared_ptr<Light> other);

    virtual float getPower() const;
    virtual float getAmbientPower() const;
    virtual void setPower(float value);
    virtual void setAmbientPower(float value);

    bool isAtCameraPosition() const;
    virtual glm::vec3 getPosition() const;
    virtual glm::quat getOrientation() const;
    virtual std::shared_ptr<Camera> getCamera() const;

    void setAtCameraPosition(bool value);
    virtual void setPosition(const glm::vec3& position);
    virtual void setOrientation(const glm::quat& orientation);

    virtual bool init();
    virtual void addGUIVariables(std::shared_ptr<GUIBar> lightBar);
    virtual void passUniforms(std::shared_ptr<Shader> shader) const;

protected:
    glm::quat mOrientation;
    glm::vec3 mPosition;

    glm::vec3 mColor, mAmbientColor;
    float mPower, mAmbientPower;
    GLfloat mPolygonOffsetFactor;

private:
    bool mAtCameraPosition = true;
};

} // namespace light

