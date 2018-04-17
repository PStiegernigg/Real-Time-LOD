#include "geometry/SceneLoader.h"

#include <glm/gtc/matrix_transform.hpp>

#include "config.h"
#include "geometry/MeshLoader.h"
#include "cameras/PerspectiveCamera.h"

SceneLoader::SceneLoader() {
}

SceneLoader::~SceneLoader() {
}

std::unique_ptr<Scene> SceneLoader::sceneFromObj(std::string objFilename, float scale) {
    std::unique_ptr<Scene> scene{ new Scene() };

    MeshLoader meshLoader;
    meshLoader.loadFromFile(objFilename);

    glm::mat4 modelMatrix(1.f);
    if (scale != 0.f && scale != 1.f) {
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale, scale, scale));
    }

    std::unique_ptr<Instance> i;
    while ((i = meshLoader.getNextInstance()) != nullptr) {
        i->setModelMatrix(modelMatrix);
        scene->addInstance(std::move(i));
    }

    using camera::PerspectiveCamera;

    std::shared_ptr<PerspectiveCamera> camera{ new PerspectiveCamera{       
        Config::CENTER_OF_PROJECTION,  
        Config::FIELD_OF_VIEW, 
        static_cast<float>(Config::SCREEN_WIDTH) / static_cast<float>(Config::SCREEN_HEIGHT), 
        Config::NEAR_CLIP, 
        Config::FAR_CLIP} 
    };
    
    camera.get()->setOrientation(Config::ORIENTATION);
    scene->setCamera(camera);

    return scene;
}
