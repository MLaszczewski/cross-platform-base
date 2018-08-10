//
// Created by Michał Łaszczewski on 15/04/16.
//

#include "shaders.h"
#include <cstddef>
#include <cstdio>
#include <string>
#include <sstream>

namespace fgfx {

  ShaderException::ShaderException(std::string errorp, GLuint shaderTypep) {
    error = errorp;
    shaderType = shaderTypep;
    std::ostringstream str;
    str << "SHADER ERROR:\n" << error << "\n";
    message = str.str();
    //fgfx_log("%s", message.c_str());
  }

  std::string& replaceAll(std::string& there, std::string what, std::string to) {
    std::string::size_type n = 0;
    while ( ( n = there.find( what, n ) ) != std::string::npos ) {
      there.replace( n, what.size(), to );
      n += to.size();
    }
    return there;
  }

  GLuint loadShader(GLenum type, std::string shaderSrc) {
#ifdef __USE_OPENGL
    replaceAll(shaderSrc, "precision highp float;", "                       ");
    replaceAll(shaderSrc, "precision mediump float;", "                       ");
    replaceAll(shaderSrc, "precision lowp float;", "                    ");
    replaceAll(shaderSrc, "highp ", "      ");
    replaceAll(shaderSrc, "mediump ", "        ");
    replaceAll(shaderSrc, "lowp ", "     ");
#endif
    GLuint shader;
    GLint compiled;

    //fgfx_log("Loading shader: \n%s\n", shaderSrc.c_str());

    // Create the shader object
    shader = glCreateShader(type);

    if (shader == 0)
      return 0;

    const char* csrc = shaderSrc.c_str();

    // Load the shader source
    glShaderSource(shader, 1, &csrc, NULL);

    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);


    if (!compiled) {
      GLint infoLen = 0;
      std::string error;

      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

      if (infoLen > 1) {
        char *infoLog = new char[infoLen];

        glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
        fgfx_log("Error compiling shader:\n%s\n", infoLog);

        error = infoLog;

        delete[] infoLog;
      }

      glDeleteShader(shader);
      throw ShaderException(error, type);
      //return 0;
    }

    return shader;

  }

  GLuint createProgram( GLuint vertexShader, GLuint fragmentShader) {
    // Create the program object
    auto programObject = glCreateProgram();

    if ( programObject == 0 ) {
      fgfx_log("Could not create program object!");
      return GL_FALSE;
    }

    glAttachShader ( programObject, vertexShader );
    glAttachShader ( programObject, fragmentShader );

    // Bind vPosition to attribute 0
    //glBindAttribLocation ( programObject, 0, "vPosition" );

    // Link the program
    glLinkProgram(programObject);

    GLint linked;

    // Check the link status
    glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

    if ( !linked )
    {
      GLint infoLen = 0;
      std::string error;

      glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );

      if ( infoLen > 1 )
      {
        char* infoLog = new char[infoLen];

        glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
        fgfx_log( "Error linking program:\n%s", infoLog );

        error = infoLog;

        delete[] infoLog;
      } else  {
        fgfx_log("Linking failed without info!");
      }

      glDeleteProgram ( programObject );

      throw ShaderException(error, 0);
      return GL_FALSE;
    }

    return programObject;
  }

}