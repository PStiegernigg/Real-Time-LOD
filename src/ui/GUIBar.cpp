#include "ui/GUIBar.h"
#include "util/Util.h"
#include <memory>

GUIBar::GUIBar()
    : mTwBar {nullptr}
{
}

GUIBar::~GUIBar() {
    if (mTwBar != nullptr) {
        // FIXME: reports not initialized for some reason
        //TwDeleteBar(mTwBar); 
    }
}

void GUIBar::init() {
    mTwBarName = getUniqueName();
    mTwBar = TwNewBar(mTwBarName.c_str());
    setTitle(mTwBarName);

    define("color='0 0 0' alpha=192");
    define("valueswidth=130");
    define("iconifiable=false");
    define("movable=false");
    define("resizable=false");
    define("contained=true");
    define("fontresizable=false");

    resize();
}

void GUIBar::setTitle(std::string title) {
    define("label='" + title + "'");
}

void GUIBar::addButton(GUIBarOptions options, Callback callback) {
    auto cbWrapper = [](void* clientData) {
        (*static_cast<Callback*>(clientData))();
    };

    auto callbackCopy = std::make_unique<Callback>(callback);
    TwAddButton(mTwBar, options.getLabel().c_str(), cbWrapper, callbackCopy.get(), options.asString().c_str());
    mCallbacks.push_back(std::move(callbackCopy));

    storeControl(options.getLabel(), options.getGroup());
}

void GUIBar::addVarCBRaw(GUIBarOptions options, TwType type, TwSetVarCallback setter, TwGetVarCallback getter, void* clientData) {
    TwAddVarCB(mTwBar, options.getLabel().c_str(), type, setter, getter, clientData, options.asString().c_str());
}

void GUIBar::closeGroup(std::string name) {
    // NYI
}

void GUIBar::setSize(unsigned w, unsigned h) const {
    std::stringstream ss;
    ss << "size='" << w << " " << h << " '";
    define(ss.str());
}

void GUIBar::setPosition(unsigned x, unsigned y) const {
    std::stringstream ss;
    ss << "position='" << x << " " << y << " '";
    define(ss.str());
}

void GUIBar::reset() {
    for (auto c : mControls) {
        TwRemoveVar(mTwBar, c.first.c_str());
    }

    mControls.clear();
    mGroups.clear();
    mCallbacks.clear();
    mGSCallbacks.clear();
}

std::string GUIBar::getUniqueName() {
    static auto counter = 0;
    counter++;
    std::stringstream ss;
    ss << "guibar-" << counter;
    return ss.str();
}

void GUIBar::define(std::string option) const {
    TwDefine((mTwBarName + " " + option).c_str());
}

void GUIBar::storeControl(std::string label, std::string group) {
    mControls[label] = group;
    mGroups[group] = true && mGroups[group]; // default to true == open
    resize();
}

void GUIBar::resize() {
    std::map<std::string, bool> groups;
    int elementCount = 0;
    for (auto c : mControls) {
        auto g = c.first;
        if (groups.find(g) == groups.end()) {
            groups[g] = mGroups[g];
            elementCount++;
        }

        if (groups[g]) {
            // group is open, control is visible
            elementCount++;
        }
    }

    // FIXME: hardcoded pixel values
    setSize(300, elementCount * 20 + 40);
}
