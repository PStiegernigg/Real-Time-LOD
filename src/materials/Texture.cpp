/* 
 * File:   Texture.cpp
 * Author: tport
 * 
 * Created on March 12, 2014, 5:01 PM
 */

#include "materials/Texture.h"
#include <iostream>

Texture::Texture() {
}

Texture::Texture(const std::string& fileName, GLenum textureTarget, GLint internalFormat)
: fileName(fileName),
textureTarget{textureTarget},
internalFormat{internalFormat},
image{nullptr}
{
}

Texture::Texture(const Texture& orig) {
}

Texture::~Texture() {
}

bool Texture::loadTexture() {
    try {
        image = Magick::Image(fileName);
        image.flip();
        image.write(&blob, "RGBA");
    } catch (Magick::Error& error) {
        std::cout << "Error loading texture: " << fileName << ": " <<
                error.what() << std::endl;
        return false;
    }
    glGenTextures(1, &textureObject);
    glBindTexture(textureTarget, textureObject);
    glTexImage2D(
            textureTarget, // target
            0, // Level of Detail (0 = best resolution)
            internalFormat, // target format
            (GLsizei) image.columns(),
            (GLsizei) image.rows(),
            0, // border
            GL_RGBA, // source format
            GL_UNSIGNED_BYTE, // source type
            blob.data() // data memory address (start)
            );
    glGenerateMipmap(textureTarget);

    //glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // test, further checks required
    float aniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

    return true;
}

void Texture::bindTexture(GLenum textureUnit) const {
    glActiveTexture(textureUnit);
    glBindTexture(textureTarget, textureObject);
}
