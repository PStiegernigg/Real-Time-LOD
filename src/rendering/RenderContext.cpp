#include "rendering/RenderContext.h"
#include <iostream>
#include <glm/gtc/matrix_transform.inl>
#include "util/Debugging.h"
#include "config.h"
#include "lights/PointLight.h"
#include "lights/SpotLight.h"
#include "lights/DirectionalLight.h"

using namespace camera;
using namespace light;
using namespace util;

RenderContext::RenderContext(unsigned int w, unsigned int h, const char* cwd)
: mWidth(w)
, mHeight(h)
, mPolygonCount{0}
, mLevel{0}
, mTimer {}

, mInputHandler()
, mScene {
    nullptr
}

, mScreenshot{"screenshots", w, h}
, mDirectTechnique{nullptr}
, mRenderMode{DIRECTRENDERING}
, mWireframe{false}

{
    Magick::InitializeMagick(cwd);
}

RenderContext::~RenderContext() {

}

void RenderContext::renderLoop() {
    while (!glfwWindowShouldClose(mWindow)) {
        update(mTimer.getFrameTime());
        render();
        glfwPollEvents();
        mTimer.measureTime();
    }
}

void RenderContext::update(double deltaTime) {
    double dx, dy;
    mInputHandler.getMouseDragDelta(dx, dy);
    rotateCamera(0.002f, dx, dy);

    auto camMoveVector = glm::vec3(0.f);
    if (mInputHandler.keyDown(KEY_W)) camMoveVector = glm::vec3(0, 0, -1);
    if (mInputHandler.keyDown(KEY_S)) camMoveVector = glm::vec3(0, 0, 1);
    if (mInputHandler.keyDown(KEY_A)) camMoveVector = glm::vec3(-1.f, 0, 0);
    if (mInputHandler.keyDown(KEY_D)) camMoveVector = glm::vec3(1.f, 0, 0);
    if (mInputHandler.keyDown(KEY_E)) camMoveVector = glm::vec3(0, -1.f, 0);
    if (mInputHandler.keyDown(KEY_Q)) camMoveVector = glm::vec3(0, 1.f, 0);

    float speedMod = mInputHandler.modifier(GLFW_MOD_SHIFT) ? 5.f : (mInputHandler.modifier(GLFW_MOD_CONTROL) ? 0.5f : 1.f);
    float speed = speedMod * 0.001f * float(deltaTime);
    moveCamera(camMoveVector * speed);
}

void RenderContext::render() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if(mWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    mPolygonCount = mDirectTechnique->renderPass(mScene, mRenderMode, mLevel);
    mScreenshot.takeIfScheduled();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    mGUI.draw();
    glfwSwapBuffers(mWindow);

}

void RenderContext::setScene(std::shared_ptr<Scene> scene) {
    mScene = scene;
    mDirectTechnique->setCamera(mScene->getCamera());
}

void RenderContext::moveCamera(glm::vec3 delta) {
    auto camera = mDirectTechnique->getCamera();
    camera->move(delta);  
}

void RenderContext::rotateCamera(float speed, float deltaX, float deltaY) {
    auto camera = mDirectTechnique->getCamera();
    camera->rotate(speed, deltaX, deltaY);
}

bool RenderContext::init() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Open a window and create its OpenGL context
    mWindow = glfwCreateWindow(mWidth, mHeight, "Real Time Level of Detail", nullptr, nullptr);
    if (mWindow == nullptr) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(mWindow);

    glewExperimental = true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return false;
    }

    mInputHandler.init(mWindow);
    glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, GL_TRUE);

    // Disable v-sync
    glfwSwapInterval(GL_FALSE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
   
    // Accept fragment if it closer to the camera than the previous one
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    // Correct Gamma handling
    glEnable(GL_FRAMEBUFFER_SRGB);
    
    initUI();
    initShadingTechnique();

    return true;
}

void RenderContext::initUI() {
    mGUI.init(mWidth, mHeight);
    auto mainBar = mGUI.createWindow();
    mainBar->setTitle("General");
    mainBar->setPosition(20, 20); // auto positioning NYI

    // IMPORTANT: when passing callbacks using std::bind (for member functions), always pass the pointer as argument
    // as the object will be copied otherwise!
    mainBar->addField<unsigned>(GUIBarOptions("FPS").group("Performance"), std::bind(&Timer::getFPS, &mTimer));
    mainBar->addField<double>(GUIBarOptions("Duration (ms)").group("Performance"), std::bind(&Timer::getFrameTime, &mTimer));
    mainBar->addField<unsigned>(GUIBarOptions("Polygons").group("Performance"), std::bind([this](){return mPolygonCount;}));
    
    mainBar->addButton(GUIBarOptions("Toggle Wireframe").group("Rendermode"), [this]() {mWireframe = !mWireframe;});
    
    std::function<void(unsigned int)> levelSetter = [this](unsigned int level){setLevel(level);};
    
    mainBar->addField<unsigned>(GUIBarOptions("LOD-Level").group("LOD-Control"), std::bind([this](){return mLevel;}), levelSetter);
    mainBar->addButton(GUIBarOptions("Reset Level"), [this]() {mLevel = 0;});
    
    mainBar->addButton(GUIBarOptions("Off").group("LOD-Mode"), [this]() {mRenderMode = DIRECTRENDERING;});
    mainBar->addButton(GUIBarOptions("Discrete LOD").group("LOD-Mode"), [this]() {mRenderMode = DISCRETELEVELMODE;});
    mainBar->addButton(GUIBarOptions("Continuous LOD").group("LOD-Mode"), [this]() {mRenderMode = CONTINUOUSLEVELMODE;});
    mainBar->addButton(GUIBarOptions("View-Dependent LOD").group("LOD-Mode"), [this]() {mRenderMode = VIEWDEPENDENTMODDE;});
    mainBar->addButton(GUIBarOptions("Take Screenshot"), [this]() {mScreenshot.schedule();});

    // REGISTER KEYBINDINGS
    mInputHandler.registerKeyBinding(KEY_P, [this](bool, bool, bool, bool) {
        mScreenshot.schedule(); });
    mInputHandler.registerKeyBinding(KEY_TAB, [this](bool released, bool, bool, bool) {
        if (!released) mGUI.toggleHidden();
    });
    mInputHandler.registerKeyBinding(KEY_ESCAPE, [this](bool, bool, bool, bool) {
        glfwSetWindowShouldClose(mWindow, true); });
}

bool RenderContext::initShadingTechnique() {
    mDirectTechnique = std::shared_ptr<DirectRenderTechnique>{new DirectRenderTechnique("shaders/phongShading.shdr")};

    if (!mDirectTechnique->init()) {
        std::cerr << "Error: Direct shading technique not initialized!" << std::endl;
        return false;
    }
    
    return true;
}

void RenderContext::cleanUp() {
    mScene->cleanUp();
    glfwTerminate();
}

void RenderContext::setLevel(unsigned level) {
    mLevel = level;
} 
