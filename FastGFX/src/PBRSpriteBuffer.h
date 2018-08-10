//
// Created by Michał Łaszczewski on 14/04/16.
//

#ifndef FGFX_PBRSpriteBuffer_H
#define FGFX_PBRSpriteBuffer_H

#include "config.h"
#include "Sprite.h"
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace fgfx {

  class PBRSpriteBuffer {
  protected:
    std::vector<glm::vec2> spriteVertices;
    std::vector<glm::vec2> spriteNormals;
    std::vector<glm::vec4> spriteColors;
    std::vector<glm::vec2> spriteColorCoords;
    std::vector<glm::vec2> spriteNormalCoords;
    std::vector<glm::vec2> spriteMaterialCoords;
    int spritePointsCount;

    GLuint positionBuffer;
    GLuint normalBuffer;
    GLuint colorBuffer;
    GLuint colorCoordBuffer;
    GLuint normalCoordBuffer;
    GLuint materialCoordBuffer;

    friend class PBRSpriteLayer;
  public:
    int textureId;

    PBRSpriteBuffer(int textureIdp);
    ~PBRSpriteBuffer();

    void reserve(int pointCount);
    
    void bufferPoint(glm::vec2 pos, glm::vec2 normal, glm::vec4 color, glm::vec2 colorCoord, glm::vec2 normalCoord, glm::vec2 materialCoord);
    void bufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color);
    void rawBufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color);
    
    void reset();
    void upload();
    void render();
  };

}


#endif //TANKS_NormalMappedSpriteBuffer_H
