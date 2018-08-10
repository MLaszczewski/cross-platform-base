//
// Created by Michał Łaszczewski on 09/07/16.
//
#include <fastgfx.h>
#include "../src/Engine.h"
#include <cmath>
#include <vector>
#include <cstdlib>

std::vector<std::pair<glm::vec2, glm::vec2>> points;
#define POINTS_COUNT 23
#define POINTS_SIZE 50.0f
#define POINTS_SPEED 2.3f

int main() {
  int res;
  res = fgfx::init();
  if(res!=0) {
    fgfx_log("FGFX Initialization failed\n");
    return res;
  }

  points.resize(POINTS_COUNT);
  for(int i = 0; i < POINTS_COUNT; i++) {
    points[i].first.x = 1.0*(rand()%2000)/1000-1;
    points[i].first.y = 1.0*(rand()%2000)/1000-1;
    points[i].second.x = 1.0*(rand()%2000)/1000-1;
    points[i].second.y = 1.0*(rand()%2000)/1000-1;
  }

  auto logoSprite = fgfx::engine->getSprite("../assets/logo.png");
  auto spriteLayer = fgfx::engine->createSpriteLayer();
  auto lineLayer = fgfx::engine->createLineLayer();

  fgfx::engine->setRenderFunction([=](double time, float delta) {
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
    lineLayer->reset();

    for(int i=0; i<POINTS_COUNT; i++) {
      std::pair<glm::vec2,glm::vec2>& p = points[i];
      p.first = p.first + p.second * POINTS_SPEED * (delta*0.001f);
      if(p.first.x > 1) {
        p.second.x *= -1;
        p.first.x = 1;
      }
      if(p.first.x < -1) {
        p.second.x *= -1;
        p.first.x = -1;
      }
      if(p.first.y > 1) {
        p.second.y *= -1;
        p.first.y = 1;
      }
      if(p.first.y < -1) {
        p.second.y *= -1;
        p.first.y = -1;
      }
    }

    for(int i=0; i<POINTS_COUNT; i++)
      for(int j=i+1; j<POINTS_COUNT; j++) {
        const glm::vec2& p1 = points[i].first;
        const glm::vec2& p2 = points[j].first;
        lineLayer->bufferLine(
            glm::vec3(p1.x,p1.y,0)*POINTS_SIZE,
            glm::vec3(p2.x,p2.y,0)*POINTS_SIZE,
            glm::vec4(0,0,0.5,0.7));
      }


    spriteLayer->bufferSprite(logoSprite, glm::vec2(0,0), glm::vec4(1,1,1,0.7), 10.0, 0);

    lineLayer->upload();
    lineLayer->render(camera);
    spriteLayer->upload();
    spriteLayer->render(camera);


  });
}
