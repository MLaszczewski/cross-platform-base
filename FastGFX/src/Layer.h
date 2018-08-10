#ifndef LAYER_H
#define LAYER_H

#include <string>

#include <glm/glm.hpp>
#include <functional>
#include <memory>
#include "TypeList.h"

namespace fgfx {

  class Engine;

  struct ModelViewProjection {
    glm::mat4 multipliedMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 modelMatrix;
    glm::vec3 eyePosition;
    void multiply() {
      multipliedMatrix = projectionMatrix * viewMatrix * modelMatrix;
    }
  };

  class Layer;

  struct LayerType {
    std::string name;
    unsigned short id;
    std::function<std::shared_ptr<Layer>(Engine* enginep)> layerGenerator;
  };

  class Layer {
  public:
    Engine* engine;
    int composition;
    bool frozen;
    bool visible;
    bool camera;
    Layer(Engine* enginep);
    virtual ~Layer() = 0;

    virtual void reset() {};
    virtual void upload() {};
    virtual void render(const ModelViewProjection& mvp) {};

    virtual LayerType* type() { return nullptr; }
  };

  template<class T> class TypedLayer : public Layer {
  public:
    static LayerType staticType;

    virtual LayerType* type() override {
      return &staticType;
    };

    TypedLayer(Engine* enginep) : Layer(enginep) {}
  };

  template <typename T> LayerType TypedLayer<T>::staticType = {
      .id = TypeList<LayerType>::registerType(&T::staticType),
      .layerGenerator = [](Engine* engine){
        return std::make_shared<T>(engine);
      }
  };

};

#endif // LAYER_H
