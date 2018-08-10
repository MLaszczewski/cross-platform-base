//
// Created by Michał Łaszczewski on 20/04/16.
//

#ifndef ENGINE_VECTORLAYER_H
#define ENGINE_VECTORLAYER_H

#include "Layer.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <string>
#include "shaders.h"

namespace fgfx {

  class LineLayer : public TypedLayer<LineLayer> {
  protected:
    std::vector<glm::vec3> lineVertices;
    std::vector<glm::vec4> lineColors;
    int linePointsCount;

    GLuint positionBuffer;
    GLuint colorBuffer;

    friend class Engine;

    static GLuint lineProgram;
    static GLuint lineProgramAttribPosition;
    static GLuint lineProgramAttribColor;
    static GLuint lineProgramUniformCameraMatrix;

    static void initializeLineProgram();

  public:
    int composition;
    bool wireframe;
    bool uploaded;

    LineLayer(Engine* enginep);
    ~LineLayer();

    void reserve(int pointCount);
    void bufferPoint(glm::vec3 pos, glm::vec4 color);
    void bufferLine(glm::vec3 p1, glm::vec3 p2, glm::vec4 color);

    virtual void reset() override;
    virtual void upload() override;
    virtual void render(const ModelViewProjection& mvp) override;

    void beginDraw(const ModelViewProjection& mvp);
    void endDraw();

    static void destroyLineProgram();
  };

}


#endif //ENGINE_VECTORLAYER_H
