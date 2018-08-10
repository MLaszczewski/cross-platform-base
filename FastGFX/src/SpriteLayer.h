#ifndef SPRITELAYER_H
#define SPRITELAYER_H

#include "Sprite.h"
#include "SpriteBuffer.h"
#include "Layer.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>

#include <string>

#include "shaders.h"

namespace fgfx {

  class Engine;

  class SpriteLayer : public TypedLayer<SpriteLayer>{
  public:
    std::vector<SpriteBuffer*> buffers;
    SpriteBuffer* getBuffer(int textureId);

  protected:

    friend class Engine;
    friend class SpriteBuffer;

    static GLuint spriteProgram;
    static GLuint spriteProgramAttribPosition;
    static GLuint spriteProgramAttribColor;
    static GLuint spriteProgramAttribCoord;
    static GLuint spriteProgramUniformCameraMatrix;
    static GLuint spriteProgramUniformSampler;

    static void initializeSpriteProgram();
  public:
    SpriteLayer(Engine* enginep);

    void bufferSprite(const std::shared_ptr<Sprite>& sprite, glm::vec2 pos, glm::vec4 color, float size, float rotation);
    void bufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color);
    void rawBufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color);

    virtual void reset() override ;
    virtual void upload() override ;
    virtual void render(const ModelViewProjection& mvp) override;

    void beginDraw(const ModelViewProjection& mvp);
    void endDraw();

    static void destroySpriteProgram();
  };

}
#endif // SPRITELAYER_H
