/* 
 * File:   PointLight.cpp
 * Author: tport
 * 
 * Created on April 9, 2014, 9:58 PM
 */

#include "lights/PointLight.h"

light::PointLight::PointLight() { }

light::PointLight::~PointLight() { }

light::LightType light::PointLight::getType() const {
    return LightType::POINTLIGHT;
}
