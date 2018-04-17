#include "ui/GUI.h"

GUI::GUI()
    : mHidden(false)
{
}

GUI::~GUI() {
    TwTerminate();
}

void GUI::init(int width, int height) const {
    TwInit(TW_OPENGL_CORE, nullptr);
    TwCopyStdStringToClientFunc([](std::string& dest, const std::string& src){
        dest = src;
    });
    TwWindowSize(width, height);
    TwDefine("TW_HELP visible=false");
}

void GUI::draw() const {
    if (!mHidden) {
        TwDraw();
    }
}

void GUI::toggleHidden() {
    mHidden = !mHidden;
}

std::shared_ptr<GUIBar> GUI::createWindow() {
    auto window = std::make_shared<GUIBar>();
    window->init();
    mGUIWindows.push_back(window);
    return window;
}
