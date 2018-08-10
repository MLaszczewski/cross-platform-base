//
// Created by Michał Łaszczewski on 04/05/16.
//

#ifndef ENGINE_SPRITEFONT_H
#define ENGINE_SPRITEFONT_H

#include <glm/glm.hpp>
#include "SpriteLayer.h"

namespace fgfx {

  class SpriteCharacter {
  public:
    std::shared_ptr<Sprite> sprite;
    glm::vec2 size;
    glm::vec2 offset;
    float advance;
  };

  class SpriteFont : public std::enable_shared_from_this<SpriteFont> {
  public:
    std::string name;
    std::map<unsigned short, SpriteCharacter> characters;

    float lineHeight;
    float baseLine;
    glm::vec2 scale;

    SpriteFont(std::string namep);
    ~SpriteFont();

    void setHeight(float linep, float basep);
    void setCharacter(unsigned short character, std::shared_ptr<Sprite> sprite, glm::vec2 size, glm::vec2 offset,
                      float advance);

    void renderString(const std::shared_ptr<SpriteLayer>& layer, const glm::mat4& mat, const glm::vec4& color,
                      const std::string& text);
    void renderSubString(const std::shared_ptr<SpriteLayer>& layer, const glm::mat4& mat, const glm::vec4& color,
                         const std::string& text, int from, int count);

    void renderString(SpriteLayer* layer, const glm::mat4& mat, const glm::vec4& color,
                      const std::string& text);
    void renderSubString(SpriteLayer* layer, const glm::mat4& mat, const glm::vec4& color,
                         const std::string& text, int from, int count);

    glm::vec2 measureString(const std::string& text);
    glm::vec2 measureCharacter(char c);

    std::shared_ptr<promise::Promise<std::shared_ptr<SpriteFont>>> loadPromise;
  };

}

#endif //ENGINE_SPRITEFONT_H
