#include "TextureTransitionLayer.h"
#include "TextureTransitionBuffer.h"
#include "Engine.h"

#include <glm/gtc/type_ptr.hpp>
#include "config.h"

namespace fgfx {

  TextureTransitionLayer::TextureTransitionLayer(Engine* enginep) : TypedLayer<TextureTransitionLayer>(enginep) {
    composition=1;
  };

  TextureTransitionBuffer* TextureTransitionLayer::getBuffer(int textureId) {
    for(auto it=buffers.begin(); it!=buffers.end(); ++it) {
      if((*it)->textureId == textureId) return *it;
    }
    TextureTransitionBuffer* buf = new TextureTransitionBuffer(textureId);
    buffers.push_back(buf);
    return buf;
  }

  GLuint TextureTransitionLayer::textureTransitionProgram = 0;
  GLuint TextureTransitionLayer::textureTransitionProgramAttribColor = 0;
  GLuint TextureTransitionLayer::textureTransitionProgramAttribPosition = 0;
  GLuint TextureTransitionLayer::textureTransitionProgramAttribCoord1 = 0;
  GLuint TextureTransitionLayer::textureTransitionProgramAttribCoord2 = 0;
  GLuint TextureTransitionLayer::textureTransitionProgramUniformSampler = 0;
  GLuint TextureTransitionLayer::textureTransitionProgramUniformCameraMatrix = 0;

  void TextureTransitionLayer::initializeTextureTransitionProgram() {

#ifndef __USE_OPENGL
    const char vShaderStr[] =
        "attribute vec3 aVertexPosition;\n"
        "attribute vec4 aVertexColor;\n"
        "attribute vec3 aTextureCoord1;\n"
        "attribute vec3 aTextureCoord2;\n"
        "uniform mat4 uCameraMatrix;\n"
        "varying vec4 vColor;\n"
        "varying highp vec3 vTextureCoord1;\n"
        "varying highp vec3 vTextureCoord2;\n"
        "void main() {\n"
        "  gl_Position = uCameraMatrix * vec4(aVertexPosition, 1.0);\n"
        "  vColor = aVertexColor;\n"
        "  vTextureCoord1 = aTextureCoord1;\n"
        "  vTextureCoord2 = aTextureCoord2;\n"
        "}\n";

    const char fShaderStr[] =
        "precision mediump float;\n"\
        "varying vec4 vColor;\n"
        "uniform sampler2D uSampler;\n"
        "varying highp vec3 vTextureCoord1;\n"
        "varying highp vec3 vTextureCoord2;\n"
        "void main() {\n"
        "  vec4 textureColor1 = texture2D(uSampler, vTextureCoord1.xy);\n"
        "  vec4 textureColor2 = texture2D(uSampler, vTextureCoord2.xy);\n"
        "  gl_FragColor = vColor * (textureColor1 * vTextureCoord1.z + textureColor2 * vTextureCoord2.z);\n"
        "}\n";
#endif

#ifdef __USE_OPENGL
    const char vShaderStr[] = R"GLSL(
      attribute vec3 aVertexPosition;
      attribute vec4 aVertexColor;
      attribute vec3 aTextureCoord1;
      attribute vec3 aTextureCoord2;
      uniform mat4 uCameraMatrix;
      varying vec4 vColor;
      varying vec3 vTextureCoord1;
      varying vec3 vTextureCoord2;
      void main() {
        gl_Position = uCameraMatrix * vec4(aVertexPosition, 1.0);
        vColor = aVertexColor;
        vTextureCoord1 = aTextureCoord1;
        vTextureCoord2 = aTextureCoord2;
      }
    )GLSL";
        

    const char fShaderStr[] = R"GLSL(
      varying vec4 vColor;
      uniform sampler2D uSampler;
      varying vec3 vTextureCoord1;
      varying vec3 vTextureCoord2;
      void main() {
        vec4 textureColor1 = texture2D(uSampler, vTextureCoord1.xy);
        vec4 textureColor2 = texture2D(uSampler, vTextureCoord2.xy);
        gl_FragColor = vColor * ((textureColor1 * vTextureCoord1.z + textureColor2 * vTextureCoord2.z));
        //gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0) ;
      }
    )GLSL";
#endif

    auto vertexShader = fgfx::loadShader ( GL_VERTEX_SHADER, vShaderStr );
    auto fragmentShader = fgfx::loadShader ( GL_FRAGMENT_SHADER, fShaderStr );

    auto textureTransitionProgram = fgfx::createProgram (vertexShader, fragmentShader);
    if(!textureTransitionProgram) {
      fgfx_log("CANNOT CREATE SHADER PROGRAM!")
      throw "CANNOT CREATE SHADER PROGRAM!";
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    TextureTransitionLayer::textureTransitionProgram = textureTransitionProgram;

    TextureTransitionLayer::textureTransitionProgramAttribPosition = glGetAttribLocation(textureTransitionProgram,"aVertexPosition");
    TextureTransitionLayer::textureTransitionProgramAttribColor = glGetAttribLocation(textureTransitionProgram,"aVertexColor");
    TextureTransitionLayer::textureTransitionProgramAttribCoord1 = glGetAttribLocation(textureTransitionProgram,"aTextureCoord1");
    TextureTransitionLayer::textureTransitionProgramAttribCoord2 = glGetAttribLocation(textureTransitionProgram,"aTextureCoord2");

    TextureTransitionLayer::textureTransitionProgramUniformCameraMatrix = glGetUniformLocation(textureTransitionProgram,"uCameraMatrix");
    TextureTransitionLayer::textureTransitionProgramUniformSampler = glGetUniformLocation(textureTransitionProgram,"uSampler");
  }

  void TextureTransitionLayer::beginDraw(const ModelViewProjection& mvp) {
    glUseProgram(TextureTransitionLayer::textureTransitionProgram);
    glEnableVertexAttribArray(TextureTransitionLayer::textureTransitionProgramAttribPosition);
    glEnableVertexAttribArray(TextureTransitionLayer::textureTransitionProgramAttribColor);
    glEnableVertexAttribArray(TextureTransitionLayer::textureTransitionProgramAttribCoord1);
    glEnableVertexAttribArray(TextureTransitionLayer::textureTransitionProgramAttribCoord2);

    glUniformMatrix4fv(TextureTransitionLayer::textureTransitionProgramUniformCameraMatrix, 1, GL_FALSE,
                       (GLfloat*)glm::value_ptr(mvp.multipliedMatrix));
  }

  void TextureTransitionLayer::endDraw() {
    glDisableVertexAttribArray(TextureTransitionLayer::textureTransitionProgramAttribPosition);
    glDisableVertexAttribArray(TextureTransitionLayer::textureTransitionProgramAttribColor);
    glDisableVertexAttribArray(TextureTransitionLayer::textureTransitionProgramAttribCoord1);
    glDisableVertexAttribArray(TextureTransitionLayer::textureTransitionProgramAttribCoord2);
  }

  void TextureTransitionLayer::reset() {
    for(auto it = buffers.begin(); it!=buffers.end(); ++it) {
      TextureTransitionBuffer *buffer = *it;
      buffer->reset();
    }
  }

  void TextureTransitionLayer::upload() {
    if(!visible) return;
    for(auto it = buffers.begin(); it!=buffers.end(); ++it) {
      TextureTransitionBuffer *buffer = *it;
      buffer->upload();
    }
  }

  void TextureTransitionLayer::render(const ModelViewProjection& mvp) {
    if(!visible) return;
    beginDraw(mvp);
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_CULL_FACE);
    for(auto it = buffers.begin(); it!=buffers.end(); ++it) {
      TextureTransitionBuffer* buffer=*it;
      glBindTexture(GL_TEXTURE_2D, buffer->textureId);
      glUniform1i(TextureTransitionLayer::textureTransitionProgramUniformSampler, 0);
      buffer->render();
    }
    endDraw();
  }

  void TextureTransitionLayer::bufferTextureTransitionPoint(const std::shared_ptr<Texture>& texture, glm::vec2 point,
                                                            glm::vec4 color, glm::vec3 coord1, glm::vec3 coord2) {
    if(texture->unloaded) engine->reloadTexture(texture);
    if(texture->texture == -1) return;
    getBuffer(texture->texture)->bufferPoint(point, color, coord1, coord2);
  }
  void TextureTransitionLayer::bufferTextureTransitionPoint(const std::shared_ptr<Texture>& texture, glm::vec3 point,
                                                            glm::vec4 color, glm::vec3 coord1, glm::vec3 coord2) {
    if(texture->unloaded) engine->reloadTexture(texture);
    if(texture->texture == -1) return;
    getBuffer(texture->texture)->bufferPoint(point, color, coord1, coord2);
  }
  TextureTransitionBuffer* TextureTransitionLayer::getBuffer(const std::shared_ptr<Texture>& texture) {
    if(texture->unloaded) engine->reloadTexture(texture);
    if(texture->texture == -1) return nullptr;
    return getBuffer(texture->texture);
  }

  void TextureTransitionLayer::destroyTextureTransitionProgram() {
    glDeleteProgram(textureTransitionProgram);
  }

}
