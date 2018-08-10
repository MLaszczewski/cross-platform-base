//
// Created by Michał Łaszczewski on 10/02/17.
//

#ifndef FSXX_H
#define FSXX_H

#include <string>
#include <functional>
#include <memory>

#include <Promise.h>

#ifndef _NOEXCEPT
#define _NOEXCEPT _GLIBCXX_USE_NOEXCEPT _GLIBCXX_TXN_SAFE_DYN
#endif

#ifdef __ANDROID
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif

namespace fsxx {

  class FileNotFoundError : public std::exception {
  public:
    std::string message;
    FileNotFoundError(std::string name) {
      message = "File "+name+" not found";
    }
    virtual const char* what() const _NOEXCEPT override { return message.c_str(); }
  };

  std::shared_ptr<promise::Promise<std::string>> loadFile(std::string path);

  void init(int loadThreads = 1);
  void destroy();

  #ifdef __ANDROID
    extern AAssetManager* assetManager;
  #endif

}

#endif //TANKS_FS_H
