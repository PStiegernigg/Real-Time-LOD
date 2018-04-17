/* 
 * A Scene is made up of an collection of instances and a camera.
 *
 * @author Philip Salzmann
 * Based on original implementation by tport (SceneManager).
 */

#pragma once

#include <memory>
#include <thread>

#include "cameras/Camera.h"
using camera::Camera;

#include "geometry/Instance.h"
#include "geometry/BoundingBox.h"

class Scene {
public:
    Scene();
    virtual ~Scene();
    std::shared_ptr<Camera> getCamera() const;
    void addInstance(std::unique_ptr<Instance> instance);
    void setCamera(std::shared_ptr<Camera> camera);
    bool inCameraFrustum(const BoundingSphere& bSphere) const;
    const std::vector<std::unique_ptr<Instance>>& getInstances() const;
    void cleanUp();
    const BoundingBox& getBoundingBox() const;
    void startUpdateRoutine();
    void stopUpdateRoutine();
    
private:
    std::vector<std::unique_ptr<Instance>> mInstances;
    std::shared_ptr<Camera> mCamera;
    BoundingBox mBoundingBox;
    std::unique_ptr<std::thread> mUpdateThread;
    bool mUpdating;
    
    void update();
};

