//
// Created by Michał Łaszczewski on 14/04/16.
//

#ifndef FGFX_TextureTransitionBUFFER_H
#define FGFX_TextureTransitionBUFFER_H

#include "Texture.h"

#include <vector>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#include "config.h"

namespace fgfx {

  class TextureTransitionBuffer {
  protected:
    std::vector<glm::vec3> textureTransitionVertices;
    std::vector<glm::vec4> textureTransitionColors;
    std::vector<glm::vec3> textureTransitionCoords1;
    std::vector<glm::vec3> textureTransitionCoords2;
    int textureTransitionPointsCount;

    GLuint positionBuffer;
    GLuint colorBuffer;
    GLuint coord1Buffer;
    GLuint coord2Buffer;

    friend class TextureTransitionLayer;
  public:
    int textureId;

    TextureTransitionBuffer(int textureIdp);
    ~TextureTransitionBuffer();

    void reserve(int pointCount);
    void bufferPoint(glm::vec3 pos, glm::vec4 color, glm::vec3 coord1, glm::vec3 coord2);
    void bufferPoint(glm::vec2 pos, glm::vec4 color, glm::vec3 coord1, glm::vec3 coord2);
    void reset();
    void upload();
    void render();
  };

}


#endif //FGFX_TextureTransitionBUFFER_H
