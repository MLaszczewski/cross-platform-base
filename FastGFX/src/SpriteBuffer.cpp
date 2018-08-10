//
// Created by Michał Łaszczewski on 14/04/16.
//

#include "SpriteBuffer.h"
#include "SpriteLayer.h"
#include <cmath>
#include "config.h"

#define SQRT_2 1.41421356237
#define INV_SQRT_2 0.70710678118

namespace fgfx {
  
  SpriteBuffer::SpriteBuffer(int textureIdp) : textureId(textureIdp) {
    glGenBuffers(1, &positionBuffer);
    glGenBuffers(1, &colorBuffer);
    glGenBuffers(1, &coordBuffer);
    spritePointsCount = 0;
  }
  SpriteBuffer::~SpriteBuffer() {
    glDeleteBuffers(1, &positionBuffer);
    glDeleteBuffers(1, &colorBuffer);
    glDeleteBuffers(1, &coordBuffer);
  }

  void SpriteBuffer::reserve(int pointCount) {
    spriteVertices.reserve(pointCount);
    spriteColors.reserve(pointCount);
    spriteCoords.reserve(pointCount);
  }
  void SpriteBuffer::bufferPoint(glm::vec3 pos, glm::vec4 color, glm::vec2 coord) {
    spriteVertices.push_back(pos);
    spriteColors.push_back(color);
    spriteCoords.push_back(coord);
    spritePointsCount++;
    //emscripten_log(EM_LOG_ERROR, "BUFFER POINT (%.4lf,%.4lf) (%.2lf,%.2lf,%.2lf,%.2lf) (%.2lf,%.2lf)\n",
    //               pos.x,pos.y,color.r,color.g,color.b,color.a,coord.x,coord.y);
  }
  void SpriteBuffer::bufferPoint(glm::vec2 pos, glm::vec4 color, glm::vec2 coord) {
    spriteVertices.push_back(glm::vec3(pos.x,pos.y,0));
    spriteColors.push_back(color);
    spriteCoords.push_back(coord);
    spritePointsCount++;
   // emscripten_log(EM_LOG_ERROR, "BUFFER POINT (%.4lf,%.4lf) (%.2lf,%.2lf,%.2lf,%.2lf) (%.2lf,%.2lf)\n",
    //               pos.x,pos.y,color.r,color.g,color.b,color.a,coord.x,coord.y);
  }
  void SpriteBuffer::bufferSprite(const std::shared_ptr<Sprite>& sprite, glm::vec2 pos, glm::vec4 color, double size, double rotation) {
    double sv = sin(rotation); // Rotation
    double cv = cos(rotation);
    glm::vec2 up(sv * size, -cv * size);
    glm::vec2 right(cv * size * sprite->ratio, sv * size * sprite->ratio);
    glm::vec2 m1 = up - right;
    glm::vec2 m2 = up + right;

    bufferPoint(pos + m1, color, sprite->coords[0]); // upper left
    bufferPoint(pos + m2, color, sprite->coords[1]); // upper right
    bufferPoint(pos - m1, color, sprite->coords[2]); // lower right

    bufferPoint(pos - m2, color, sprite->coords[3]); // lower left
    bufferPoint(pos + m1, color, sprite->coords[0]); // upper left
    bufferPoint(pos - m1, color, sprite->coords[2]); // lower right
  }
  void SpriteBuffer::bufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color) {
    auto r = sprite->ratio;
    glm::vec3 p1 = glm::vec3(mat*glm::vec4(-1*r,-1,0,1));
    glm::vec3 p2 = glm::vec3(mat*glm::vec4(1*r,-1,0,1));
    glm::vec3 p3 = glm::vec3(mat*glm::vec4(1*r,1,0,1));
    glm::vec3 p4 = glm::vec3(mat*glm::vec4(-1*r,1,0,1));

    bufferPoint(p1, color, sprite->coords[0]); // upper left
    bufferPoint(p2, color, sprite->coords[1]); // upper right
    bufferPoint(p3, color, sprite->coords[2]); // lower right

    bufferPoint(p4, color, sprite->coords[3]); // lower left
    bufferPoint(p1, color, sprite->coords[0]); // upper left
    bufferPoint(p3, color, sprite->coords[2]); // lower right
  }
  void SpriteBuffer::rawBufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color) {
    glm::vec3 p1 = glm::vec3(mat*glm::vec4(-1,-1,0,1));
    glm::vec3 p2 = glm::vec3(mat*glm::vec4(1,-1,0,1));
    glm::vec3 p3 = glm::vec3(mat*glm::vec4(1,1,0,1));
    glm::vec3 p4 = glm::vec3(mat*glm::vec4(-1,1,0,1));

    bufferPoint(p1, color, sprite->coords[0]); // upper left
    bufferPoint(p2, color, sprite->coords[1]); // upper right
    bufferPoint(p3, color, sprite->coords[2]); // lower right

    bufferPoint(p4, color, sprite->coords[3]); // lower left
    bufferPoint(p1, color, sprite->coords[0]); // upper left
    bufferPoint(p3, color, sprite->coords[2]); // lower right
  }
  void SpriteBuffer::reset() {
    spriteVertices.clear();
    spriteColors.clear();
    spriteCoords.clear();
    spritePointsCount=0;
  }
  void SpriteBuffer::upload() {
    /// TODO: test GL_STREAM_DRAW

    if(!spritePointsCount) return;

//    GLfloat* points = (GLfloat*)spriteVertices.data();

    /*printf("FIRST 3 VERTICES (%f %f %f) (%f %f %f) (%f %f %f)\n",
           points[0],points[1],points[2],
           points[3],points[4],points[5],
           points[6],points[7],points[8]);*/

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, spritePointsCount*sizeof(glm::vec3),spriteVertices.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, spritePointsCount*sizeof(glm::vec4),spriteColors.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
    glBufferData(GL_ARRAY_BUFFER, spritePointsCount*sizeof(glm::vec2),spriteCoords.data(),
                 GL_DYNAMIC_DRAW);

  }
  void SpriteBuffer::render() {
    if(!spritePointsCount) return;

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glVertexAttribPointer(SpriteLayer::spriteProgramAttribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
    glVertexAttribPointer( SpriteLayer::spriteProgramAttribCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glVertexAttribPointer( SpriteLayer::spriteProgramAttribColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, spritePointsCount);
  }

}