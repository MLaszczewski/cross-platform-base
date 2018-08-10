//
// Created by Michał Łaszczewski on 20/04/16.
//

#ifndef FGFX_PBRPOLYGONLAYER_H
#define FGFX_PBRPOLYGONLAYER_H

#include <fastgfx.h>
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <string>

namespace fgfx {

  class PBRPolygonBuffer  {
  public:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec4> colors;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec4> materials;
    int polygonPointsCount;

    GLuint positionBuffer;
    GLuint colorBuffer;
    GLuint normalBuffer;
    GLuint materialBuffer;

  public:
    std::shared_ptr<Shader> shader;
    bool rendered;

    static void initializePolygonProgram();

    PBRPolygonBuffer(std::shared_ptr<Shader> shaderp);

    ~PBRPolygonBuffer();

    void reserve(int pointCount);

    void reserveMore(int pointCount);

    inline void bufferPoint(glm::vec3 pos, glm::vec4 color, glm::vec3 normal, glm::vec4 material) {
      vertices.push_back(pos);
      colors.push_back(color);
      normals.push_back(normal);
      materials.push_back(material);

      polygonPointsCount++;
    }

    void bufferTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec4 color, glm::vec4 material);

    void bufferTriangle(glm::vec3 p[3], glm::vec4 color, glm::vec4 material);

    void bufferQuad(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec4 color, glm::vec4 material);

    void bufferQuad(glm::vec3 p[4], glm::vec4 color, glm::vec4 material);

    void
    bufferTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec4 color, glm::vec3 normal, glm::vec4 material);

    void bufferTriangle(glm::vec3 p[3], glm::vec4 color, glm::vec3 normal, glm::vec4 material);

    void bufferQuad(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec4 color, glm::vec3 normal,
                    glm::vec4 material);

    void bufferQuad(glm::vec3 p[4], glm::vec4 color, glm::vec3 normal, glm::vec4 material);

    void reset();
    void upload();
    void render(ShaderBinding* binding);

    static void renderLayer(BuffersLayer<PBRPolygonBuffer>* layer, const ModelViewProjection& mvp);

    static void beginDraw(Shader* shaderId, ShaderBinding* binding, const ModelViewProjection& mvp);
    static void endDraw(ShaderBinding* binding);

    static ShaderBindingRequest shaderBindingRequest;

  };


}


#endif //ENGINE_POLYGONLAYER_H
