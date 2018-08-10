//
// Created by Michał Łaszczewski on 06/03/17.
//

#ifndef FGFX_UNIVERSALLAYER_H
#define FGFX_UNIVERSALLAYER_H

#include <memory>
#include <Fox2D/utils/log.h>
#include "Layer.h"

namespace fgfx {

  class UniversalLayer : public TypedLayer<UniversalLayer> {
  public:
    std::vector<std::shared_ptr<Layer>> layers;

    virtual void reset() override;
    virtual void upload() override;
    virtual void render(const ModelViewProjection& mvp) override;

    UniversalLayer(Engine* enginep);
    ~UniversalLayer();

    template<typename T> std::shared_ptr<T> getLayer() {
      auto id = T::staticType.id;
      for(auto it = layers.begin(); it != layers.end(); it++) {
        if((*it)->type()->id == id) {
          return std::dynamic_pointer_cast<T>(*it);
        }
      }

      auto nlayer = std::dynamic_pointer_cast<T>(T::staticType.layerGenerator(engine));
      layers.push_back(nlayer);
      return nlayer;
    }

    std::shared_ptr<Layer> getLayer(std::string typeName);
  };

}


#endif //TANKS_UNIVERSALLAYER_H
