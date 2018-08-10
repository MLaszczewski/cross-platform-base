//
// Created by Michał Łaszczewski on 06/03/17.
//

#include "UniversalLayer.h"

namespace fgfx {

  class UnknownLayerType : public std::exception {
    virtual const char* what() const _NOEXCEPT override {
      return "UnknownLayerType";
    }
  };

  void UniversalLayer::reset() {
    for(auto& layer : layers) {
      layer->reset();
    }
  }
  void UniversalLayer::upload() {
    for(auto& layer : layers) {
      layer->upload();
    }
  }
  void UniversalLayer::render(const ModelViewProjection& mvp) {
    for(auto& layer : layers) {
      layer->render(mvp);
    }
  }

  UniversalLayer::UniversalLayer(Engine* enginep) : TypedLayer<UniversalLayer>(enginep) {}
  UniversalLayer::~UniversalLayer() {}

  std::shared_ptr<Layer> UniversalLayer::getLayer(std::string typeName) {
    for(auto& layer : layers) {
      if(layer->type()->name == typeName) return layer;
    }
    for(auto type : TypeList<LayerType>::typeById) {
      if(type->name == typeName) {
        auto nlayer = type->layerGenerator(engine);
        layers.push_back(nlayer);
        return nlayer;
      }
    }
    throw UnknownLayerType();
  }

}