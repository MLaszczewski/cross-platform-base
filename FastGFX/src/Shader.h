//
// Created by Michał Łaszczewski on 09/05/17.
//

#ifndef FGFX_SHADER_H
#define FGFX_SHADER_H

#include "config.h"
#include <vector>
#include <string>

namespace fgfx {

  struct ShaderBindingRequest {
    std::vector<std::string> attribNames;
    std::vector<std::string> uniformNames;
  };

  struct ShaderBinding {
    std::vector<GLuint> attribLocations;
    std::vector<GLuint> uniformLocations;
  };

  class Shader {
  public:
    bool loaded;
    GLuint program;
    std::vector<std::pair<ShaderBindingRequest*, ShaderBinding*>> bindings;
    ShaderBinding* requestBinding(ShaderBindingRequest* request);
    void initializeBinding(ShaderBindingRequest* request, ShaderBinding* binding);

    Shader();
    ~Shader();

    void upload(std::string vertexCode, std::string fragmentCode);
  };

}


#endif //FGFX_SHADER_H
