//
// Created by Michał Łaszczewski on 28/03/17.
//

#ifndef FGFX_FRAMEBUFFER_H
#define FGFX_FRAMEBUFFER_H

#include "config.h"
#include <memory>
#include "Image.h"

namespace fgfx {

  class FrameBuffer {
  public:
    int width;
    int height;
    bool hasZ;

    GLuint fbo;
    GLuint colorRbo;
    GLuint depthRbo;

    FrameBuffer(int widthp, int heightp, bool hasZp);

    void beginRender();
    void endRender();
    void readImage(std::shared_ptr<Image> image);
    void readRGB8(void* rgb8);
    void readRGB16(void* rgb16);
  private:
    GLint old_fbo;
    GLint old_viewport[4];
    void saveFBO();
    void loadFBO();
  };

}



#endif //FGFX_FRAMEBUFFER_H


