#ifndef TEXTUREDLAYER_H
#define TEXTUREDLAYER_H

#include "Texture.h"
#include "TexturedBuffer.h"
#include "Layer.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <string>
#include <memory>

#include "shaders.h"

namespace fgfx {

  class Engine;

  class TexturedLayer : public TypedLayer<TexturedLayer> {
  protected:
    std::vector<TexturedBuffer*> buffers;
    TexturedBuffer* getBuffer(int textureId);

    friend class Engine;
    friend class TexturedBuffer;

    static GLuint texturedProgram;
    static GLuint texturedProgramAttribPosition;
    static GLuint texturedProgramAttribColor;
    static GLuint texturedProgramAttribCoord;
    static GLuint texturedProgramUniformCameraMatrix;
    static GLuint texturedProgramUniformSampler;

    static void initializeTexturedProgram();
  public:
    TexturedLayer(Engine* enginep);

    void bufferTexturedPoint(const std::shared_ptr<Texture>& texture, glm::vec2 point, glm::vec2 coord, glm::vec4 color);
    void bufferTexturedPoint(const std::shared_ptr<Texture>& texture, glm::vec3 point, glm::vec2 coord, glm::vec4 color);
    TexturedBuffer* getBuffer(const std::shared_ptr<Texture>& texture);

    virtual void reset() override ;
    virtual void upload() override ;
    virtual void render(const ModelViewProjection& mvp) override ;

    void beginDraw(const ModelViewProjection& mvp);
    void endDraw();

    static void destroyTexturedProgram();
  };

}
#endif // TEXTUREDLAYER_H
