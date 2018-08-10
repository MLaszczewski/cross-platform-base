//
// Created by Michał Łaszczewski on 14/04/16.
//

#ifndef ENGINE_SPRITEBUFFER_H
#define ENGINE_SPRITEBUFFER_H

#include "Sprite.h"

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "config.h"

namespace fgfx {

  class SpriteBuffer {
  protected:
    std::vector<glm::vec3> spriteVertices;
    std::vector<glm::vec4> spriteColors;
    std::vector<glm::vec2> spriteCoords;
    int spritePointsCount;

    GLuint positionBuffer;
    GLuint colorBuffer;
    GLuint coordBuffer;

    friend class SpriteLayer;
  public:
    int textureId;

    SpriteBuffer(int textureIdp);
    ~SpriteBuffer();

    void reserve(int pointCount);
    void bufferPoint(glm::vec3 pos, glm::vec4 color, glm::vec2 coord);
    void bufferPoint(glm::vec2 pos, glm::vec4 color, glm::vec2 coord);
    void bufferSprite(const std::shared_ptr<Sprite>& sprite, glm::vec2 pos, glm::vec4 color, double size, double rotation);
    void bufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color);
    void rawBufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color);
    void reset();
    void upload();
    void render();
  };

}


#endif //ENGINE_SPRITEBUFFER_H
