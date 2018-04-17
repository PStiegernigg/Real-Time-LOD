/**
 * The main control unit. Initializes everything, handles all render passes.
 *
 * @author tport
 * @author Philip Salzmann
 * @author Philipp Stiegernigg
 */

#pragma once

#include <GL/glew.h>
#include <map>

#include "geometry/Scene.h"
#include "ui/InputHandler.h"
#include "ui/GUI.h"
#include "util/Timer.h"
using util::Timer;

#include "rendering/RenderBuffer.h"
#include "util/Screenshot.h"
#include "rendering/DirectRenderTechnique.h"
#include "util/Enumerators.h"
using light::LightType;

class RenderContext {
public:
    RenderContext(unsigned int w, unsigned int h, const char* cwd);
    ~RenderContext();
    bool init();
    void renderLoop();
    void setScene(std::shared_ptr<Scene> scene);
    void setLevel(unsigned level);
    void cleanUp();
    
private:
    unsigned int            mWidth;
    unsigned int            mHeight;
    unsigned long           mPolygonCount;
    unsigned int            mLevel;
    Timer                   mTimer;
    InputHandler            mInputHandler;
    GLFWwindow*             mWindow;

    std::shared_ptr<Scene>  mScene;
    GUI                     mGUI;
    util::Screenshot        mScreenshot;
    std::shared_ptr<DirectRenderTechnique> mDirectTechnique;
    RenderMode              mRenderMode;
    bool                    mWireframe;
    
    // update scene logic
    void update(double deltaTime);
    void moveCamera(glm::vec3 delta);
    void rotateCamera(float speed, float deltaX, float deltaY);
    void moveLightToCamera();

    // render scene
    void render();

    /**
     * Inits GUI and keybinds.
     */
    void initUI();
    bool initShadingTechnique();
};
