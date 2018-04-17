/**
 * VirtaMed Playground
 *
 * Originally created by tport.
 */

#include <iostream>

#include "rendering/RenderContext.h"
#include "geometry/SceneLoader.h"
#include "cameras/PerspectiveCamera.h"
#include "config.h"
#include "geometry/LODGenerator.h"
#include "util/Timer.h"

int main(int argc, char** argv) {
    

    // initialize OpenGL context
    RenderContext renderContext(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, *argv);
    if (!renderContext.init()) {
        return EXIT_FAILURE;
    }

    SceneLoader sl;
    auto scene = sl.sceneFromObj("scenes/bunny.obj");
    //auto scene = sl.sceneFromObj("scenes/head/head.obj");
    //auto scene = sl.sceneFromObj("scenes/sphere.obj");
    //auto scene = sl.sceneFromObj("scenes/dragon.obj");
    //auto scene = sl.sceneFromObj("scenes/dragon2.obj");
    //auto scene = sl.sceneFromObj("scenes/armadillo/armadillo.obj");
    
    scene->startUpdateRoutine();
    
    LODGenerator generator;
    generator.createDetailLevels(scene->getInstances()[0]->getMesh(), 3100000000, 3250000000, 10);
    
    renderContext.setScene(std::move(scene));
    renderContext.renderLoop();
    renderContext.cleanUp();
    
    return EXIT_SUCCESS;
}
