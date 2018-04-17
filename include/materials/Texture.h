/* 
 * File:   Texture.h
 * Author: tport
 *
 * Created on March 12, 2014, 5:01 PM
 */

#ifndef TEXTURE_H
#define	TEXTURE_H

#include <GL/glew.h>
#include <Magick++.h>


class Texture {
public:
    Texture();
    Texture(const std::string& fileName, GLenum textureTarget, GLint internalFormat);
    Texture(const Texture& orig);
    bool loadTexture();
    void bindTexture(GLenum textureUnit) const;
    virtual ~Texture();
private:
    std::string fileName;
    GLenum textureTarget;
    GLuint textureObject;
    GLint internalFormat;
    Magick::Image image;
    Magick::Blob blob;
};

#endif	/* TEXTURE_H */

