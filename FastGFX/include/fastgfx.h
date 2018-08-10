//
// Created by Michał Łaszczewski on 01/07/16.
//

#ifndef FGFX_H
#define FGFX_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../src/config.h"

#include <memory>
#include <functional>
#include <Promise.h>

namespace fgfx {

  class Engine;
  class LineLayer;
  class SpriteLayer;
  class Layer;
  class Sprite;
  class PolygonLayer;
  class SpriteFont;
  class Texture;
  class TexturedLayer;
  class TextureTransitionLayer;
  class UniversalLayer;
  class NormalMappedSpriteLayer;
  class PBRSpriteLayer;
  class FrameBuffer;

  class InitializationFailed : public std::exception {};

  void init();
  void destroy();
  extern std::shared_ptr<Engine> engine;

#ifdef __USE_GLFW
  extern GLFWwindow* glfwWindow;
#endif
}

#include "../src/Sprite.h"
#include "../src/Engine.h"
#include "../src/LineLayer.h"
#include "../src/SpriteLayer.h"
#include "../src/Layer.h"
#include "../src/PolygonLayer.h"
#include "../src/SpriteFont.h"
#include "../src/Texture.h"
#include "../src/FrameBuffer.h"

#include "../src/TexturedBuffer.h"
#include "../src/TexturedLayer.h"
#include "../src/TextureTransitionLayer.h"
#include "../src/Image.h"

#include "../src/UniversalLayer.h"
#include "../src/NormalMappedSpriteLayer.h"
#include "../src/PBRSpriteLayer.h"

#include "../src/BuffersLayer.h"
#include "../src/PBRSpriteShaderBuffer.h"
#include "../src/PBRPolygonBuffer.h"
#include "../src/PBRTexturedBuffer.h"

#include "../src/GeneratorBuffer.h"

#endif //FGFX_H
