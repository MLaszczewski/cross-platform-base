//
// Created by Michał Łaszczewski on 14/04/16.
//

#include "TextureTransitionBuffer.h"
#include "TextureTransitionLayer.h"
#include <cmath>
#include "config.h"

#define SQRT_2 1.41421356237
#define INV_SQRT_2 0.70710678118

namespace fgfx {

  TextureTransitionBuffer::TextureTransitionBuffer(int textureIdp) : textureId(textureIdp) {
    glGenBuffers(1, &positionBuffer);
    glGenBuffers(1, &colorBuffer);
    glGenBuffers(1, &coord1Buffer);
    glGenBuffers(1, &coord2Buffer);
    textureTransitionPointsCount = 0;
  }
  TextureTransitionBuffer::~TextureTransitionBuffer() {
    glDeleteBuffers(1, &positionBuffer);
    glDeleteBuffers(1, &colorBuffer);
    glDeleteBuffers(1, &coord1Buffer);
    glDeleteBuffers(1, &coord2Buffer);
  }

  void TextureTransitionBuffer::reserve(int pointCount) {
    textureTransitionVertices.reserve(pointCount);
    textureTransitionColors.reserve(pointCount);
    textureTransitionCoords1.reserve(pointCount);
    textureTransitionCoords2.reserve(pointCount);
  }
  void TextureTransitionBuffer::bufferPoint(glm::vec3 pos, glm::vec4 color, glm::vec3 coord1, glm::vec3 coord2) {
    textureTransitionVertices.push_back(pos);
    textureTransitionColors.push_back(color);
    textureTransitionCoords1.push_back(coord1);
    textureTransitionCoords2.push_back(coord2);
    textureTransitionPointsCount++;
    /*fgfx_log("BUFFER POINT (%.4lf,%.4lf,%.4lf) (%.2lf,%.2lf,%.2lf,%.2lf) (%.2lf,%.2lf)*%.2lf + (%.2lf,%.2lf)*%.2lf\n",
                   pos.x,pos.y,pos.z,color.r,color.g,color.b,color.a,coord1.x,coord1.y,coord1.z,coord2.x,coord2.y,coord2.z);//*/
  }
  void TextureTransitionBuffer::bufferPoint(glm::vec2 pos, glm::vec4 color, glm::vec3 coord1, glm::vec3 coord2) {
    textureTransitionVertices.push_back(glm::vec3(pos.x,pos.y,0));
    textureTransitionColors.push_back(color);
    textureTransitionCoords1.push_back(coord1);
    textureTransitionCoords2.push_back(coord2);
    textureTransitionPointsCount++;
    // emscripten_log(EM_LOG_ERROR, "BUFFER POINT (%.4lf,%.4lf) (%.2lf,%.2lf,%.2lf,%.2lf) (%.2lf,%.2lf)\n",
    //               pos.x,pos.y,color.r,color.g,color.b,color.a,coord.x,coord.y);
  }
  void TextureTransitionBuffer::reset() {
    textureTransitionVertices.clear();
    textureTransitionColors.clear();
    textureTransitionCoords1.clear();
    textureTransitionCoords2.clear();
    textureTransitionPointsCount=0;
  }
  void TextureTransitionBuffer::upload() {
    /// TODO: test GL_STREAM_DRAW

    if(!textureTransitionPointsCount) return;

/*    GLfloat* points = (GLfloat*)textureTransitionVertices.data();
    fgfx_log("FIRST 3 VERTICES (%f %f %f) (%f %f %f) (%f %f %f)\n",
           points[0],points[1],points[2],
           points[3],points[4],points[5],
           points[6],points[7],points[8]);*/

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, textureTransitionPointsCount * sizeof(glm::vec3), textureTransitionVertices.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, textureTransitionPointsCount * sizeof(glm::vec4), textureTransitionColors.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, coord1Buffer);
    glBufferData(GL_ARRAY_BUFFER, textureTransitionPointsCount * sizeof(glm::vec3), textureTransitionCoords1.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, coord2Buffer);
    glBufferData(GL_ARRAY_BUFFER, textureTransitionPointsCount * sizeof(glm::vec3), textureTransitionCoords2.data(),
                 GL_DYNAMIC_DRAW);

  }
  void TextureTransitionBuffer::render() {
    if(!textureTransitionPointsCount) return;

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glVertexAttribPointer(TextureTransitionLayer::textureTransitionProgramAttribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glVertexAttribPointer( TextureTransitionLayer::textureTransitionProgramAttribColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, coord1Buffer);
    glVertexAttribPointer( TextureTransitionLayer::textureTransitionProgramAttribCoord1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, coord2Buffer);
    glVertexAttribPointer( TextureTransitionLayer::textureTransitionProgramAttribCoord2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, textureTransitionPointsCount);
  }

}