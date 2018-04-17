/**
 * Utility class for taking screen captures.
 *
 * @author Philip Salzmann
 * Based on original implementation by tport.
 */

#pragma once

#include <GL/glew.h>
#include <string>

namespace util {

class Screenshot {
public:
    Screenshot(const std::string& directory, unsigned int w, unsigned int h);
    virtual ~Screenshot();

    void takeIfScheduled();
    void schedule();
private:
    std::string     mDirectory;
    GLubyte*        mBuffer;
    unsigned int    mWidth, mHeight;
    bool            mIsScheduled;
};

} // namespace util
