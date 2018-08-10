#include "PBRSpriteLayer.h"
#include <fastgfx.h>

#include <glm/gtc/type_ptr.hpp>
#include <Fox2D/utils/log.h>

namespace fgfx {

  PBRSpriteLayer::PBRSpriteLayer(fgfx::Engine* enginep) : TypedLayer<PBRSpriteLayer>(enginep) {
    composition = 1;
  };

  PBRSpriteBuffer* PBRSpriteLayer::getBuffer(int textureId) {
    for(auto it=buffers.begin(); it!=buffers.end(); ++it) {
      if((*it)->textureId == textureId) return *it;
    }
    PBRSpriteBuffer* buf = new PBRSpriteBuffer(textureId);
    buffers.push_back(buf);
    return buf;
  }

  PBRSpriteBuffer* PBRSpriteLayer::getBuffer(std::shared_ptr<Sprite> sprite) {
    if(sprite->unloaded) engine->reloadSprite(sprite);
    if(sprite->textureId == -1) return nullptr;
    return getBuffer(sprite->textureId);
  }

  GLuint PBRSpriteLayer::spriteProgram = 0;
  GLuint PBRSpriteLayer::spriteProgramAttribColor = 0;
  GLuint PBRSpriteLayer::spriteProgramAttribPosition = 0;
  GLuint PBRSpriteLayer::spriteProgramAttribNormal = 0;
  GLuint PBRSpriteLayer::spriteProgramAttribColorCoord = 0;
  GLuint PBRSpriteLayer::spriteProgramAttribNormalCoord = 0;
  GLuint PBRSpriteLayer::spriteProgramAttribMaterialCoord = 0;
  GLuint PBRSpriteLayer::spriteProgramUniformSampler = 0;
  GLuint PBRSpriteLayer::spriteProgramUniformCameraMatrix = 0;

  void PBRSpriteLayer::initializeSpriteProgram() {

    const char vShaderStr[] = R"GLSL(
      attribute vec2 aVertexPosition;
      attribute vec2 aVertexNormal;
      attribute vec4 aVertexColor;
      attribute vec2 aColorCoord;
      attribute vec2 aNormalCoord;
      attribute vec2 aMaterialCoord;
      uniform mat4 uCameraMatrix;
      varying vec2 vNormal;
      varying vec4 vColor;
      varying highp vec2 vColorCoord;
      varying highp vec2 vNormalCoord;
      varying highp vec2 vMaterialCoord;
      void main() {
        gl_Position = uCameraMatrix * vec4(aVertexPosition, 0.0, 1.0);
        vNormal = aVertexNormal;
        vColor = aVertexColor;
        vColorCoord = aColorCoord;
        vNormalCoord = aNormalCoord;
        vMaterialCoord = aMaterialCoord;
      }
    )GLSL";

    const char fShaderStr[] = R"GLSL(
      precision mediump float;
      varying vec2 vNormal;
      varying vec4 vColor;
      uniform sampler2D uSampler;
      varying highp vec2 vColorCoord;
      varying highp vec2 vNormalCoord;
      varying highp vec2 vMaterialCoord;

      vec3 environmentLight(vec3 dir, float roughness) {
        vec3 sunDir = normalize(vec3(1.0, 5.0, 4.0));
        vec3 sunColor = vec3(1.0, 0.9, 0.8) * 5.0;
        vec3 sunHaloColor = vec3(1.0, 0.8, 0.5) * 0.023;
        float sunSize = 0.046;
        float sunBorder = 0.05;
        vec3 skyColor = vec3(0.027, 0.571, 0.847) * ( dir.y * 0.5 + 0.5);
        vec3 groundColor = vec3(0.29, 0.23, 0.19) * (-dir.y * 0.7 + 0.3);
        float groundHeight = 0.0;

        float sunDistance = length(dir - sunDir);

        vec3 sunBeam = sunColor * (clamp((sunSize - sunDistance) / (sunBorder + roughness), -1.0, 1.0) * 0.5 + 0.5);
        vec3 sunHalo = sunHaloColor * (1.0 / (sunDistance + roughness));
        skyColor+= sunBeam + sunHalo;

        return mix(groundColor, skyColor, clamp(( dir.y + groundHeight) / roughness, -1.0, 1.0) * 0.5 + 0.5  );
      }

      void main() {
        vec4 textureColor = texture2D(uSampler, vColorCoord);
        vec4 color = textureColor * vColor;
        vec4 textureNormalPixel = texture2D(uSampler, vNormalCoord);
        vec4 textureMaterialPixel = texture2D(uSampler, vMaterialCoord);
        vec3 normal = textureNormalPixel.xyz * 2.0 - vec3(1.0, 1.0, 1.0);
        vec3 normalYAxis = vec3(vNormal.x, vNormal.y, 0);
        vec3 normalXAxis = vec3(normalYAxis.y, -normalYAxis.x, 0);
        normal = - normalXAxis * normal.x - normalYAxis * normal.y - vec3(0, 0, normal.z);
        normal.z = normal.z * -1.0;
        vec3 eyeDir = normalize(vec3(0, 0, 1));
        vec3 reflection = normalize(reflect(eyeDir, normal));
        float roughness = textureMaterialPixel.r;
        float metallic = textureMaterialPixel.g;
        float occulsion = textureMaterialPixel.b;
        vec3 diffuse = color.rgb * (1.0 - metallic); // metals have no diffuse
        vec3 specular = mix(vec3(0.04), color.rgb, metallic); // fixed spec for non-metals
        vec3 envDiffuseColor = diffuse * environmentLight(normal, 2.0);
        vec3 envSpecularColor = specular * environmentLight(reflection, roughness);
        vec3 ambientLight = diffuse * vec3(1.0, 1.0, 1.0) * 0.2;
        gl_FragColor = vec4( occulsion * (envDiffuseColor + envSpecularColor), color.a * textureMaterialPixel.a);
      }
    )GLSL";

    auto vertexShader = fgfx::loadShader ( GL_VERTEX_SHADER, vShaderStr );
    auto fragmentShader = fgfx::loadShader ( GL_FRAGMENT_SHADER, fShaderStr );

    auto spriteProgram = fgfx::createProgram (vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if(!spriteProgram) throw "nie ma programu!";
    PBRSpriteLayer::spriteProgram = spriteProgram;

    PBRSpriteLayer::spriteProgramAttribPosition = glGetAttribLocation(spriteProgram, "aVertexPosition");
    PBRSpriteLayer::spriteProgramAttribNormal = glGetAttribLocation(spriteProgram, "aVertexNormal");
    PBRSpriteLayer::spriteProgramAttribColor = glGetAttribLocation(spriteProgram, "aVertexColor");
    PBRSpriteLayer::spriteProgramAttribColorCoord = glGetAttribLocation(spriteProgram, "aColorCoord");
    PBRSpriteLayer::spriteProgramAttribNormalCoord = glGetAttribLocation(spriteProgram, "aNormalCoord");
    PBRSpriteLayer::spriteProgramAttribMaterialCoord = glGetAttribLocation(spriteProgram, "aMaterialCoord");

    PBRSpriteLayer::spriteProgramUniformCameraMatrix = glGetUniformLocation(spriteProgram, "uCameraMatrix");
    PBRSpriteLayer::spriteProgramUniformSampler = glGetUniformLocation(spriteProgram, "uSampler");
  }

  void PBRSpriteLayer::destroySpriteProgram() {
    glDeleteProgram(spriteProgram);
  }

  void PBRSpriteLayer::beginDraw(const fgfx::ModelViewProjection& mvp) {
    glUseProgram(PBRSpriteLayer::spriteProgram);
    glEnableVertexAttribArray(PBRSpriteLayer::spriteProgramAttribPosition);
    glEnableVertexAttribArray(PBRSpriteLayer::spriteProgramAttribColor);
    glEnableVertexAttribArray(PBRSpriteLayer::spriteProgramAttribNormal);
    glEnableVertexAttribArray(PBRSpriteLayer::spriteProgramAttribColorCoord);
    glEnableVertexAttribArray(PBRSpriteLayer::spriteProgramAttribNormalCoord);
    glEnableVertexAttribArray(PBRSpriteLayer::spriteProgramAttribMaterialCoord);

    glUniformMatrix4fv(PBRSpriteLayer::spriteProgramUniformCameraMatrix, 1, GL_FALSE,
                       (GLfloat*)glm::value_ptr(mvp.multipliedMatrix));
  }

  void PBRSpriteLayer::endDraw() {
    glDisableVertexAttribArray(PBRSpriteLayer::spriteProgramAttribPosition);
    glDisableVertexAttribArray(PBRSpriteLayer::spriteProgramAttribNormal);
    glDisableVertexAttribArray(PBRSpriteLayer::spriteProgramAttribColor);
    glDisableVertexAttribArray(PBRSpriteLayer::spriteProgramAttribColorCoord);
    glDisableVertexAttribArray(PBRSpriteLayer::spriteProgramAttribNormalCoord);
    glDisableVertexAttribArray(PBRSpriteLayer::spriteProgramAttribMaterialCoord);
  }

  void PBRSpriteLayer::bufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color) {
    //emscripten_log(EM_LOG_ERROR,"BUFFER SPRITE %s textureId=%d\n",sprite->name.c_str(),sprite->textureId);
    //sprite->lastUseTime = engine->currentTime;
    if(sprite->unloaded) engine->reloadSprite(sprite);
    if(sprite->textureId == -1) return;
    PBRSpriteBuffer* buf = getBuffer(sprite->textureId);
    buf->bufferSprite(sprite, mat, color);
  }
  void PBRSpriteLayer::rawBufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color) {
    if(sprite->unloaded) engine->reloadSprite(sprite);
    if(sprite->textureId == -1) return;
    PBRSpriteBuffer* buf = getBuffer(sprite->textureId);
    buf->rawBufferSprite(sprite, mat, color);
  }

  void PBRSpriteLayer::reset() {
    for(auto it = buffers.begin(); it != buffers.end(); ++it) {
      PBRSpriteBuffer *buffer = *it;
      buffer->reset();
    }
  }

  void PBRSpriteLayer::upload() {
    if(!visible) return;
    for(auto it = buffers.begin(); it != buffers.end(); ++it) {
      PBRSpriteBuffer *buffer = *it;
      buffer->upload();
    }
  }

  void PBRSpriteLayer::render(const fgfx::ModelViewProjection& mvp) {
    if(!visible) return;
    beginDraw(mvp);
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_CULL_FACE);
    for(auto it = buffers.begin(); it != buffers.end(); ++it) {
      PBRSpriteBuffer* buffer = *it;
      glBindTexture(GL_TEXTURE_2D, buffer->textureId);
      glUniform1i(PBRSpriteLayer::spriteProgramUniformSampler, 0);
      buffer->render();
    }
    endDraw();
  }


}
