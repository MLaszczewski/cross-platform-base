//
// Created by Michał Łaszczewski on 09/05/17.
//

#ifndef FGFX_SHADERLAYER_H
#define FGFX_SHADERLAYER_H

#include "Layer.h"
#include <functional>

namespace fgfx {

  template<class Buffer> class BuffersLayer : public TypedLayer<BuffersLayer<Buffer>> {
  public:
    std::vector<Buffer*> buffers;

    Buffer* getBuffer(std::function<bool(Buffer*)> discriminator, std::function<Buffer*()> generator) {
      for(Buffer* buf : buffers) {
        if(discriminator(buf)) return buf;
      }
      Buffer* nbuf = generator();
      buffers.push_back(nbuf);
      return nbuf;
    }

    BuffersLayer(Engine* enginep) : TypedLayer<BuffersLayer<Buffer>>(enginep) {}
    virtual ~BuffersLayer() {};

    virtual void reset() override {
      for(Buffer* buf : buffers) buf->reset();
    }
    virtual void upload() override {
      for(Buffer* buf : buffers) buf->upload();
    }
    virtual void render(const ModelViewProjection& mvp) override {
      Buffer::renderLayer(this, mvp);
    };

  };

}


#endif //FGFX_SHADERLAYER_H
