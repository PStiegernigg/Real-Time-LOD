/* 
 * File:   SceneManager.cpp
 * Author: tport
 * 
 * Created on March 11, 2014, 9:59 PM
 */

#include <thread>

#include "geometry/Scene.h"
#include "util/Util.h"
#include "config.h"

Scene::Scene()
    : mInstances(), mUpdating{false}
{
}

Scene::~Scene() {
    stopUpdateRoutine();
}

std::shared_ptr<Camera> Scene::getCamera() const {
    return mCamera;
}

void Scene::addInstance(std::unique_ptr<Instance> instance) {
    mBoundingBox.include(instance->getBoundingBox());
    mInstances.push_back(std::move(instance));
}

void Scene::setCamera(std::shared_ptr<Camera> camera) {
    this->mCamera = camera;
}

bool Scene::inCameraFrustum(const BoundingSphere& bSphere) const {
    return mCamera->inFrustum(bSphere);
}

const std::vector<std::unique_ptr<Instance>>&Scene::getInstances() const {
    return mInstances;
}

void Scene::cleanUp() {
    for (std::unique_ptr<Instance>& i : mInstances) {
        i->cleanUp();
    }
}

void Scene::update() {
      while(mUpdating) {
        for(const std::unique_ptr<Instance>& i : mInstances) {
            i->updateCameraAttributes(mCamera->getViewProjection());
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(Config::UPDATERATE));
    }
}

const BoundingBox& Scene::getBoundingBox() const {
    return mBoundingBox;
}

void Scene::startUpdateRoutine() {
    
    if(!mUpdating) {
        mUpdating = true;
        mUpdateThread.reset(new std::thread([this] () {update();}));
    }
}

void Scene::stopUpdateRoutine() {
    
    if(mUpdating) {
        mUpdating = false;
        mUpdateThread->join();
    }
}