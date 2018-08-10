//
// Created by Michał Łaszczewski on 14/04/16.
//

#include "NormalMappedSpriteBuffer.h"
#include "NormalMappedSpriteLayer.h"
#include <cmath>

namespace fgfx {

  NormalMappedSpriteBuffer::NormalMappedSpriteBuffer(int textureIdp) : textureId(textureIdp) {
    glGenBuffers(1, &positionBuffer);
    glGenBuffers(1, &normalBuffer);
    glGenBuffers(1, &colorBuffer);
    glGenBuffers(1, &colorCoordBuffer);
    glGenBuffers(1, &normalCoordBuffer);
    spritePointsCount = 0;
  }
  NormalMappedSpriteBuffer::~NormalMappedSpriteBuffer() {
    glDeleteBuffers(1, &positionBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteBuffers(1, &colorBuffer);
    glDeleteBuffers(1, &colorCoordBuffer);
    glDeleteBuffers(1, &normalCoordBuffer);
  }

  void NormalMappedSpriteBuffer::reserve(int pointCount) {
    spriteVertices.reserve(pointCount);
    spriteNormals.reserve(pointCount);
    spriteColors.reserve(pointCount);
    spriteColorCoords.reserve(pointCount);
    spriteNormalCoords.reserve(pointCount);
  }
  void NormalMappedSpriteBuffer::bufferPoint(glm::vec2 pos, glm::vec2 normal, glm::vec4 color, glm::vec2 colorCoord,
                                             glm::vec2 normalCoord) {
    spriteVertices.push_back(pos);
    spriteNormals.push_back(normal);
    spriteColors.push_back(color);
    spriteColorCoords.push_back(colorCoord);
    spriteNormalCoords.push_back(normalCoord);
    spritePointsCount++;
   // emscripten_log(EM_LOG_ERROR, "BUFFER POINT (%.4lf,%.4lf) (%.2lf,%.2lf,%.2lf,%.2lf) (%.2lf,%.2lf)\n",
    //               pos.x,pos.y,color.r,color.g,color.b,color.a,coord.x,coord.y);
  }
  void NormalMappedSpriteBuffer::bufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color) {
    /*fox_log("BUFFER SPRITE %d (%f %f) (%f %f) (%f %f) (%f %f)", (unsigned int)((unsigned long)sprite.get()),
             sprite->coords[0].x, sprite->coords[0].y, sprite->coords[1].x, sprite->coords[1].y,
             sprite->coords[2].x, sprite->coords[2].y, sprite->coords[3].x, sprite->coords[3].y );*/

    auto r = sprite->ratio*2;
    glm::vec2 normal = glm::normalize(glm::vec2(glm::mat3(mat)*glm::vec3(0, 1, 0)));
    glm::vec2 p1 = glm::vec2(mat*glm::vec4(-1*r, -1, 0, 1));
    glm::vec2 p2 = glm::vec2(mat*glm::vec4(1*r, -1, 0, 1));
    glm::vec2 p3 = glm::vec2(mat*glm::vec4(1*r, 1, 0, 1));
    glm::vec2 p4 = glm::vec2(mat*glm::vec4(-1*r, 1, 0, 1));

    glm::vec2 colorCoords[4] = {
        sprite->coords[0],
        sprite->coords[1],
        (sprite->coords[2] + sprite->coords[1])*0.5f,
        (sprite->coords[3] + sprite->coords[0])*0.5f
    };
    glm::vec2 normalCoords[4] = {
        (sprite->coords[0]+sprite->coords[3])*0.5f, // upper left
        (sprite->coords[1]+sprite->coords[2])*0.5f, // upper right
        sprite->coords[2],
        sprite->coords[3]
    };

    
    bufferPoint(p1, normal, color, colorCoords[0], normalCoords[0]); // upper left
    bufferPoint(p2, normal, color, colorCoords[1], normalCoords[1]); // upper right
    bufferPoint(p3, normal, color, colorCoords[2], normalCoords[2]); // lower right

    bufferPoint(p4, normal, color, colorCoords[3], normalCoords[3]); // lower left
    bufferPoint(p1, normal, color, colorCoords[0], normalCoords[0]); // upper left
    bufferPoint(p3, normal, color, colorCoords[2], normalCoords[2]); // lower right
  }
  void NormalMappedSpriteBuffer::rawBufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color) {
    glm::vec2 normal = glm::normalize(glm::vec2(mat*glm::vec4(1, 0, 0, 0)));
    glm::vec2 p1 = glm::vec2(mat*glm::vec4(-1, -1, 0, 1));
    glm::vec2 p2 = glm::vec2(mat*glm::vec4(1, -1, 0, 1));
    glm::vec2 p3 = glm::vec2(mat*glm::vec4(1, 1, 0, 1));
    glm::vec2 p4 = glm::vec2(mat*glm::vec4(-1, 1, 0, 1));

    glm::vec2 colorCoords[4] = {
        sprite->coords[0]+sprite->coords[3]*0.5f, // upper left
        sprite->coords[1]+sprite->coords[2]*0.5f, // upper right
        sprite->coords[2],
        sprite->coords[3]
    };
    glm::vec2* normalCoords = sprite->coords;

    bufferPoint(p1, normal, color, colorCoords[0], normalCoords[0]); // upper left
    bufferPoint(p2, normal, color, colorCoords[1], normalCoords[1]); // upper right
    bufferPoint(p3, normal, color, colorCoords[2], normalCoords[2]); // lower right

    bufferPoint(p4, normal, color, colorCoords[3], normalCoords[3]); // lower left
    bufferPoint(p1, normal, color, colorCoords[0], normalCoords[0]); // upper left
    bufferPoint(p3, normal, color, colorCoords[2], normalCoords[2]); // lower right
  }
  void NormalMappedSpriteBuffer::reset() {
    spriteVertices.clear();
    spriteNormals.clear();
    spriteColors.clear();
    spriteColorCoords.clear();
    spriteNormalCoords.clear();
    spritePointsCount = 0;
  }
  void NormalMappedSpriteBuffer::upload() {
    /// TODO: test GL_STREAM_DRAW

    if(!spritePointsCount) return;

//    GLfloat* points = (GLfloat*)spriteVertices.data();

    /*printf("FIRST 3 VERTICES (%f %f %f) (%f %f %f) (%f %f %f)\n",
           points[0],points[1],points[2],
           points[3],points[4],points[5],
           points[6],points[7],points[8]);*/

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, spritePointsCount * sizeof(glm::vec2), spriteVertices.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, spritePointsCount * sizeof(glm::vec2), spriteNormals.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, spritePointsCount * sizeof(glm::vec4), spriteColors.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colorCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, spritePointsCount * sizeof(glm::vec2), spriteColorCoords.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normalCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, spritePointsCount * sizeof(glm::vec2), spriteNormalCoords.data(),
                 GL_DYNAMIC_DRAW);

  }
  void NormalMappedSpriteBuffer::render() {
    if(!spritePointsCount) return;

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glVertexAttribPointer( NormalMappedSpriteLayer::spriteProgramAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer( NormalMappedSpriteLayer::spriteProgramAttribNormal, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, colorCoordBuffer);
    glVertexAttribPointer( NormalMappedSpriteLayer::spriteProgramAttribColorCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, normalCoordBuffer);
    glVertexAttribPointer( NormalMappedSpriteLayer::spriteProgramAttribNormalCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glVertexAttribPointer( NormalMappedSpriteLayer::spriteProgramAttribColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, spritePointsCount);
  }

}