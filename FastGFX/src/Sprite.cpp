#include "Sprite.h"

#include "config.h"
#include <cstring>

namespace fgfx {

  Sprite::Sprite(std::string namep) : name(namep), textureId(-1), width(0), height(0), unloaded(false),
                                      loadPromise(nullptr) {
  }

  std::string Sprite::getSpriteName() {
   // emscripten_log(EM_LOG_ERROR, "GET SPRITE NAME = %s\n",stringName.c_str());
    return name;
  }

  void Sprite::setTextureFragment(int textureIdp, float left, float top, float right, float bottom, int widthp,
                                  int heightp, bool preloadedp) {
    //fgfx_log("SET TEXTURE FRAGMENT %d %s %d %d %d\n",(unsigned int)((unsigned long)this),name.c_str(),textureIdp,width,height);
    textureId = textureIdp;
    coords[0] = glm::vec2(left,bottom);
    coords[1] = glm::vec2(right,bottom);
    coords[2] = glm::vec2(right,top);
    coords[3] = glm::vec2(left,top);
    width = widthp;
    height = heightp;
    ratio = 1.0*width/height;
    preloaded = preloadedp;
  }

  void Sprite::unload() {
    textureId = -1;
    unloaded = true;
  }

};
