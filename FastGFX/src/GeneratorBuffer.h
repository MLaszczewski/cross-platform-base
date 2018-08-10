//
// Created by Michał Łaszczewski on 14/04/16.
//

#ifndef FGFX_GeneratorBUFFER_H
#define FGFX_GeneratorBUFFER_H

#include <fastgfx.h>

namespace fgfx {
      
  class GeneratorBuffer {
  protected:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    std::vector<glm::vec2> coords;
    std::vector<glm::vec4> generatorFactors;
    std::vector<glm::vec4> outputFactors;

  public:
    int texturedPointsCount;

    GLuint positionBuffer;
    GLuint normalBuffer;
    GLuint tangentBuffer;
    GLuint bitangentBuffer;
    GLuint coordBuffer;
    GLuint generatorFactorBuffer;
    GLuint outputFactorBuffer;

  public:
    std::shared_ptr<Shader> shader;
    bool rendered;

    GeneratorBuffer(const std::shared_ptr<Shader>& shaderp);
    ~GeneratorBuffer();

    void reserve(int pointCount);
    void bufferPoint(glm::vec3 pos, glm::vec2 coord, glm::vec4 generatorFactor, glm::vec4 outputFactor);
    void reset();
    void computeNormals();
    void computeTangents();
    void upload();
    void render(ShaderBinding* binding);

    static void renderLayer(BuffersLayer<GeneratorBuffer>* layer, const ModelViewProjection& mvp);

    static void beginDraw(Shader* shaderId, ShaderBinding* binding, const ModelViewProjection& mvp);
    static void endDraw(ShaderBinding* binding);

    static ShaderBindingRequest shaderBindingRequest;
  };

}


#endif //FGFX_TEXTUREDBUFFER_H
