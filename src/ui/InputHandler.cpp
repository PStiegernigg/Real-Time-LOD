#include "ui/InputHandler.h"
#include <AntTweakBar.h>

InputHandler::InputHandler()
    : mModifiers(0)
{ }

void InputHandler::init(GLFWwindow* window) {
    mWindow = window;

    glfwSetWindowUserPointer(mWindow, this);
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mod) {
        static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->keyCallback(key, action, mod);
    });

    glfwSetCharCallback(mWindow, [](GLFWwindow* window, unsigned codePoint) {
        TwEventCharGLFW(codePoint, GLFW_PRESS);
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mod) {
        static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->mouseButtonCallback(button, action);
    });

    glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xPos, double yPos) {
        static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->mouseMoveCallback(xPos, yPos);
    });
}

void InputHandler::registerKeyBinding(int key, KeyBinding binding) {
    mKeyBindings[key] = binding;
}

void InputHandler::getMouseDragDelta(double& dx, double& dy) {
    if (!mMouse1Down || glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_1) != GLFW_PRESS) {
        dx = dy = 0.0;
        return;
    }

    double x, y;
    glfwGetCursorPos(mWindow, &x, &y);
    dx = x - mouseXPrev;
    dy = y - mouseYPrev;
    mouseXPrev = x;
    mouseYPrev = y;
}

bool InputHandler::keyDown(int key) const {
    auto it = mPressedKeys.find(key);
    if (it != mPressedKeys.end()) {
        return it->second;
    }

    return false;
}

bool InputHandler::modifier(int mod) const {
    return mModifiers & mod;
}

void InputHandler::keyCallback(int key, int action, int mod) {
    auto handled = TwEventKeyGLFW(key, action);
    mModifiers = mod;

    if (!handled) {
        mPressedKeys[key] = action == GLFW_PRESS || action == GLFW_REPEAT;
        if (mKeyBindings.find(key) != mKeyBindings.end()) {
            mKeyBindings[key](action == GLFW_RELEASE, mod & GLFW_MOD_CONTROL, mod & GLFW_MOD_SHIFT, mod & GLFW_MOD_ALT);
        }
    }
}

void InputHandler::mouseButtonCallback(int button, int action) {
    auto handled = TwEventMouseButtonGLFW(button, action);
    if (!handled && button == GLFW_MOUSE_BUTTON_1) {
        mMouse1Down = action == GLFW_PRESS;
        glfwGetCursorPos(mWindow, &mouseXPrev, &mouseYPrev);
    }
}

void InputHandler::mouseMoveCallback(double xPos, double yPos) {
    TwEventMousePosGLFW(static_cast<int>(xPos), static_cast<int>(yPos));
}
