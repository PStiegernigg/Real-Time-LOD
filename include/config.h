#define GLM_FORCE_RADIANS


#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"

#pragma once

namespace Config
{
    // screen window
    const uint SCREEN_WIDTH = 1280;
    const uint SCREEN_HEIGHT = 720;
    const uint SCREEN_RESOLUTION = SCREEN_HEIGHT * SCREEN_WIDTH;
    
    // orientation
    const glm::quat ORIENTATION { glm::radians(glm::vec3{-10.0f, 45.0f, 0.0f}) };
    const glm::vec3 CENTER_OF_PROJECTION { 8.0f, 4.0f, 8.0f };
    const float FIELD_OF_VIEW = 45.0f;
    
    // the near and far clip value used for perspective projections
    const float NEAR_CLIP = 0.01f;
    const float FAR_CLIP = 100.f;
    
    // Level of detail parameters
    const uint PIXEL_ERROR_THRESHOLD = 2000;
    const float MAX_MODEL_ERROR = 0.99; //Percentage (range 0.0 - 1.0)
    const uint UPDATERATE = 20; //In milliseconds 
    
    #define WORKGROUPSIZE 256

}
