#ifndef PBRSpriteLayer_H
#define PBRSpriteLayer_H

#include <fastgfx.h>
#include "PBRSpriteBuffer.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>

#include <string>

namespace fgfx {

  class PBRSpriteLayer : public fgfx::TypedLayer<PBRSpriteLayer> {
  protected:
    std::vector<PBRSpriteBuffer*> buffers;

    friend class PBRSpriteBuffer;

    static GLuint spriteProgram;
    static GLuint spriteProgramAttribPosition;
    static GLuint spriteProgramAttribNormal;
    static GLuint spriteProgramAttribColor;
    static GLuint spriteProgramAttribColorCoord;
    static GLuint spriteProgramAttribNormalCoord;
    static GLuint spriteProgramAttribMaterialCoord;
    static GLuint spriteProgramUniformCameraMatrix;
    static GLuint spriteProgramUniformSampler;

  public:
    static void initializeSpriteProgram();
    static void destroySpriteProgram();
    PBRSpriteLayer(fgfx::Engine* enginep);

    void bufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color);
    void rawBufferSprite(const std::shared_ptr<Sprite>& sprite, const glm::mat4 &mat, glm::vec4 color);

    virtual void reset() override;
    virtual void upload() override;
    virtual void render(const fgfx::ModelViewProjection& mvp) override;

    void beginDraw(const fgfx::ModelViewProjection& mvp);
    void endDraw();

    PBRSpriteBuffer* getBuffer(int textureId);
    PBRSpriteBuffer* getBuffer(std::shared_ptr<Sprite> sprite);
  };

}
#endif // PBRSpriteLayer_H
