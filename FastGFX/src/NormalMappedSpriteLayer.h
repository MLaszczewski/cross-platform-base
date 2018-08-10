#ifndef NormalMappedSpriteLayer_H
#define NormalMappedSpriteLayer_H

#include "config.h"
#include "NormalMappedSpriteBuffer.h"
#include "Layer.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>

#include <string>

namespace fgfx {

  class NormalMappedSpriteLayer : public Layer {
  protected:
    std::vector<NormalMappedSpriteBuffer*> buffers;
    NormalMappedSpriteBuffer* getBuffer(int textureId);

    friend class NormalMappedSpriteBuffer;

    static GLuint spriteProgram;
    static GLuint spriteProgramAttribPosition;
    static GLuint spriteProgramAttribNormal;
    static GLuint spriteProgramAttribColor;
    static GLuint spriteProgramAttribColorCoord;
    static GLuint spriteProgramAttribNormalCoord;
    static GLuint spriteProgramUniformCameraMatrix;
    static GLuint spriteProgramUniformSampler;

  public:
    static void initializeSpriteProgram();
    static void destroySpriteProgram();
    NormalMappedSpriteLayer(fgfx::Engine* enginep);

    void bufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color);
    void rawBufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color);

    virtual void reset() override;
    virtual void upload() override;
    virtual void render(const fgfx::ModelViewProjection& mvp) override;

    void beginDraw(const fgfx::ModelViewProjection& mvp);
    void endDraw();
  };

}
#endif // NormalMappedSpriteLayer_H
