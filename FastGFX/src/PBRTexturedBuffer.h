//
// Created by Michał Łaszczewski on 14/04/16.
//

#ifndef FGFX_PBRTEXTUREDBUFFER_H
#define FGFX_PBRTEXTUREDBUFFER_H

#include <fastgfx.h>

namespace fgfx {
      
  class PBRTexturedBuffer {
  protected:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec4> colors;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    std::vector<glm::vec2> colorCoords;
    std::vector<glm::vec2> normalCoords;
    std::vector<glm::vec2> materialCoords;

  public:
    int texturedPointsCount;

    GLuint positionBuffer;
    GLuint colorBuffer;
    GLuint normalBuffer;
    GLuint tangentBuffer;
    GLuint bitangentBuffer;
    GLuint colorCoordBuffer;
    GLuint normalCoordBuffer;
    GLuint materialCoordBuffer;
  private:

    friend class TexturedLayer;
  public:
    GLuint textureId;
    std::shared_ptr<Shader> shader;
    bool rendered;

    PBRTexturedBuffer(int textureIdp);
    ~PBRTexturedBuffer();

    void reserve(int pointCount);
    void bufferPoint(glm::vec3 pos, glm::vec4 color,
                     glm::vec2 colorCoord, glm::vec2 normalCoord, glm::vec2 materialCoord);
    void reset();
    void computeNormals();
    void computeTangents();
    void upload();
    void render(ShaderBinding* binding);

    static void renderLayer(BuffersLayer<PBRTexturedBuffer>* layer, const ModelViewProjection& mvp);

    static void beginDraw(GLuint textureId, Shader* shaderId, ShaderBinding* binding, const ModelViewProjection& mvp);
    static void endDraw(ShaderBinding* binding);

    static ShaderBindingRequest shaderBindingRequest;
  };

}


#endif //FGFX_TEXTUREDBUFFER_H
