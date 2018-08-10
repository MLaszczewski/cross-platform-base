//
// Created by Michał Łaszczewski on 20/04/16.
//

#include "PBRPolygonBuffer.h"

#include <fastgfx.h>
#include "Shader.h"

namespace fgfx {

  ShaderBindingRequest PBRPolygonBuffer::shaderBindingRequest =  {
      .attribNames = { "aPosition", "aColor", "aNormal", "aMaterial" },
      .uniformNames = { "uProjectionMatrix", "uViewMatrix", "uModelMatrix", "uModelMatrix3", "uEyePosition" }
    };

  PBRPolygonBuffer::PBRPolygonBuffer(std::shared_ptr<Shader> shaderp) : polygonPointsCount(0), shader(shaderp) {
    glGenBuffers(1, &positionBuffer);
    glGenBuffers(1, &colorBuffer);
    glGenBuffers(1, &normalBuffer);
    glGenBuffers(1, &materialBuffer);
  }

  PBRPolygonBuffer::~PBRPolygonBuffer() {
    glDeleteBuffers(1, &positionBuffer);
    glDeleteBuffers(1, &colorBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteBuffers(1, &materialBuffer);
  }

  void PBRPolygonBuffer::reserve(int pointCount) {
    vertices.reserve(pointCount);
    colors.reserve(pointCount);
    normals.reserve(pointCount);
    materials.reserve(pointCount);
  }

  void PBRPolygonBuffer::reserveMore(int pointCount) {
    vertices.reserve(pointCount + polygonPointsCount);
    colors.reserve(pointCount + polygonPointsCount);
    normals.reserve(pointCount + polygonPointsCount);
    materials.reserve(pointCount + polygonPointsCount);
  }

  void PBRPolygonBuffer::bufferTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec4 color, glm::vec3 normal,
                                       glm::vec4 material) {
    bufferPoint(p1, color, normal, material);
    bufferPoint(p2, color, normal, material);
    bufferPoint(p3, color, normal, material);
  }

  void PBRPolygonBuffer::bufferTriangle(glm::vec3 p[3], glm::vec4 color, glm::vec3 normal, glm::vec4 material) {
    bufferPoint(p[0], color, normal, material);
    bufferPoint(p[1], color, normal, material);
    bufferPoint(p[2], color, normal, material);
  }

  void PBRPolygonBuffer::bufferQuad(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec4 color,
                                   glm::vec3 normal, glm::vec4 material) {
    bufferPoint(p1, color, normal, material);
    bufferPoint(p2, color, normal, material);
    bufferPoint(p3, color, normal, material);
    bufferPoint(p1, color, normal, material);
    bufferPoint(p3, color, normal, material);
    bufferPoint(p4, color, normal, material);
  }

  void PBRPolygonBuffer::bufferQuad(glm::vec3 p[4], glm::vec4 color, glm::vec3 normal, glm::vec4 material) {
    bufferPoint(p[0], color, normal, material);
    bufferPoint(p[1], color, normal, material);
    bufferPoint(p[2], color, normal, material);
    bufferPoint(p[0], color, normal, material);
    bufferPoint(p[2], color, normal, material);
    bufferPoint(p[3], color, normal, material);
  }



  void PBRPolygonBuffer::bufferTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec4 color, glm::vec4 material) {
    glm::vec3 u = p2 - p1;
    glm::vec3 v = p3 - p1;
    glm::vec3 normal = glm::normalize(glm::cross(u,v));
    bufferTriangle(p1, p2, p3, color, normal, material);
  }

  void PBRPolygonBuffer::bufferTriangle(glm::vec3 p[3], glm::vec4 color, glm::vec4 material) {
    glm::vec3 u = p[1] - p[0];
    glm::vec3 v = p[2] - p[0];
    glm::vec3 normal = glm::normalize( glm::cross(u,v) );
    bufferTriangle(p, color, normal, material);
  }

  void PBRPolygonBuffer::bufferQuad(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec4 color,
                                   glm::vec4 material) {
    glm::vec3 u = p2 - p1;
    glm::vec3 v = p3 - p1;
    glm::vec3 normal = glm::normalize( glm::cross(u,v) );
    bufferQuad(p1, p2, p3, p4, color, normal, material);
  }

  void PBRPolygonBuffer::bufferQuad(glm::vec3 p[4], glm::vec4 color, glm::vec4 material) {
    glm::vec3 u = p[2] - p[0];
    glm::vec3 v = p[1] - p[0];
    glm::vec3 normal = glm::normalize( glm::cross(u,v) );
    bufferQuad(p, color, normal, material);
  }

  void PBRPolygonBuffer::reset() {
    vertices.clear();
    colors.clear();
    normals.clear();
    materials.clear();
    polygonPointsCount = 0;
  }

  void PBRPolygonBuffer::upload() {
    if (!polygonPointsCount) return;

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, polygonPointsCount * sizeof(glm::vec3), vertices.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, polygonPointsCount * sizeof(glm::vec4), colors.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, polygonPointsCount * sizeof(glm::vec3), normals.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, materialBuffer);
    glBufferData(GL_ARRAY_BUFFER, polygonPointsCount * sizeof(glm::vec4), materials.data(),
                 GL_DYNAMIC_DRAW);

  }

  void PBRPolygonBuffer::render(ShaderBinding* binding) {
    if(!polygonPointsCount) return;

    //fgfx_log("REDNER PBR POLYGON SHADER");

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glVertexAttribPointer(binding->attribLocations[0], 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glVertexAttribPointer(binding->attribLocations[1], 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer(binding->attribLocations[2], 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, materialBuffer);
    glVertexAttribPointer(binding->attribLocations[3], 4, GL_FLOAT, GL_FALSE, 0, 0);
    fgfx::checkGLError();

    glDrawArrays(GL_TRIANGLES, 0, polygonPointsCount);
    fgfx::checkGLError();
  }

  void PBRPolygonBuffer::renderLayer(BuffersLayer<PBRPolygonBuffer>* layer, const ModelViewProjection& mvp) {
    std::vector<PBRPolygonBuffer*>& buffers = layer->buffers;
    int l = buffers.size();
    for(int i = 0; i < l; i++) buffers[i]->rendered = false;
    int first = 0;
    while(first < l) {
      if(buffers[first]->rendered) {
        first ++;
        continue;
      }
      auto shader = buffers[first]->shader;

      ShaderBinding* binding = shader->requestBinding(&PBRPolygonBuffer::shaderBindingRequest);

      PBRPolygonBuffer::beginDraw(shader.get(), binding, mvp);
      fgfx::checkGLError();

      if(shader->loaded) buffers[first]->render(binding);
      buffers[first]->rendered = true;

      for(int i = first + 1; i < l; i++) {
        if(buffers[i]->shader == shader) {
          if(shader->loaded) buffers[i]->render(binding);
          buffers[i]->rendered = true;
        }
      }

      PBRPolygonBuffer::endDraw(binding);
      first ++;
    }
  }

  void PBRPolygonBuffer::beginDraw(Shader* shader, ShaderBinding* binding,
                                  const ModelViewProjection& mvp) {
    glUseProgram(shader->program);

    for(int i = 0; i < 4; i++) glEnableVertexAttribArray(binding->attribLocations[i]);
    fgfx::checkGLError();

    //for(int i = 0; i <= 5; i++) fgfx_log("BINDING ATTRIB %s #%d", shaderBindingRequest.attribNames[i].c_str(), binding->attribLocations[i]);
    //for(int i = 0; i <= 1; i++) fgfx_log("BINDING UNIFORM %s #%d", shaderBindingRequest.uniformNames[i].c_str(), binding->uniformLocations[i]);

    glUniformMatrix4fv(binding->uniformLocations[0], 1, GL_FALSE,
                       (GLfloat*)glm::value_ptr(mvp.projectionMatrix));
    glUniformMatrix4fv(binding->uniformLocations[1], 1, GL_FALSE,
                       (GLfloat*)glm::value_ptr(mvp.viewMatrix));
    glUniformMatrix4fv(binding->uniformLocations[2], 1, GL_FALSE,
                       (GLfloat*)glm::value_ptr(mvp.modelMatrix));
    glm::mat3 modelMatrix3(mvp.modelMatrix);
    glUniformMatrix3fv(binding->uniformLocations[3], 1, GL_FALSE,
                       (GLfloat*)glm::value_ptr(modelMatrix3));
    glUniform3fv(binding->uniformLocations[4], 1, (GLfloat*)glm::value_ptr(mvp.eyePosition));

    fgfx::checkGLError();
  }
  void PBRPolygonBuffer::endDraw(ShaderBinding* binding) {
    for(int i = 0; i < 4; i++) glDisableVertexAttribArray(binding->attribLocations[i]);
  }

}