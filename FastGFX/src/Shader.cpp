//
// Created by Michał Łaszczewski on 09/05/17.
//

#include "Shader.h"
#include "shaders.h"

namespace fgfx {

  ShaderBinding* Shader::requestBinding(ShaderBindingRequest* request) {
    for(auto binding : bindings) {
      if(binding.first == request) return binding.second;
    }
    ShaderBinding* binding = new ShaderBinding();
    initializeBinding(request, binding);
    return binding;
  }

  void Shader::initializeBinding(ShaderBindingRequest *request, ShaderBinding *binding) {
    binding->attribLocations.resize(request->attribNames.size());
    binding->uniformLocations.resize(request->uniformNames.size());
    if(loaded) {
      for(int i = 0; i < request->attribNames.size(); i++) {
        binding->attribLocations[i] = glGetAttribLocation(program, request->attribNames[i].c_str());
      }
      for(int i = 0; i < request->uniformNames.size(); i++) {
        binding->uniformLocations[i] = glGetUniformLocation(program, request->uniformNames[i].c_str());
      }
    } else {
      for(int i = 0; i < request->attribNames.size(); i++) binding->attribLocations[i] = 0;
      for(int i = 0; i < request->uniformNames.size(); i++) binding->uniformLocations[i] = 0;
    }
  }

  Shader::Shader() {
  }
  Shader::~Shader() {
  }

  void Shader::upload(std::string vertexCode, std::string fragmentCode) {
    if(loaded) {
      glDeleteProgram(program);
      loaded = false;
    }

    auto vertexShader = fgfx::loadShader ( GL_VERTEX_SHADER, vertexCode.c_str() );
    auto fragmentShader = fgfx::loadShader ( GL_FRAGMENT_SHADER, fragmentCode.c_str() );

    program = fgfx::createProgram (vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if(!program) throw ShaderException("unknown error!", 0);

    for(auto binding : bindings) {
      initializeBinding(binding.first, binding.second);
    }

    loaded = true;
  }

}