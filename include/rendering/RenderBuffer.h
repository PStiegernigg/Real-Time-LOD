/* 
 * File:   RenderBuffer.h
 * Author: tport
 *
 * Created on April 9, 2014, 1:16 PM
 */

#ifndef RENDERBUFFER_H
#define	RENDERBUFFER_H

#include <GL/glew.h>
#include <vector>

class RenderBuffer {
public:
    RenderBuffer(unsigned int w, unsigned int h);
    virtual ~RenderBuffer();
    void init();
    void addColorAttachment(unsigned int index, GLint internalFormat, GLint filter, bool mipmap);
    void addDepthBuffer();
    void addDepthTexture(GLint internalFormat, GLint filter, GLint compareMode);
    bool check();
    GLuint getBufferID() const;
    void bindColorAttachmentForReading(unsigned int index, GLenum textureUnit) const;
    void bindForCopyRead(unsigned int index) const;
    void bindForCopyWrite();
    void bindDepthtextureForReading(GLenum textureUnit) const;
    GLuint getDepthTexture() const;
    void bindForWriting(bool clear = true) const;
    void bindForReading() const;
    void unbind() const;
private:
    unsigned int width, height;
    GLuint frameBuffer = 0;
    GLuint mDepthBuffer = 0;
    std::vector<GLuint> colorAttachments;
    std::vector<GLenum> drawBuffers;
    GLuint depthTexture;
    bool doMipmap;
};

#endif	/* RENDERBUFFER_H */

