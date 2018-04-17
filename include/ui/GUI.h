/**
 * The GUI class acts as a manager for one or more GUIBar instances.
 * All *global* Tw calls are made from here.
 *
 * @author Philip Salzmann
 */

#pragma once

#include <AntTweakBar.h>
#include "GUIBar.h"
#include <vector>
#include <memory>

class GUI {
public:
    GUI();
    virtual ~GUI();

    void init(int width, int height) const;
    void draw() const;
    void toggleHidden();

    // creates a new managed window
    std::shared_ptr<GUIBar> createWindow();
private:
    std::vector<std::shared_ptr<GUIBar>> mGUIWindows;
    bool mHidden;
};
