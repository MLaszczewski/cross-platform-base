//
// Created by Michał Łaszczewski on 09/07/16.
//
#include <fastgfx.h>
#include "../src/Engine.h"
#include <cmath>

#define PI 3.141592

int main() {
  int res;
  res = fgfx::init();
  if(res!=0) {
    fgfx_log("FGFX Initialization failed\n");
    return res;
  }

  auto logoSprite = fgfx::engine->getSprite("../assets/logo.png");
  auto spriteLayer = fgfx::engine->createSpriteLayer();
  auto font = fgfx::engine->getSpriteFont("../assets/cat");

  fgfx::engine->setRenderFunction([logoSprite,spriteLayer,font](double time, float delta) {
   // fgfx_log("FGFX Render! %d %d %f\n",fgfx::engine->width, fgfx::engine->height,delta);
    float aspectRatio = 1.0 * fgfx::engine->width / fgfx::engine->height;
    glm::mat4 projection = glm::perspective(35.0f, aspectRatio, 0.1f, 1000.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 20), glm::vec3(0, 0, 0),glm::vec3(0, -1, 0));
    glm::mat4 camera = projection * view;

    glClearColor(0.9, 0.9, 0.95, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    spriteLayer->reset();
    for(float i = 100; i > 0; i-=0.2) {
      float cycleTime = fmod((time-i)/10000,1.0)*2*PI;
      float rotation = cycleTime*sin(cycleTime)*12.3;
      spriteLayer->bufferSprite(logoSprite,
                                glm::scale(
                                    glm::rotate(glm::mat4(), rotation, glm::vec3(0, 0, 1)), glm::vec3(10, 10, 10)),
                                glm::vec4(1, 1, 1, 1.0/(i*0.1)));
    }

    float fontScale = 0.23;
    font->renderString(spriteLayer,glm::scale(glm::translate(glm::mat4(),glm::vec3(-80,80,0)), glm::vec3(fontScale,fontScale,fontScale)),
                       glm::vec4(0.6,0,0.7,1),"FastGFX FTW!");

    spriteLayer->upload();
    spriteLayer->render(camera);

  });
}
