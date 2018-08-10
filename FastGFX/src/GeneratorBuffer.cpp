//
// Created by Michał Łaszczewski on 14/04/16.
//

#include <fastgfx.h>

#include "GeneratorBuffer.h"

namespace fgfx {

  ShaderBindingRequest GeneratorBuffer::shaderBindingRequest = {
      .attribNames = { "aPosition", "aNormal", "aTangent", "aBitangent",
                       "aCoord", "aGeneratorFactor", "aOutputFactor" },
      .uniformNames = { "uProjectionMatrix", "uViewMatrix", "uModelMatrix", "uModelMatrix3", "uEyePosition" }
  };

  GeneratorBuffer::GeneratorBuffer(const std::shared_ptr<Shader>& shaderp) : shader(shaderp) {
    glGenBuffers(1, &positionBuffer);
    glGenBuffers(1, &normalBuffer);
    glGenBuffers(1, &tangentBuffer);
    glGenBuffers(1, &bitangentBuffer);
    glGenBuffers(1, &coordBuffer);
    glGenBuffers(1, &generatorFactorBuffer);
    glGenBuffers(1, &outputFactorBuffer);
    texturedPointsCount = 0;
  }

  GeneratorBuffer::~GeneratorBuffer() {
    glDeleteBuffers(1, &positionBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteBuffers(1, &tangentBuffer);
    glDeleteBuffers(1, &bitangentBuffer);
    glDeleteBuffers(1, &coordBuffer);
    glDeleteBuffers(1, &generatorFactorBuffer);
    glDeleteBuffers(1, &outputFactorBuffer);
  }

  void GeneratorBuffer::reserve(int pointCount) {
    vertices.reserve( pointCount );
    normals.reserve( pointCount );
    tangents.reserve( pointCount );
    bitangents.reserve( pointCount );
    coords.reserve( pointCount );
    generatorFactors.reserve( pointCount );
    outputFactors.reserve( pointCount );
  }

  void GeneratorBuffer::bufferPoint(glm::vec3 pos, glm::vec2 coord, glm::vec4 generatorFactor, glm::vec4 outputFactor) {
    vertices.push_back( pos );
    generatorFactors.push_back( generatorFactor );
    outputFactors.push_back( outputFactor );
    normals.push_back( glm::vec3(0, 0, 1) );
    tangents.push_back( glm::vec3(1, 0, 0) );
    bitangents.push_back( glm::vec3(0, 1, 0) );
    coords.push_back( coord );
    texturedPointsCount++;
  }

  void GeneratorBuffer::reset() {
    vertices.clear();
    normals.clear();
    tangents.clear();
    bitangents.clear();
    coords.clear();
    generatorFactors.clear();
    outputFactors.clear();
    texturedPointsCount = 0;
  }

  void GeneratorBuffer::computeNormals() {
    normals.resize(texturedPointsCount);
    for(int i = 0; i < texturedPointsCount; i += 3) {
      // Shortcuts for vertices
      glm::vec3 & v0 = vertices[i + 0];
      glm::vec3 & v1 = vertices[i + 1];
      glm::vec3 & v2 = vertices[i + 2];

      glm::vec3 u = v1 - v0;
      glm::vec3 v = v2 - v0;
      glm::vec3 normal = glm::normalize(glm::cross(u,v));

      normals[i + 0] = normal;
      normals[i + 1] = normal;
      normals[i + 2] = normal;
    }
  }
  void GeneratorBuffer::computeTangents() {
    tangents.resize( texturedPointsCount );
    bitangents.resize( texturedPointsCount );
    for(int i = 0; i < texturedPointsCount; i += 3) {
      // Shortcuts for vertices
      glm::vec3 & v0 = vertices[i + 0];
      glm::vec3 & v1 = vertices[i + 1];
      glm::vec3 & v2 = vertices[i + 2];

      // Shortcuts for UVs
      glm::vec2 & uv0 = coords[i + 0];
      glm::vec2 & uv1 = coords[i + 1];
      glm::vec2 & uv2 = coords[i + 2];

      // Edges of the triangle : postion delta
      glm::vec3 deltaPos1 = v1 - v0;
      glm::vec3 deltaPos2 = v2 - v0;

      // UV delta
      glm::vec2 deltaUV1 = uv1 - uv0;
      glm::vec2 deltaUV2 = uv2 - uv0;

      float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
      glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
      glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

      tangent = glm::vec3(1,0,0);
      bitangent = glm::vec3(0,1,0);

      tangents[i + 0] = tangent;
      tangents[i + 1] = tangent;
      tangents[i + 2] = tangent;

      bitangents[i + 0] = bitangent;
      bitangents[i + 1] = bitangent;
      bitangents[i + 2] = bitangent;
    }
  }

  void GeneratorBuffer::upload() {
    if (!texturedPointsCount) return;

    computeNormals();
    computeTangents();

/*
    GLfloat* points = (GLfloat*)vertices.data();
    printf("FIRST 3 VERTICES (%f %f %f) (%f %f %f) (%f %f %f)\n",
           points[0],points[1],points[2],
           points[3],points[4],points[5],
           points[6],points[7],points[8]);//*/


    /*
    GLfloat* points = (GLfloat*)normals.data();
    printf("FIRST 3 NORMALS (%f %f %f) (%f %f %f) (%f %f %f)\n",
           points[0],points[1],points[2],
           points[3],points[4],points[5],
           points[6],points[7],points[8]);//*/

    /*
    GLfloat* points = (GLfloat*)tangents.data();
    printf("FIRST 3 TANGENTS (%f %f %f) (%f %f %f) (%f %f %f)\n",
           points[0],points[1],points[2],
           points[3],points[4],points[5],
           points[6],points[7],points[8]);
    points = (GLfloat*)bitangents.data();
    printf("FIRST 3 BITANGENTS (%f %f %f) (%f %f %f) (%f %f %f)\n",
           points[0],points[1],points[2],
           points[3],points[4],points[5],
           points[6],points[7],points[8]);
    //*/

    /*
    GLfloat* points = (GLfloat*)coords.data();
    printf("FIRST 3 COORDS (%f %f) (%f %f) (%f %f)\n",
           points[0],points[1],
           points[2],points[3],
           points[4],points[5]);//*/

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, texturedPointsCount * sizeof(glm::vec3), vertices.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, texturedPointsCount * sizeof(glm::vec3), normals.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
    glBufferData(GL_ARRAY_BUFFER, texturedPointsCount * sizeof(glm::vec3), tangents.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bitangentBuffer);
    glBufferData(GL_ARRAY_BUFFER, texturedPointsCount * sizeof(glm::vec3), bitangents.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
    glBufferData(GL_ARRAY_BUFFER, texturedPointsCount * sizeof(glm::vec2), coords.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, generatorFactorBuffer);
    glBufferData(GL_ARRAY_BUFFER, texturedPointsCount * sizeof(glm::vec4), generatorFactors.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, outputFactorBuffer);
    glBufferData(GL_ARRAY_BUFFER, texturedPointsCount * sizeof(glm::vec4), outputFactors.data(),
                 GL_DYNAMIC_DRAW);

  }

  void GeneratorBuffer::render(ShaderBinding* binding) {
    //fox_log("RENDER GEN BUFFER %d",texturedPointsCount);

    if (!texturedPointsCount) return;

    /*
  .attribNames = { "aPosition", "aNormal", "aTangent", "aBitangent",
                       "aGeneratorFactor", "aOutputFactor" },
    */
    fgfx::checkGLError();

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    if(binding->attribLocations[0]!=-1) glVertexAttribPointer(binding->attribLocations[0], 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    if(binding->attribLocations[1]!=-1) glVertexAttribPointer(binding->attribLocations[1], 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
    if(binding->attribLocations[2]!=-1) glVertexAttribPointer(binding->attribLocations[2], 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, bitangentBuffer);
    if(binding->attribLocations[3]!=-1) glVertexAttribPointer(binding->attribLocations[3], 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
    if(binding->attribLocations[4]!=-1) glVertexAttribPointer(binding->attribLocations[4], 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, generatorFactorBuffer);
    if(binding->attribLocations[5]!=-1) glVertexAttribPointer(binding->attribLocations[5], 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, outputFactorBuffer);
    if(binding->attribLocations[6]!=-1) glVertexAttribPointer(binding->attribLocations[6], 4, GL_FLOAT, GL_FALSE, 0, 0);

    fgfx::checkGLError();

    glDrawArrays(GL_TRIANGLES, 0, texturedPointsCount);
    fgfx::checkGLError();
  }

  void GeneratorBuffer::renderLayer(BuffersLayer<GeneratorBuffer>* layer, const ModelViewProjection& mvp) {
    std::vector<GeneratorBuffer*>& buffers = layer->buffers;
    int l = buffers.size();
    for(int i = 0; i < l; i++) buffers[i]->rendered = false;
    int first = 0;
    while(first < l) {
      if(buffers[first]->rendered) {
        first ++;
        continue;
      }
      fgfx::checkGLError();
      auto shader = buffers[first]->shader;
      ShaderBinding* binding = shader->requestBinding(&GeneratorBuffer::shaderBindingRequest);
      fgfx::checkGLError();

      GeneratorBuffer::beginDraw(shader.get(), binding, mvp);

      if(shader->loaded) buffers[first]->render(binding);
      buffers[first]->rendered = true;

      for(int i = first + 1; i < l; i++) {
        if(buffers[i]->shader == shader) {
          if(shader->loaded) buffers[i]->render(binding);
          buffers[i]->rendered = true;
        }
      }

      GeneratorBuffer::endDraw(binding);
      fgfx::checkGLError();
      first ++;
    }
  }
  void GeneratorBuffer::beginDraw(Shader* shader, ShaderBinding* binding, const ModelViewProjection& mvp) {
    glUseProgram(shader->program);

    fgfx::checkGLError();

    for(int i = 0; i < 7; i++) if(binding->attribLocations[i]!=-1) glEnableVertexAttribArray(binding->attribLocations[i]);

    //for(int i = 0; i <= 5; i++) fgfx_log("BINDING ATTRIB %s #%d", shaderBindingRequest.attribNames[i].c_str(), binding->attribLocations[i]);
    //for(int i = 0; i <= 1; i++) fgfx_log("BINDING UNIFORM %s #%d", shaderBindingRequest.uniformNames[i].c_str(), binding->uniformLocations[i]);

    fgfx::checkGLError();

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
  void GeneratorBuffer::endDraw(ShaderBinding* binding) {
    for(int i = 0; i < 7; i++) if(binding->attribLocations[i]!=-1) glDisableVertexAttribArray(binding->attribLocations[i]);
  }


}
