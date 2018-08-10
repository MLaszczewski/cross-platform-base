//
// Created by Michał Łaszczewski on 14/04/16.
//

#ifndef FGFX_NormalMappedSpriteBuffer_H
#define FGFX_NormalMappedSpriteBuffer_H

#include "Sprite.h"
#include "config.h"
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace fgfx {

  class NormalMappedSpriteBuffer {
  protected:
    std::vector<glm::vec2> spriteVertices;
    std::vector<glm::vec2> spriteNormals;
    std::vector<glm::vec4> spriteColors;
    std::vector<glm::vec2> spriteColorCoords;
    std::vector<glm::vec2> spriteNormalCoords;
    int spritePointsCount;

    GLuint positionBuffer;
    GLuint normalBuffer;
    GLuint colorBuffer;
    GLuint colorCoordBuffer;
    GLuint normalCoordBuffer;

    friend class NormalMappedSpriteLayer;
  public:
    int textureId;

    NormalMappedSpriteBuffer(int textureIdp);
    ~NormalMappedSpriteBuffer();

    void reserve(int pointCount);
    void bufferPoint(glm::vec2 pos, glm::vec2 normal, glm::vec4 color, glm::vec2 colorCoord, glm::vec2 normalCoord);
    void bufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color);
    void rawBufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color);
    void reset();
    void upload();
    void render();
  };

}


#endif //ENGINE_NormalMappedSpriteBuffer_H
