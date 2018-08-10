//
// Created by Michał Łaszczewski on 14/04/16.
//

#ifndef FGFX_TEXTUREDBUFFER_H
#define FGFX_TEXTUREDBUFFER_H

#include "Texture.h"

#include <vector>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#include "config.h"

namespace fgfx {

  class TexturedBuffer {
  protected:
    std::vector<glm::vec3> texturedVertices;
    std::vector<glm::vec4> texturedColors;
    std::vector<glm::vec2> texturedCoords;

  public:
    int texturedPointsCount;

    GLuint positionBuffer;
    GLuint colorBuffer;
    GLuint coordBuffer;
  private:

    friend class TexturedLayer;
  public:
    int textureId;

    TexturedBuffer(int textureIdp);
    ~TexturedBuffer();

    void reserve(int pointCount);
    void bufferPoint(glm::vec3 pos, glm::vec4 color, glm::vec2 coord);
    void bufferPoint(glm::vec2 pos, glm::vec4 color, glm::vec2 coord);
    void reset();
    void upload();
    void render();
  };

}


#endif //FGFX_TEXTUREDBUFFER_H
