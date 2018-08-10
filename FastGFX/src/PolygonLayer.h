//
// Created by Michał Łaszczewski on 20/04/16.
//

#ifndef ENGINE_POLYGONLAYER_H
#define ENGINE_POLYGONLAYER_H

#include "Layer.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <string>
#include "shaders.h"

namespace fgfx {

  class PolygonLayer : public TypedLayer<PolygonLayer>{
  public:
    std::vector<glm::vec3> polygonVertices;
    std::vector<glm::vec4> polygonColors;
    int polygonPointsCount;

    GLuint positionBuffer;
    GLuint colorBuffer;

  protected:
    friend class Engine;

    static GLuint polygonProgram;
    static GLuint polygonProgramAttribPosition;
    static GLuint polygonProgramAttribColor;
    static GLuint polygonProgramUniformCameraMatrix;

    static void initializePolygonProgram();

  public:
    int composition;
    bool wireframe;
    bool uploaded;

    PolygonLayer(Engine* enginep);
    ~PolygonLayer();

    void reserve(int pointCount);
    void reserveMore(int pointCount);

    inline void bufferPoint(glm::vec3 pos, glm::vec4 color) {
      if(frozen && uploaded) return;
      /*emscripten_log(EM_LOG_ERROR,"BUFFER POLYGON POINT %d (%f %f %f) COLOR (%f %f %f %f)\n",polygonPointsCount,
                     pos.x,pos.y,pos.z,color.r,color.g,color.b,color.a);*/
      polygonVertices.push_back(pos);
      polygonColors.push_back(color);
      polygonPointsCount++;
    }
    void bufferTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec4 color);
    void bufferTriangle(glm::vec3 p[3], glm::vec4 color);
    void bufferQuad(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec4 color);
    void bufferQuad(glm::vec3 p[4], glm::vec4 color);

    /// TODO: Buffer polygon, buffer line(with weight)

    virtual void reset() override;
    virtual void upload() override;
    virtual void render(const ModelViewProjection& mvp) override;

    static void beginDraw(const ModelViewProjection& mvp);
    static void endDraw();
    void draw();

    static void destroyPolygonProgram();
  };

}


#endif //ENGINE_POLYGONLAYER_H
