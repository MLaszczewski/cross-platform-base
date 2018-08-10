//
// Created by Michał Łaszczewski on 09/12/16.
//

#ifndef EMSCRIPTEN

#include <vector>
#include <algorithm>
#include <fastgfx.h>

namespace fgfx {

  STNode::STNode(int xp, int yp, int widthp, int heightp) : left(nullptr), right(nullptr), image(nullptr),
    x(xp), y(yp), width(widthp), height(heightp) {
  }
  std::shared_ptr<STNode> STNode::insert(std::shared_ptr<Image> src_image, int spacing) {
    int box_width = src_image->width + spacing;
    int box_height = src_image->height + spacing;

    if (left && right) {
      std::shared_ptr<STNode> r = left->insert(src_image, spacing);
      if(r) return r;
      return right->insert(src_image, spacing);
    } else {
      // if there's already a sprite here, return
      if (image) return nullptr;
      // if we're too small, return
      if ((box_width > width) || ((box_height) > height)) return nullptr;
      if ((box_width == width) && ((box_height) == height)) { //if we're just right, accept
        image = src_image;
        return shared_from_this();
      } else { // otherwise, gotta split this node and create some kids
        int dw = width - box_width;
        int dh = height - box_height;
        if (dw > dh) {
          left = std::make_shared<STNode>(x, y, box_width, height);
          right = std::make_shared<STNode>(x + box_width, y, width - box_width, height);
        } else {
          left = std::make_shared<STNode>(x, y, width, box_height);
          right = std::make_shared<STNode>(x, y + box_height, width, height - box_height);
        }
        // insert into first child we created
        return left->insert(src_image, spacing);
      }
    }
  }

  SpritesTexture::SpritesTexture(int sizep, int spacingp) : size(sizep), spacing(spacingp) {
    image = std::make_shared<Image>(size, size);
    textureCreated = false;
  }

  bool SpritesTexture::tryPack(std::vector<SpriteInfo> &sprites) {
    std::vector<std::shared_ptr<STNode>> nodes(sprites.size());

    std::shared_ptr<STNode> root = std::make_shared<STNode>(0, 0, size, size);

    std::sort(sprites.begin(), sprites.end(),
              [](const SpriteInfo & a, const SpriteInfo & b) -> bool {
                return std::max(b.image->width, b.image->height) < std::max(a.image->width, a.image->height);
              });

    for (int i = 0; i < sprites.size(); i++) {
      SpriteInfo sprite = sprites[i];
      std::shared_ptr<STNode> node = root->insert(sprite.image, spacing*2);
      if (!node) return false;
      nodes[i] = node;
    }


    /// TODO: posprzątac
    float fix = -2;

    for (int i = 0; i < sprites.size(); i++) {
      std::shared_ptr<STNode>& node = nodes[i];
      SpriteInfo& sprite = sprites[i];
      sprite.x = node->x;
      sprite.y = node->y;
      sprite.xmin = (1.0 * node->x + fix) / size;
      sprite.xmax = (1.0 * node->x + node->width + fix - spacing) / size;
      sprite.ymin = (1.0 * node->y + fix) / size;
      sprite.ymax = (1.0 * node->y + node->height + fix - spacing) / size;
      sprite.texture = texture;
    }
    packedSprites = sprites;

    image->clear();
    for(SpriteInfo& info : sprites) {
      image->put(info.image, info.x, info.y);
    }

    fgfx_log("SPRITES TEXTURE REQUEST UPLOAD")
    fgfx::engine->uploadQueue.enqueue([this]{
      this->upload();
    });

    return true;
  }

  bool SpritesTexture::tryAddSprite(SpriteInfo& sprite) {
    std::vector<SpriteInfo> newSprites = packedSprites;
    newSprites.push_back(sprite);
    return tryPack(newSprites);
  }

  bool SpritesTexture::isBigEnough(int w, int h) {
    return (w < size && h < size);
  }

  void SpritesTexture::clean() {
    for (int i = 0; i < packedSprites.size(); i++) {
      SpriteInfo& info = packedSprites[i];
      if (info.sprite && (!info.sprite->locked) && (!info.sprite->locked)
          && fgfx::engine->renderTime - info.sprite->lastUseTime > 3000) { /// REMOVE SPRITE
        info.sprite->unload();
        packedSprites.erase(packedSprites.begin()+i);
        i--;
      }
    }
  }

  void SpritesTexture::upload() { // in renderer thread
    fgfx_log("SPRITES TEXTURE UPLOAD")
    if(!textureCreated) {
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      textureCreated = true;
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
    glGenerateMipmap(GL_TEXTURE_2D);

    for(SpriteInfo& info : packedSprites) {
      info.sprite->setTextureFragment(texture, info.xmin, info.ymin, info.xmax, info.ymax,
                                      info.image->width, info.image->height, false);
    }
  }

}


#endif
