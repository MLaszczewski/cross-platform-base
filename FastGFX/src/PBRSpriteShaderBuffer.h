//
// Created by Michał Łaszczewski on 09/05/17.
//

#ifndef FGFX_PBRSPRITESHADERBUFFER_H
#define FGFX_PBRSPRITESHADERBUFFER_H

#include "BuffersLayer.h"
#include "Sprite.h"
#include "Shader.h"

namespace fgfx {

  class PBRSpriteShaderBuffer {
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
  public:
    GLuint textureId;
    std::shared_ptr<Shader> shader;
    bool rendered;

    PBRSpriteShaderBuffer(GLuint textureIdp, std::shared_ptr<Shader> shader);
    ~PBRSpriteShaderBuffer();

    void reserve(int pointCount);

    void bufferPoint(glm::vec2 pos, glm::vec2 normal, glm::vec4 color, glm::vec2 colorCoord, glm::vec2 normalCoord, glm::vec2 materialCoord);
    void bufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color);
    void rawBufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color);

    void reset();
    void upload();
    void render(ShaderBinding* binding);

    static void renderLayer(BuffersLayer<PBRSpriteShaderBuffer>* layer, const ModelViewProjection& mvp);

    static void beginDraw(GLuint textureId, Shader* shaderId, ShaderBinding* binding, const ModelViewProjection& mvp);
    static void endDraw(ShaderBinding* binding);

    static ShaderBindingRequest shaderBindingRequest;

  };

}


#endif //FGFX_PBRSPRITESHADERBUFFER_H
