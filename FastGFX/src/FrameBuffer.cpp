//
// Created by Michał Łaszczewski on 28/03/17.
//

#include <cassert>
#include "FrameBuffer.h"

namespace fgfx {

#ifdef __VIDEO_RENDERING

  void FrameBuffer::saveFBO() {
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old_fbo);
    glGetIntegerv(GL_VIEWPORT, old_viewport);
  }
  void FrameBuffer::loadFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, old_fbo);
    glViewport(old_viewport[0], old_viewport[1], old_viewport[2], old_viewport[3]);
  }

  FrameBuffer::FrameBuffer(int widthp, int heightp, bool hasZp) : width(widthp), height(heightp), hasZ(hasZp) {
    saveFBO();

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glGenRenderbuffers(1, &colorRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRbo);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA16, width, height);

    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    if(hasZ) {
      glGenRenderbuffers(1, &depthRbo);
      glBindRenderbuffer(GL_RENDERBUFFER, depthRbo);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
      glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRbo);
    }

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER));

    loadFBO();
  }

  void FrameBuffer::readImage(std::shared_ptr<Image> image) {
    assert(image->width == width);
    assert(image->height == height);
    glFlush();
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
  }

  void FrameBuffer::readRGB8(void* rgb8) {
    glFlush();
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, rgb8);

    checkGLError();
  }

  void FrameBuffer::readRGB16(void* rgb16) {
    glFlush();
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_SHORT, rgb16);

    checkGLError();
  }

  void FrameBuffer::beginRender() {
    saveFBO();
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glViewport(0, 0, width, height);
  }

  void FrameBuffer::endRender() {
    loadFBO();
  }

#endif

}