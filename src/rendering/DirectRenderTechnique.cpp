#include <iostream>
#include "rendering/DirectRenderTechnique.h"
#include "config.h"

DirectRenderTechnique::DirectRenderTechnique(const std::string& shaderFile)
: mDiscreteLevelShader {
    new Shader(shaderFile)
}
, mContinuousLevelShader{
    new Shader("shaders/continuousLODRender.shdr")
}

, mViewDependantShader {
    new Shader("shaders/viewDependentLODRender.shdr")
}
, mCamera{nullptr}
{
}

DirectRenderTechnique::~DirectRenderTechnique() {
}

unsigned long DirectRenderTechnique::renderPass(const std::shared_ptr<Scene> scene, RenderMode mode, unsigned setLevel) {

    unsigned long polygonCount = 0;

    switch (mode) {
        case DIRECTRENDERING:

            mDiscreteLevelShader->useProgram();

            for (const std::unique_ptr<Instance>& i : scene->getInstances()) {
                mDiscreteLevelShader->setUniform("MVP", mCamera->getViewProjection() * i->getModelMatrix());
                mDiscreteLevelShader->setUniform("vecToCamera", i->getVecToCamera());
                polygonCount += i->getMesh()->draw();
            }

            break;
        
        case DISCRETELEVELMODE:

            mDiscreteLevelShader->useProgram();

            for (const std::unique_ptr<Instance>& i : scene->getInstances()) {
    
                mDiscreteLevelShader->setUniform("MVP", mCamera->getViewProjection() * i->getModelMatrix());
                mDiscreteLevelShader->setUniform("vecToCamera", i->getVecToCamera());
                
                if(setLevel != 0) {
                    polygonCount += i->getMesh()->drawDetailLevel(setLevel);
                }
                else {
                    GLuint minPolygons = i->getMesh()->getPolygonCount() - 2 * i->getCurrentLevel();
                    polygonCount += i->getMesh()->drawThresholdLevel(minPolygons); 
                }
            }

            break;
        
        case CONTINUOUSLEVELMODE:

            mContinuousLevelShader->useProgram();

            for (const std::unique_ptr<Instance>& i : scene->getInstances()) {

                GLuint level = i->getCurrentLevel();
                           
                mContinuousLevelShader->setUniform("MVP", mCamera->getViewProjection() * i->getModelMatrix());
                mContinuousLevelShader->setUniform("vecToCamera", i->getVecToCamera());
                mContinuousLevelShader->setUniform("level", level);

                polygonCount += i->getMesh()->drawContinuousLevel(level);
            }

            break;
            
        case VIEWDEPENDENTMODDE:

            mViewDependantShader->useProgram();

            for (const std::unique_ptr<Instance>& i : scene->getInstances()) {
                
               GLuint level = i->getCurrentLevel();
               
                mViewDependantShader->setUniform("MVP", mCamera->getViewProjection() * i->getModelMatrix());
                mViewDependantShader->setUniform("vecToCamera", i->getVecToCamera());
                mViewDependantShader->setUniform("level", level);
                
                polygonCount += i->getMesh()->drawViewDependentLevel(level);
            }

            break;
    }

    return polygonCount;
}

bool DirectRenderTechnique::init() {

    if (!mDiscreteLevelShader->init()) {
        std::cout << "Error: Discrete LOD shader not ready!" << std::endl;
        return false;
    }

    if (!mContinuousLevelShader->init()) {
        std::cout << "Error: Continuous LOD shader not ready!" << std::endl;
        return false;
    }
    
    if (!mViewDependantShader->init()) {
        std::cout << "Error: View-Dependant LOD shader not ready!" << std::endl;
        return false;
    }
    
    return true;
}

void DirectRenderTechnique::setCamera(std::shared_ptr<Camera> camera) {
    mCamera = camera;
}

std::shared_ptr<Camera> DirectRenderTechnique::getCamera() {
    return mCamera;
}

