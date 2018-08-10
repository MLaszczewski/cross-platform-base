#include "TexturedLayer.h"
#include "TexturedBuffer.h"
#include "Engine.h"

#include <glm/gtc/type_ptr.hpp>
#include "config.h"

namespace fgfx {

  TexturedLayer::TexturedLayer(Engine* enginep) : TypedLayer<TexturedLayer>(enginep) {
    composition=1;
  };

  TexturedBuffer* TexturedLayer::getBuffer(int textureId) {
    for(auto it=buffers.begin(); it!=buffers.end(); ++it) {
      if((*it)->textureId == textureId) return *it;
    }
    TexturedBuffer* buf = new TexturedBuffer(textureId);
    buffers.push_back(buf);
    return buf;
  }

  GLuint TexturedLayer::texturedProgram = 0;
  GLuint TexturedLayer::texturedProgramAttribColor = 0;
  GLuint TexturedLayer::texturedProgramAttribPosition = 0;
  GLuint TexturedLayer::texturedProgramAttribCoord = 0;
  GLuint TexturedLayer::texturedProgramUniformSampler = 0;
  GLuint TexturedLayer::texturedProgramUniformCameraMatrix = 0;

  void TexturedLayer::initializeTexturedProgram() {

#ifndef __USE_OPENGL
    const char vShaderStr[] =
        "attribute vec3 aVertexPosition;\n"
        "attribute vec4 aVertexColor;\n"
        "attribute vec2 aTextureCoord;\n"
        "uniform mat4 uCameraMatrix;\n"
        "varying vec4 vColor;\n"
        "varying highp vec2 vTextureCoord;\n"
        "void main() {\n"
        "  gl_Position = uCameraMatrix*vec4(aVertexPosition, 1.0);\n"
        "  vColor = aVertexColor;\n"
        "  vTextureCoord = aTextureCoord;\n"
        "}\n";

    const char fShaderStr[] =
        "precision mediump float;\n"\
        "varying vec4 vColor;\n"
        "uniform sampler2D uSampler;\n"
        "varying highp vec2 vTextureCoord;\n"
        "void main() {\n"
        "  vec4 textureColor = texture2D(uSampler, vTextureCoord);\n"
        "  gl_FragColor = vColor*textureColor;\n"
        "}\n";
#endif

#ifdef __USE_OPENGL
    const char vShaderStr[] =
        "attribute vec3 aVertexPosition;\n"
            "attribute vec4 aVertexColor;\n"
            "attribute vec2 aTextureCoord;\n"
            "uniform mat4 uCameraMatrix;\n"
            "varying vec4 vColor;\n"
            "varying vec2 vTextureCoord;\n"
            "void main() {\n"
            "  gl_Position = uCameraMatrix*vec4(aVertexPosition, 1.0);\n"
            "  vColor = aVertexColor;\n"
            "  vTextureCoord = aTextureCoord;\n"
            "}\n";

    const char fShaderStr[] =
        "varying vec4 vColor;\n"\
        "uniform sampler2D uSampler;\n"
            "varying vec2 vTextureCoord;\n"
            "void main() {\n"
            "  vec4 textureColor = texture2D(uSampler, vTextureCoord);\n"
            "  gl_FragColor = vColor*textureColor;\n"
            "}\n";
#endif

    auto vertexShader = fgfx::loadShader ( GL_VERTEX_SHADER, vShaderStr );
    auto fragmentShader = fgfx::loadShader ( GL_FRAGMENT_SHADER, fShaderStr );

    auto texturedProgram = fgfx::createProgram (vertexShader, fragmentShader);
    if(!texturedProgram) throw "nie ma programu!";
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    TexturedLayer::texturedProgram = texturedProgram;

    TexturedLayer::texturedProgramAttribPosition = glGetAttribLocation(texturedProgram,"aVertexPosition");
    TexturedLayer::texturedProgramAttribColor = glGetAttribLocation(texturedProgram,"aVertexColor");
    TexturedLayer::texturedProgramAttribCoord = glGetAttribLocation(texturedProgram,"aTextureCoord");

    TexturedLayer::texturedProgramUniformCameraMatrix = glGetUniformLocation(texturedProgram,"uCameraMatrix");
    TexturedLayer::texturedProgramUniformSampler = glGetUniformLocation(texturedProgram,"uSampler");
  }

  void TexturedLayer::beginDraw(const ModelViewProjection& mvp) {
    glUseProgram(TexturedLayer::texturedProgram);
    glEnableVertexAttribArray(TexturedLayer::texturedProgramAttribPosition);
    glEnableVertexAttribArray(TexturedLayer::texturedProgramAttribColor);
    glEnableVertexAttribArray(TexturedLayer::texturedProgramAttribCoord);

    glUniformMatrix4fv(TexturedLayer::texturedProgramUniformCameraMatrix, 1, GL_FALSE,
                       (GLfloat*)glm::value_ptr(mvp.multipliedMatrix));
  }

  void TexturedLayer::endDraw() {
    glDisableVertexAttribArray(TexturedLayer::texturedProgramAttribPosition);
    glDisableVertexAttribArray(TexturedLayer::texturedProgramAttribColor);
    glDisableVertexAttribArray(TexturedLayer::texturedProgramAttribCoord);
  }

  void TexturedLayer::reset() {
    for(auto it = buffers.begin(); it!=buffers.end(); ++it) {
      TexturedBuffer *buffer = *it;
      buffer->reset();
    }
  }

  void TexturedLayer::upload() {
    if(!visible) return;
    for(auto it = buffers.begin(); it!=buffers.end(); ++it) {
      TexturedBuffer *buffer = *it;
      buffer->upload();
    }
  }

  void TexturedLayer::render(const ModelViewProjection& mvp) {
    if(!visible) return;
    beginDraw(mvp);
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_CULL_FACE);
    for(auto it = buffers.begin(); it!=buffers.end(); ++it) {
      TexturedBuffer* buffer=*it;
      glBindTexture(GL_TEXTURE_2D, buffer->textureId);
      glUniform1i(TexturedLayer::texturedProgramUniformSampler, 0);
      buffer->render();
    }
    endDraw();
  }

  void TexturedLayer::bufferTexturedPoint(const std::shared_ptr<Texture>& texture, glm::vec2 point, glm::vec2 coord, glm::vec4 color) {
    if(texture->unloaded) engine->reloadTexture(texture);
    if(texture->texture == -1) return;
    getBuffer(texture->texture)->bufferPoint(point, color, coord);
  }
  void TexturedLayer::bufferTexturedPoint(const std::shared_ptr<Texture>& texture, glm::vec3 point, glm::vec2 coord, glm::vec4 color) {
    if(texture->unloaded) engine->reloadTexture(texture);
    if(texture->texture == -1) return;
    getBuffer(texture->texture)->bufferPoint(point, color, coord);
  }
  TexturedBuffer* TexturedLayer::getBuffer(const std::shared_ptr<Texture>& texture) {
    fgfx_log("GET BUFFER %d", texture->texture);
    if(texture->unloaded) engine->reloadTexture(texture);
    if(texture->texture == -1) return nullptr;
    return getBuffer(texture->texture);
  }

  void TexturedLayer::destroyTexturedProgram() {
    glDeleteProgram(texturedProgram);
  }

}
