//
// Created by Michał Łaszczewski on 30/11/16.
//

#ifndef FGFX_TYPELIST_H
#define FGFX_TYPELIST_H

#include <vector>
#include "config.h"

namespace fgfx {

  template <class T> class TypeList {
  public:
    static std::vector<T*> typeById;
    static unsigned short registerType(T* t) {
      unsigned int size = typeById.size();
      fgfx_log("REGISTER TYPE %d", size);
      typeById.push_back(t);
      return size;
    }
    static T* get(unsigned short typeId) {
      if(typeId >= typeById.size()) {
        fgfx_log("Unknown type id %d", typeId);
        throw "unknown type id";
      }
      return typeById[typeId];
    }
  };

  template <class T> std::vector<T*> TypeList<T>::typeById = {};

}

#endif //fgfx2D_TYPELIST_H
