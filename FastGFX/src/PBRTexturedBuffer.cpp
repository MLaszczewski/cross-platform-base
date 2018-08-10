//
// Created by Michał Łaszczewski on 14/04/16.
//

#include <fastgfx.h>

#include "PBRTexturedBuffer.h"

namespace fgfx {

  ShaderBindingRequest PBRTexturedBuffer::shaderBindingRequest = {
      .attribNames = { "aPosition", "aColor", "aNormal", "aTangent", "aBitangent",
                       "aColorCoord", "aNormalCoord", "aMaterialCoord" },
      .uniformNames = { "uSampler", "uProjectionMatrix", "uViewMatrix", "uModelMatrix", "uModelMatrix3", "uEyePosition" }
  };

  PBRTexturedBuffer::PBRTexturedBuffer(int textureIdp) : textureId(textureIdp) {
    glGenBuffers(1, &positionBuffer);
    glGenBuffers(1, &colorBuffer);
    glGenBuffers(1, &normalBuffer);
    glGenBuffers(1, &tangentBuffer);
    glGenBuffers(1, &bitangentBuffer);
    glGenBuffers(1, &colorCoordBuffer);
    glGenBuffers(1, &normalCoordBuffer);
    glGenBuffers(1, &materialCoordBuffer);
    texturedPointsCount = 0;
  }

  PBRTexturedBuffer::~PBRTexturedBuffer() {
    glDeleteBuffers(1, &positionBuffer);
    glDeleteBuffers(1, &colorBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteBuffers(1, &tangentBuffer);
    glDeleteBuffers(1, &bitangentBuffer);
    glDeleteBuffers(1, &colorCoordBuffer);
    glDeleteBuffers(1, &normalCoordBuffer);
    glDeleteBuffers(1, &materialCoordBuffer);
  }

  void PBRTexturedBuffer::reserve(int pointCount) {
    vertices.reserve( pointCount );
    colors.reserve( pointCount );
    normals.reserve( pointCount );
    tangents.reserve( pointCount );
    bitangents.reserve( pointCount );
    colorCoords.reserve( pointCount );
    normalCoords.reserve( pointCount );
    materialCoords.reserve( pointCount );
  }

  void PBRTexturedBuffer::bufferPoint(glm::vec3 pos, glm::vec4 color,
                                      glm::vec2 colorCoord, glm::vec2 normalCoord, glm::vec2 materialCoord) {
    vertices.push_back( pos );
    colors.push_back( color );
    normals.push_back( glm::vec3(0,0,1) );
    tangents.push_back( glm::vec3(1,0,0) );
    bitangents.push_back( glm::vec3(0,1,0) );
    colorCoords.push_back( colorCoord );
    normalCoords.push_back( normalCoord );
    materialCoords.push_back( materialCoord );
    texturedPointsCount++;
    //emscripten_log(EM_LOG_ERROR, "BUFFER POINT (%.4lf,%.4lf) (%.2lf,%.2lf,%.2lf,%.2lf) (%.2lf,%.2lf)\n",
    //               pos.x,pos.y,color.r,color.g,color.b,color.a,coord.x,coord.y);
  }

  void PBRTexturedBuffer::reset() {
    vertices.clear();
    colors.clear();
    normals.clear();
    tangents.clear();
    bitangents.clear();
    colorCoords.clear();
    normalCoords.clear();
    materialCoords.clear();
    texturedPointsCount = 0;
  }

  void PBRTexturedBuffer::computeNormals() {
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
  void PBRTexturedBuffer::computeTangents() {
    tangents.resize( texturedPointsCount );
    bitangents.resize( texturedPointsCount );
    for(int i = 0; i < texturedPointsCount; i += 3) {
      // Shortcuts for vertices
      glm::vec3 & v0 = vertices[i + 0];
      glm::vec3 & v1 = vertices[i + 1];
      glm::vec3 & v2 = vertices[i + 2];

      // Shortcuts for UVs
      glm::vec2 & uv0 = normalCoords[i + 0];
      glm::vec2 & uv1 = normalCoords[i + 1];
      glm::vec2 & uv2 = normalCoords[i + 2];

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

  void PBRTexturedBuffer::upload() {
    if (!texturedPointsCount) return;

    /*
    GLfloat* points = (GLfloat*)vertices.data();
    printf("FIRST 3 VERTICES (%f %f %f) (%f %f %f) (%f %f %f)\n",
           points[0],points[1],points[2],
           points[3],points[4],points[5],
           points[6],points[7],points[8]);//*/

    //*
    /*GLfloat* points = (GLfloat*)normals.data();
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
    GLfloat* points = (GLfloat*)colorCoords.data();
    printf("FIRST 3 COORDS (%f %f) (%f %f) (%f %f)\n",
           points[0],points[1],
           points[2],points[3],
           points[4],points[5]);//*/

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, texturedPointsCount * sizeof(glm::vec3), vertices.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, texturedPointsCount * sizeof(glm::vec4), colors.data(),
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

    glBindBuffer(GL_ARRAY_BUFFER, colorCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, texturedPointsCount * sizeof(glm::vec2), colorCoords.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normalCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, texturedPointsCount * sizeof(glm::vec2), normalCoords.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, materialCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, texturedPointsCount * sizeof(glm::vec2), materialCoords.data(),
                 GL_DYNAMIC_DRAW);

  }

  void PBRTexturedBuffer::render(ShaderBinding* binding) {
    if (!texturedPointsCount) return;

    /*
                     { "aPosition", "aColor", "aNormal", "aTangent", "aBitangent",
                       "aColorCoord", "aNormalCoord", "aMaterialCoord" },
    */

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glVertexAttribPointer(binding->attribLocations[0], 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glVertexAttribPointer(binding->attribLocations[1], 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer(binding->attribLocations[2], 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
    glVertexAttribPointer(binding->attribLocations[3], 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, bitangentBuffer);
    glVertexAttribPointer(binding->attribLocations[4], 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, colorCoordBuffer);
    glVertexAttribPointer(binding->attribLocations[5], 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, normalCoordBuffer);
    glVertexAttribPointer(binding->attribLocations[6], 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, materialCoordBuffer);
    glVertexAttribPointer(binding->attribLocations[7], 2, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, texturedPointsCount);
  }

  void PBRTexturedBuffer::renderLayer(BuffersLayer<PBRTexturedBuffer>* layer, const ModelViewProjection& mvp) {
    std::vector<PBRTexturedBuffer*>& buffers = layer->buffers;
    int l = buffers.size();
    for(int i = 0; i < l; i++) buffers[i]->rendered = false;
    int first = 0;
    while(first < l) {
      if(buffers[first]->rendered) {
        first ++;
        continue;
      }
      GLuint textureId = buffers[first]->textureId;
      auto shader = buffers[first]->shader;

      ShaderBinding* binding = shader->requestBinding(&PBRTexturedBuffer::shaderBindingRequest);

      PBRTexturedBuffer::beginDraw(textureId, shader.get(), binding, mvp);

      if(shader->loaded) buffers[first]->render(binding);
      buffers[first]->rendered = true;

      for(int i = first + 1; i < l; i++) {
        if(buffers[i]->textureId == textureId && buffers[i]->shader == shader) {
          if(shader->loaded) buffers[i]->render(binding);
          buffers[i]->rendered = true;
        }
      }

      PBRTexturedBuffer::endDraw(binding);
      first ++;
    }
  }
  void PBRTexturedBuffer::beginDraw(GLuint textureId, Shader* shader, ShaderBinding* binding, const ModelViewProjection& mvp) {
    glUseProgram(shader->program);

    for(int i = 0; i < 8; i++) glEnableVertexAttribArray(binding->attribLocations[i]);

    //for(int i = 0; i <= 5; i++) fgfx_log("BINDING ATTRIB %s #%d", shaderBindingRequest.attribNames[i].c_str(), binding->attribLocations[i]);
    //for(int i = 0; i <= 1; i++) fgfx_log("BINDING UNIFORM %s #%d", shaderBindingRequest.uniformNames[i].c_str(), binding->uniformLocations[i]);

    glBindTexture(GL_TEXTURE_2D, textureId);
    glUniform1i(binding->uniformLocations[0], 0);

    glUniformMatrix4fv(binding->uniformLocations[1], 1, GL_FALSE,
                       (GLfloat*)glm::value_ptr(mvp.projectionMatrix));
    glUniformMatrix4fv(binding->uniformLocations[2], 1, GL_FALSE,
                       (GLfloat*)glm::value_ptr(mvp.viewMatrix));
    glUniformMatrix4fv(binding->uniformLocations[3], 1, GL_FALSE,
                       (GLfloat*)glm::value_ptr(mvp.modelMatrix));
    glm::mat3 modelMatrix3(mvp.modelMatrix);
    glUniformMatrix3fv(binding->uniformLocations[4], 1, GL_FALSE,
                       (GLfloat*)glm::value_ptr(modelMatrix3));
    glUniform3fv(binding->uniformLocations[5], 1, (GLfloat*)glm::value_ptr(mvp.eyePosition));

  }
  void PBRTexturedBuffer::endDraw(ShaderBinding* binding) {
    for(int i = 0; i < 8; i++) glDisableVertexAttribArray(binding->attribLocations[i]);
  }


}
