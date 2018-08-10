#ifndef EMSCRIPTEN

#ifndef FGFX_TEXTUREMANAGER_H
#define FGFX_TEXTUREMANAGER_H

#include "SpritesTexture.h"
#include "SpriteFont.h"
#include "Texture.h"

#include <map>
#include <vector>
#include <memory>

namespace fgfx {

  class TextureManager {
  public:

    int maxSize;
    int spacing;

    std::vector<std::shared_ptr<SpritesTexture>> spriteTextures;

    TextureManager();
    ~TextureManager();

    void loadSprite(std::shared_ptr<Sprite> sprite, std::shared_ptr<Image> image);
    void loadSpriteFont(std::shared_ptr<SpriteFont> sprite, std::shared_ptr<Image> image, std::string& data);
    void loadTexture(std::shared_ptr<Texture> texture, std::shared_ptr<Image> image);

    void clean();

  };

}

#endif //FGFX_TEXTUREMANAGER_H
#endif //EMSCRIPTEN