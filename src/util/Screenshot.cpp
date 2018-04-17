#include "util/Screenshot.h"
#include <Magick++/Image.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>

util::Screenshot::Screenshot(const std::string& directory, unsigned int w, unsigned int h)
    : mDirectory(directory)
    , mBuffer{new GLubyte[w * h * 3]}
    , mWidth{w}
    , mHeight{h}
    , mIsScheduled(false)
{
    mkdir(directory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

util::Screenshot::~Screenshot() {
    delete[] mBuffer;
}

void util::Screenshot::takeIfScheduled() {
    if (!mIsScheduled) return;

    glReadPixels(0, 0, mWidth, mHeight, GL_RGB, GL_UNSIGNED_BYTE, mBuffer);
    Magick::Image image(mWidth, mHeight, "RGB", Magick::StorageType::CharPixel, mBuffer);
    image.flip();
    
    std::stringstream s;
    s << mDirectory << "/" << "screenshot.tiff";

    try {
        image.write(s.str());
    } catch (Magick::Error& error) {
        std::cerr << "Error saving file to: " << s.str() << ": " << error.what() << std::endl;
    }

    mIsScheduled = false;
}

void util::Screenshot::schedule() {
    mIsScheduled = true;
}
