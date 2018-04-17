/**
 * Wrapper class around GLFW providing a key binding callback interface.
 *
 * @author Philip Salzmann
 */

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <map>
#include <glm/glm.hpp>

#define KEY_A GLFW_KEY_A
#define KEY_B GLFW_KEY_B
#define KEY_C GLFW_KEY_C
#define KEY_D GLFW_KEY_D
#define KEY_E GLFW_KEY_E
#define KEY_F GLFW_KEY_F
#define KEY_G GLFW_KEY_G
#define KEY_H GLFW_KEY_H
#define KEY_I GLFW_KEY_I
#define KEY_J GLFW_KEY_J
#define KEY_K GLFW_KEY_K
#define KEY_L GLFW_KEY_L
#define KEY_M GLFW_KEY_M
#define KEY_N GLFW_KEY_N
#define KEY_O GLFW_KEY_O
#define KEY_P GLFW_KEY_P
#define KEY_Q GLFW_KEY_Q
#define KEY_R GLFW_KEY_R
#define KEY_S GLFW_KEY_S
#define KEY_T GLFW_KEY_T
#define KEY_U GLFW_KEY_U
#define KEY_V GLFW_KEY_V
#define KEY_W GLFW_KEY_W
#define KEY_X GLFW_KEY_X
#define KEY_Y GLFW_KEY_Y
#define KEY_Z GLFW_KEY_Z
#define KEY_TAB GLFW_KEY_TAB
#define KEY_ENTER GLFW_KEY_ENTER
#define KEY_ESCAPE GLFW_KEY_ESCAPE
#define KEY_SPACE GLFW_KEY_SPACE

typedef std::function<void(bool released, bool ctrl, bool shift, bool alt)> KeyBinding;

class InputHandler {
public:
    InputHandler();
    void init(GLFWwindow* window);
    void registerKeyBinding(int key, KeyBinding binding);
    // get difference in mouse position with mouse1 being held down
    void getMouseDragDelta(double& dx, double& dy);
    bool keyDown(int key) const;
    bool modifier(int mod) const;
private:
    GLFWwindow* mWindow;
    double mouseXPrev, mouseYPrev;
    bool mMouse1Down; // for dragging
    void keyCallback(int key, int action, int mod);
    void mouseMoveCallback(double xPos, double yPos);
    void mouseButtonCallback(int button, int action);

    std::map<int, bool> mPressedKeys;
    int mModifiers;
    std::map<int, KeyBinding> mKeyBindings;
};

