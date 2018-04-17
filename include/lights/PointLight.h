/* 
 * File:   PointLight.h
 * Author: tport
 *
 * Created on April 9, 2014, 9:58 PM
 */

#pragma once

#include "lights/Light.h"

namespace light {

class PointLight : public Light {
public:
    PointLight();
    ~PointLight();
    LightType getType() const override;
};

} // namespace light
