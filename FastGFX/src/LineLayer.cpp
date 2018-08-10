//
// Created by Michał Łaszczewski on 20/04/16.
//

#include "LineLayer.h"


#include <glm/gtc/type_ptr.hpp>
#include "config.h"

namespace fgfx {

  GLuint LineLayer::lineProgram = 0;
  GLuint LineLayer::lineProgramAttribColor = 0;
  GLuint LineLayer::lineProgramAttribPosition = 0;
  GLuint LineLayer::lineProgramUniformCameraMatrix = 0;
  
  void LineLayer::initializeLineProgram() {

    const char vShaderStr[] =
      "attribute vec3 aVertexPosition;\n"\
      "attribute vec4 aVertexColor;\n"
      "uniform mat4 uCameraMatrix;\n"
      "varying vec4 vColor;\n"
      "void main() {\n"
      "  gl_Position = uCameraMatrix*vec4(aVertexPosition, 1.0);\n"
      "  vColor = aVertexColor;\n"
      "}\n";

#ifndef __USE_OPENGL
    const char fShaderStr[] =
      "precision mediump float;\n"\
      "varying vec4 vColor;\n"
      "void main() {\n"
      "  gl_FragColor = vColor;\n"
      "}\n";
#endif
#ifdef __USE_OPENGL
    const char fShaderStr[] =
      "varying vec4 vColor;\n"
      "void main() {\n"
      "  gl_FragColor = vColor;\n"
      "}\n";
#endif

    auto vertexShader = fgfx::loadShader(GL_VERTEX_SHADER, vShaderStr);
    auto fragmentShader = fgfx::loadShader(GL_FRAGMENT_SHADER, fShaderStr);

    auto lineProgram = fgfx::createProgram(vertexShader, fragmentShader);
    if (!lineProgram) throw "nie ma programu!";
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    LineLayer::lineProgram = lineProgram;

    LineLayer::lineProgramAttribPosition = glGetAttribLocation(lineProgram, "aVertexPosition");
    LineLayer::lineProgramAttribColor = glGetAttribLocation(lineProgram, "aVertexColor");
    LineLayer::lineProgramUniformCameraMatrix = glGetUniformLocation(lineProgram, "uCameraMatrix");
  }

  void LineLayer::beginDraw(const ModelViewProjection& mvp) {
    glUseProgram(LineLayer::lineProgram);
    glEnableVertexAttribArray(LineLayer::lineProgramAttribPosition);
    glEnableVertexAttribArray(LineLayer::lineProgramAttribColor);

    glUniformMatrix4fv(LineLayer::lineProgramUniformCameraMatrix, 1, GL_FALSE,
                       (GLfloat*)glm::value_ptr(mvp.multipliedMatrix));
  }

  void LineLayer::endDraw() {
    glDisableVertexAttribArray(LineLayer::lineProgramAttribPosition);
    glDisableVertexAttribArray(LineLayer::lineProgramAttribColor);
  }


  LineLayer::LineLayer(Engine* enginep) : TypedLayer<LineLayer>(enginep), linePointsCount(0) {
    glGenBuffers(1, &positionBuffer);
    glGenBuffers(1, &colorBuffer);
  }
  LineLayer::~LineLayer() {
    glDeleteBuffers(1, &positionBuffer);
    glDeleteBuffers(1, &colorBuffer);
  }

  void LineLayer::reserve(int pointCount) {
    lineVertices.reserve(pointCount);
    lineColors.reserve(pointCount);
  }

  void LineLayer::bufferPoint(glm::vec3 pos, glm::vec4 color) {
    if(frozen && uploaded) return;
    /*emscripten_log(EM_LOG_ERROR,"BUFFER LINE POINT %d (%f %f %f) COLOR (%f %f %f %f)\n",linePointsCount,
                   pos.x,pos.y,pos.z,color.r,color.g,color.b,color.a);*/
    lineVertices.push_back(pos);
    lineColors.push_back(color);
    linePointsCount++;
  }
  void LineLayer::bufferLine(glm::vec3 p1, glm::vec3 p2, glm::vec4 color) {
    bufferPoint(p1, color);
    bufferPoint(p2, color);
  }

  void LineLayer::reset() {
    if(frozen) return;
    lineVertices.clear();
    lineColors.clear();
    linePointsCount = 0;
    uploaded=false;
  }
  void LineLayer::upload() {
    if(!visible) return;
    if(uploaded) return;
    if(!linePointsCount) return;

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, linePointsCount * sizeof(glm::vec3),lineVertices.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, linePointsCount * sizeof(glm::vec4),lineColors.data(),
                 GL_DYNAMIC_DRAW);

    uploaded=true;
  }
  void LineLayer::render(const ModelViewProjection& mvp) {
    if(!visible) return;
    if(!linePointsCount) return;

    beginDraw(mvp);

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glVertexAttribPointer(LineLayer::lineProgramAttribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glVertexAttribPointer(LineLayer::lineProgramAttribColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_LINES, 0, linePointsCount);

    endDraw();
  }

  void LineLayer::destroyLineProgram() {
    glDeleteProgram(lineProgram);
  }
}