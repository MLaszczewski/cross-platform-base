//
// Created by Michał Łaszczewski on 09/12/16.
//

#ifndef EMSCRIPTEN

#ifndef FGFX_SPRITESTEXTURE_H
#define FGFX_SPRITESTEXTURE_H

#include <memory>
#include "Image.h"
#include "Sprite.h"
#include "config.h"

namespace fgfx {

  class STNode : public std::enable_shared_from_this<STNode> {
  public:
    std::shared_ptr <STNode> left;
    std::shared_ptr <STNode> right;
    std::shared_ptr <Image> image;
    int x;
    int y;
    int width;
    int height;

    STNode(int xp, int yp, int widthp, int heightp);
    std::shared_ptr<STNode> insert(std::shared_ptr<Image> image, int spacing);
  };

  class SpriteInfo {
  public:
    std::shared_ptr<Sprite> sprite;
    std::shared_ptr<Image> image;
    int x, y;
    float xmin, xmax, ymin, ymax;
    GLuint texture;
  };

  class SpritesTexture {
  public:
    int size;
    int spacing;

    std::shared_ptr<Image> image;
    std::vector<SpriteInfo> packedSprites;

    bool textureCreated; // only for renderer thread
    GLuint texture;

    SpritesTexture(int sizep, int spacingp);
    bool tryPack(std::vector<SpriteInfo>& sprites);
    bool tryAddSprite(SpriteInfo& sprite);
    bool isBigEnough(int w, int h);
    void clean();

    void upload(); // only for renderer thread
  };

}
#endif //FGFX_SPRITESTEXTURE_H
#endif // EMSCRIPTEN