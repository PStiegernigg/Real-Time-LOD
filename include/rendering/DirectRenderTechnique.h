/* 
 * File:   DirectShadingTechnique.h
 * Author: Philipp Stiegernigg
 *
 * Based on original work by tport
 * Created on May 25, 2015, 01:27 PM
 */

#pragma once

#include "geometry/Scene.h"

enum RenderMode {
    DIRECTRENDERING,
    DISCRETELEVELMODE,
    CONTINUOUSLEVELMODE,
    VIEWDEPENDENTMODDE
};

class DirectRenderTechnique {
public:
    DirectRenderTechnique(const std::string& shaderFile);
    virtual ~DirectRenderTechnique();

    bool init();
    virtual unsigned long renderPass(const std::shared_ptr<Scene> scene, RenderMode mode, unsigned setLevel);
    void setCamera(std::shared_ptr<Camera> camera);
    std::shared_ptr<Camera> getCamera();
    void setShader(std::shared_ptr<Shader> shader);
   
protected:
    std::shared_ptr<Shader> mDiscreteLevelShader;
    std::shared_ptr<Shader> mContinuousLevelShader;
    std::shared_ptr<Shader> mViewDependantShader;
    std::shared_ptr<Camera> mCamera;
};
