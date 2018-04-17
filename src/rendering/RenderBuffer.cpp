/* 
 * File:   RenderBuffer.cpp
 * Author: tport
 * 
 * Created on April 9, 2014, 1:16 PM
 */

#include <iostream>
#include "rendering/RenderBuffer.h"

RenderBuffer::RenderBuffer(unsigned int w, unsigned int h) :
width {
    w
},
height{h},
colorAttachments{},
drawBuffers{},
depthTexture{0},
doMipmap{false}
{
}

RenderBuffer::~RenderBuffer() {
    if (depthTexture) {
        glDeleteTextures(1, &depthTexture);
    }

    for (GLuint tex : colorAttachments) {
        if (tex)
            glDeleteTextures(1, &tex);
    }

    if (frameBuffer) {
        glDeleteFramebuffers(1, &frameBuffer);
    }

    if (mDepthBuffer) {
        glDeleteRenderbuffers(1, &mDepthBuffer);
    }
}

void RenderBuffer::init() {
    // Create render target
    glGenFramebuffers(1, &frameBuffer);
}

void RenderBuffer::addDepthBuffer() {
    glGenRenderbuffers(1, &mDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER,
            mDepthBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderBuffer::addColorAttachment(unsigned int index, GLint internalFormat, GLint filter, bool mipmap) {
    doMipmap = mipmap;
    GLuint colorAttachment = 0;
    glGenTextures(1, &colorAttachment);
    colorAttachments.insert(colorAttachments.begin() + index, colorAttachment);
    glBindTexture(GL_TEXTURE_2D, colorAttachments[index]);

    if (mipmap) {
        // calculate # mipmap levels for full stack down to 1x1 pixel level
        unsigned int level = 1;
        while ((width | height) >> level) level += 1;
        glTexStorage2D(GL_TEXTURE_2D, level, internalFormat, width, height);
    } else
        glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    if (mipmap)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (mipmap)
        glGenerateMipmap(GL_TEXTURE_2D);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0 + index,
            colorAttachments[index],
            0
            );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Update drawBuffers
    drawBuffers.insert(drawBuffers.begin() + index, GL_COLOR_ATTACHMENT0 + index);
}

void RenderBuffer::addDepthTexture(GLint internalFormat, GLint filter, GLint compareMode) {
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // the following must be commented out if the shadowMap is displayed instead of actually sampled
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, compareMode); // note: compareMode can either be GL_COMPARE_REF_TO_TEXTURE or GL_NONE (in OpenGL 4)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool RenderBuffer::check() {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    if (colorAttachments.size() && drawBuffers.size() > 0)
        glDrawBuffers((GLsizei) drawBuffers.size(), drawBuffers.data());
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error: RenderBuffer not complete!" << std::endl;
        return false;
    }
    if (!colorAttachments.size() && depthTexture) {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_DEPTH_ATTACHMENT);
    } else if (colorAttachments.size() > 0)
        glReadBuffer(GL_COLOR_ATTACHMENT1);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

GLuint RenderBuffer::getBufferID() const {
    return frameBuffer;
}

void RenderBuffer::bindForWriting(bool clear) const {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, width, height);
    if (!colorAttachments.empty()) {
        glEnable(GL_DEPTH_TEST);
        if (clear) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else if (mDepthBuffer || depthTexture) {
        glEnable(GL_DEPTH_TEST);
        if (clear) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        glDisable(GL_DEPTH_TEST);
        if (clear) glClear(GL_COLOR_BUFFER_BIT);
    }
}

void RenderBuffer::bindForReading() const {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer);
}

void RenderBuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderBuffer::bindColorAttachmentForReading(unsigned int index, GLenum textureUnit) const {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, colorAttachments[index]);
    if (doMipmap)
        glGenerateMipmap(GL_TEXTURE_2D);
}

void RenderBuffer::bindForCopyRead(unsigned int index) const {
    bindForReading();
    glReadBuffer(GL_COLOR_ATTACHMENT0);
}

void RenderBuffer::bindForCopyWrite() {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RenderBuffer::bindDepthtextureForReading(GLenum textureUnit) const {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
}

GLuint RenderBuffer::getDepthTexture() const {
    return depthTexture;
}
