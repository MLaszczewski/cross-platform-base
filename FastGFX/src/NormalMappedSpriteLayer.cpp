#include <fastgfx.h>
#include <glm/gtc/type_ptr.hpp>

namespace fgfx {

  NormalMappedSpriteLayer::NormalMappedSpriteLayer(fgfx::Engine* enginep) : Layer(enginep) {
    composition = 1;
  };

  NormalMappedSpriteBuffer* NormalMappedSpriteLayer::getBuffer(int textureId) {
    for(auto it=buffers.begin(); it!=buffers.end(); ++it) {
      if((*it)->textureId == textureId) return *it;
    }
    NormalMappedSpriteBuffer* buf = new NormalMappedSpriteBuffer(textureId);
    buffers.push_back(buf);
    return buf;
  }

  GLuint NormalMappedSpriteLayer::spriteProgram = 0;
  GLuint NormalMappedSpriteLayer::spriteProgramAttribColor = 0;
  GLuint NormalMappedSpriteLayer::spriteProgramAttribPosition = 0;
  GLuint NormalMappedSpriteLayer::spriteProgramAttribNormal = 0;
  GLuint NormalMappedSpriteLayer::spriteProgramAttribColorCoord = 0;
  GLuint NormalMappedSpriteLayer::spriteProgramAttribNormalCoord = 0;
  GLuint NormalMappedSpriteLayer::spriteProgramUniformSampler = 0;
  GLuint NormalMappedSpriteLayer::spriteProgramUniformCameraMatrix = 0;

  void NormalMappedSpriteLayer::initializeSpriteProgram() {

    const char vShaderStr[] = R"GLSL(
      attribute vec2 aVertexPosition;
      attribute vec2 aVertexNormal;
      attribute vec4 aVertexColor;
      attribute vec2 aColorCoord;
      attribute vec2 aNormalCoord;
      uniform mat4 uCameraMatrix;
      varying vec2 vNormal;
      varying vec4 vColor;
      varying highp vec2 vColorCoord;
      varying highp vec2 vNormalCoord;
      void main() {
        gl_Position = uCameraMatrix*vec4(aVertexPosition, 0.0, 1.0);
        vNormal = aVertexNormal;
        vColor = aVertexColor;
        vColorCoord = aColorCoord;
        vNormalCoord = aNormalCoord;
      }
    )GLSL";

    const char fShaderStr[] = R"GLSL(
      precision mediump float;
      varying vec2 vNormal;
      varying vec4 vColor;
      uniform sampler2D uSampler;
      varying highp vec2 vColorCoord;
      varying highp vec2 vNormalCoord;
      void main() {
        vec4 textureColor = texture2D(uSampler, vColorCoord);
        vec4 color = textureColor * vColor;
        vec4 textureNormalPixel = texture2D(uSampler, vNormalCoord);
        vec3 normal = textureNormalPixel.xyz*2.0 - vec3(1.0, 1.0, 1.0);
        vec3 normalYAxis = vec3(vNormal.x, vNormal.y, 0);
        vec3 normalXAxis = vec3(normalYAxis.y, -normalYAxis.x, 0);
        normal = normalXAxis * normal.x + normalYAxis * normal.y + vec3(0, 0, normal.z);
        normal.xy = normal.xy * -1.0;
        vec3 lightDir = normalize(vec3(-1, -3, 6));
        vec3 eyeDir = normalize(vec3(0, 0, 1));
        vec3 reflection = normalize(-reflect(lightDir, normal));
        float shininess = 23.0;
        vec3 diffuse = color.xyz * max(dot(normal, lightDir), 0.0);
        diffuse = clamp(diffuse, 0.0, 1.0);
        vec3 specular = color.xyz * pow(max(dot(reflection,eyeDir),0.0), 0.3 * shininess);
        gl_FragColor = vec4(diffuse + specular , color.a * textureNormalPixel.a);
      }
    )GLSL";

    auto vertexShader = fgfx::loadShader ( GL_VERTEX_SHADER, vShaderStr );
    auto fragmentShader = fgfx::loadShader ( GL_FRAGMENT_SHADER, fShaderStr );

    auto spriteProgram = fgfx::createProgram (vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if(!spriteProgram) throw "nie ma programu!";
    NormalMappedSpriteLayer::spriteProgram = spriteProgram;

    NormalMappedSpriteLayer::spriteProgramAttribPosition = glGetAttribLocation(spriteProgram,"aVertexPosition");
    NormalMappedSpriteLayer::spriteProgramAttribNormal = glGetAttribLocation(spriteProgram,"aVertexNormal");
    NormalMappedSpriteLayer::spriteProgramAttribColor = glGetAttribLocation(spriteProgram,"aVertexColor");
    NormalMappedSpriteLayer::spriteProgramAttribColorCoord = glGetAttribLocation(spriteProgram,"aColorCoord");
    NormalMappedSpriteLayer::spriteProgramAttribNormalCoord = glGetAttribLocation(spriteProgram,"aNormalCoord");

    NormalMappedSpriteLayer::spriteProgramUniformCameraMatrix = glGetUniformLocation(spriteProgram,"uCameraMatrix");
    NormalMappedSpriteLayer::spriteProgramUniformSampler = glGetUniformLocation(spriteProgram,"uSampler");
  }

  void NormalMappedSpriteLayer::destroySpriteProgram() {
    glDeleteProgram(spriteProgram);
  }

  void NormalMappedSpriteLayer::beginDraw(const fgfx::ModelViewProjection& mvp) {
    glUseProgram(NormalMappedSpriteLayer::spriteProgram);
    glEnableVertexAttribArray(NormalMappedSpriteLayer::spriteProgramAttribPosition);
    glEnableVertexAttribArray(NormalMappedSpriteLayer::spriteProgramAttribColor);
    glEnableVertexAttribArray(NormalMappedSpriteLayer::spriteProgramAttribNormal);
    glEnableVertexAttribArray(NormalMappedSpriteLayer::spriteProgramAttribColorCoord);
    glEnableVertexAttribArray(NormalMappedSpriteLayer::spriteProgramAttribNormalCoord);

    glUniformMatrix4fv(NormalMappedSpriteLayer::spriteProgramUniformCameraMatrix, 1, GL_FALSE,
                       (GLfloat*)glm::value_ptr(mvp.multipliedMatrix));
  }

  void NormalMappedSpriteLayer::endDraw() {
    glDisableVertexAttribArray(NormalMappedSpriteLayer::spriteProgramAttribPosition);
    glDisableVertexAttribArray(NormalMappedSpriteLayer::spriteProgramAttribNormal);
    glDisableVertexAttribArray(NormalMappedSpriteLayer::spriteProgramAttribColor);
    glDisableVertexAttribArray(NormalMappedSpriteLayer::spriteProgramAttribColorCoord);
    glDisableVertexAttribArray(NormalMappedSpriteLayer::spriteProgramAttribNormalCoord);
  }

  void NormalMappedSpriteLayer::bufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color) {
    //emscripten_log(EM_LOG_ERROR,"BUFFER SPRITE %s textureId=%d\n",sprite->name.c_str(),sprite->textureId);
    //sprite->lastUseTime = engine->currentTime;
    if(sprite->unloaded) engine->reloadSprite(sprite);
    if(sprite->textureId == -1) return;
    NormalMappedSpriteBuffer* buf = getBuffer(sprite->textureId);
    buf->bufferSprite(sprite, mat, color);
  }
  void NormalMappedSpriteLayer::rawBufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color) {
    if(sprite->unloaded) engine->reloadSprite(sprite);
    if(sprite->textureId == -1) return;
    NormalMappedSpriteBuffer* buf = getBuffer(sprite->textureId);
    buf->rawBufferSprite(sprite, mat, color);
  }

  void NormalMappedSpriteLayer::reset() {
    for(auto it = buffers.begin(); it!=buffers.end(); ++it) {
      NormalMappedSpriteBuffer *buffer = *it;
      buffer->reset();
    }
  }

  void NormalMappedSpriteLayer::upload() {
    if(!visible) return;
    for(auto it = buffers.begin(); it!=buffers.end(); ++it) {
      NormalMappedSpriteBuffer *buffer = *it;
      buffer->upload();
    }
  }

  void NormalMappedSpriteLayer::render(const fgfx::ModelViewProjection& mvp) {
    if(!visible) return;
    beginDraw(mvp);
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_CULL_FACE);
    for(auto it = buffers.begin(); it!=buffers.end(); ++it) {
      NormalMappedSpriteBuffer* buffer=*it;
      glBindTexture(GL_TEXTURE_2D, buffer->textureId);
      glUniform1i(NormalMappedSpriteLayer::spriteProgramUniformSampler, 0);
      buffer->render();
    }
    endDraw();
  }


}
