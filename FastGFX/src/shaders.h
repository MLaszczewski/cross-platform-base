//
// Created by Michał Łaszczewski on 15/04/16.
//

#ifndef ENGINE_SHADERS_H
#define ENGINE_SHADERS_H

#include "config.h"
#include <string>

#ifndef _NOEXCEPT
#define _NOEXCEPT _GLIBCXX_USE_NOEXCEPT _GLIBCXX_TXN_SAFE_DYN
#endif

namespace fgfx {

  class ShaderException : public std::exception {
  public:
    std::string message;
    std::string error;
    GLuint shaderType;
    ShaderException(std::string error, GLuint shaderTypep);
    virtual const char* what() const _NOEXCEPT override { return message.c_str(); }
  };


  GLuint loadShader ( GLenum type, std::string shaderSrc);
  GLuint createProgram( GLuint vertexShader, GLuint fragmentShader);

}


#endif //ENGINE_SHADERS_H
