#ifndef ENGINE_H
#define ENGINE_H

#include "SpriteFont.h"
#include "Sprite.h"
#include "Texture.h"
#include "SpriteLayer.h"
#include "PolygonLayer.h"
#include "LineLayer.h"
#include "TexturedLayer.h"
#include "TextureTransitionLayer.h"
#include "shaders.h"

#include <map>
#include <vector>
#include <string>
#include <memory>

#ifndef EMSCRIPTEN
#include "concurrentqueue.h"
#include "Image.h"
#include "SpritesTexture.h"
#include "TextureManager.h"

#endif

namespace fgfx {

  class Engine : public std::enable_shared_from_this<Engine> {
  protected:
    std::map<std::string, std::shared_ptr<Sprite>> sprites;
    std::map<std::string, std::shared_ptr<SpriteFont>> spriteFonts;
    std::map<std::string, std::shared_ptr<Texture>> textures;

#ifdef EMSCRIPTEN
    std::vector<std::shared_ptr<Sprite>> spritesToLoad;
    std::vector<std::shared_ptr<SpriteFont>> spriteFontsToLoad;
    std::vector<std::shared_ptr<Texture>> texturesToLoad;
#endif

#ifndef EMSCRIPTEN
  public:
    TextureManager textureManager;
    using UploadTask = std::function<void()>;
    moodycamel::ConcurrentQueue<UploadTask> uploadQueue;
  protected:
#endif

    friend class Sprite;
    friend class SpriteFont;

    std::function<void(double,float)> renderFunction;

  public:
    double renderTime;
    float renderDelta;

    int width;
    int height;

    double realWidth;
    double realHeight;

    Engine();
    ~Engine();

    std::shared_ptr<Sprite> getSprite(std::string spriteName);
    std::shared_ptr<Sprite> getEmptySprite(std::string spriteName);
    std::shared_ptr<SpriteFont> getSpriteFont(std::string spriteFontName);
    std::shared_ptr<Texture> getTexture(std::string texture);
    std::shared_ptr<Texture> createEmptyTexture(int width, int height, GLint internalFormat, GLenum format, GLenum type);

#ifdef EMSCRIPTEN
    int getSpritesToLoadCount();
    std::shared_ptr<Sprite> getSpriteToLoad(int n);
    void clearSpritesToLoad();
    int getSpriteFontsToLoadCount();
    std::shared_ptr<SpriteFont> getSpriteFontToLoad(int n);
    void clearSpriteFontsToLoad();
    int getTexturesToLoadCount();
    std::shared_ptr<Texture> getTextureToLoad(int n);
    void clearTexturesToLoad();
#endif

    void reloadSprite(std::shared_ptr<Sprite> spritep);
    void reloadTexture(std::shared_ptr<Texture> texturep);

    void setRenderFunction(std::function<void(float,float)> renderFunctionp);
    void render(double time, float delta, int widthp, int heightp);
    void uploadTextures();

    std::shared_ptr<SpriteLayer> createSpriteLayer();
    std::shared_ptr<PolygonLayer> createPolygonLayer();
    std::shared_ptr<LineLayer> createLineLayer();
    std::shared_ptr<TexturedLayer> createTexturedLayer();
    std::shared_ptr<TextureTransitionLayer> createTextureTransitionLayer();

    void upload(std::function<void()> uploadFunction);

#ifndef EMSCRIPTEN
    void mainLoop();
#endif
  };

};

#endif // ENGINE_H
