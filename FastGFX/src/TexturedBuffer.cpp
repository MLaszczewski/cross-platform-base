//
// Created by Michał Łaszczewski on 14/04/16.
//

#include "TexturedBuffer.h"
#include "TexturedLayer.h"
#include <cmath>
#include "config.h"

#define SQRT_2 1.41421356237
#define INV_SQRT_2 0.70710678118

namespace fgfx {

  TexturedBuffer::TexturedBuffer(int textureIdp) : textureId(textureIdp) {
    glGenBuffers(1, &positionBuffer);
    glGenBuffers(1, &colorBuffer);
    glGenBuffers(1, &coordBuffer);
    texturedPointsCount = 0;
  }
  TexturedBuffer::~TexturedBuffer() {
    glDeleteBuffers(1, &positionBuffer);
    glDeleteBuffers(1, &colorBuffer);
    glDeleteBuffers(1, &coordBuffer);
  }

  void TexturedBuffer::reserve(int pointCount) {
    texturedVertices.reserve(pointCount);
    texturedColors.reserve(pointCount);
    texturedCoords.reserve(pointCount);
  }
  void TexturedBuffer::bufferPoint(glm::vec3 pos, glm::vec4 color, glm::vec2 coord) {
    texturedVertices.push_back(pos);
    texturedColors.push_back(color);
    texturedCoords.push_back(coord);
    texturedPointsCount++;
    //emscripten_log(EM_LOG_ERROR, "BUFFER POINT (%.4lf,%.4lf) (%.2lf,%.2lf,%.2lf,%.2lf) (%.2lf,%.2lf)\n",
    //               pos.x,pos.y,color.r,color.g,color.b,color.a,coord.x,coord.y);
  }
  void TexturedBuffer::bufferPoint(glm::vec2 pos, glm::vec4 color, glm::vec2 coord) {
    texturedVertices.push_back(glm::vec3(pos.x,pos.y,0));
    texturedColors.push_back(color);
    texturedCoords.push_back(coord);
    texturedPointsCount++;
    // emscripten_log(EM_LOG_ERROR, "BUFFER POINT (%.4lf,%.4lf) (%.2lf,%.2lf,%.2lf,%.2lf) (%.2lf,%.2lf)\n",
    //               pos.x,pos.y,color.r,color.g,color.b,color.a,coord.x,coord.y);
  }
  void TexturedBuffer::reset() {
    texturedVertices.clear();
    texturedColors.clear();
    texturedCoords.clear();
    texturedPointsCount=0;
  }
  void TexturedBuffer::upload() {
    /// TODO: test GL_STREAM_DRAW

    if(!texturedPointsCount) return;

//    GLfloat* points = (GLfloat*)texturedVertices.data();

    /*printf("FIRST 3 VERTICES (%f %f %f) (%f %f %f) (%f %f %f)\n",
           points[0],points[1],points[2],
           points[3],points[4],points[5],
           points[6],points[7],points[8]);*/

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, texturedPointsCount*sizeof(glm::vec3),texturedVertices.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, texturedPointsCount*sizeof(glm::vec4),texturedColors.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
    glBufferData(GL_ARRAY_BUFFER, texturedPointsCount*sizeof(glm::vec2),texturedCoords.data(),
                 GL_DYNAMIC_DRAW);

  }
  void TexturedBuffer::render() {
    if(!texturedPointsCount) return;

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glVertexAttribPointer(TexturedLayer::texturedProgramAttribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
    glVertexAttribPointer( TexturedLayer::texturedProgramAttribCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glVertexAttribPointer( TexturedLayer::texturedProgramAttribColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, texturedPointsCount);
  }

}