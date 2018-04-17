/**
 * The SceneLoader class loads OBJ files into a scene description.
 *
 * @author Philip Salzmann
 */

#pragma once

#include <memory>
#include <string>

#include "geometry/Scene.h"

class SceneLoader {
public:
    SceneLoader();
    virtual ~SceneLoader();

    std::unique_ptr<Scene> sceneFromObj(std::string objFilepath, float scale = 1.0f);
private:
};
