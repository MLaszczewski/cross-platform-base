//
// Created by Michał Łaszczewski on 10/02/17.
//

#include "fsxx.h"
#include <fstream>
#include <map>
#include <Promise.h>
#include <thread>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#ifndef EMSCRIPTEN
#include <concurrentqueue.h>
#endif

namespace fsxx {

#ifdef ANDROID
  AAssetManager* assetManager;
#endif

#ifndef EMSCRIPTEN
  struct AssetLoadTask {
    std::string path;
    std::shared_ptr<promise::Promise<std::string>> promise;
  };
  moodycamel::ConcurrentQueue<AssetLoadTask> assetLoadQueue;
#endif

#ifdef EMSCRIPTEN
  int lastFileLoad = 0;
  std::map<int, std::shared_ptr<promise::Promise<std::string>>> fileLoadPromises;
  extern "C" {
    EMSCRIPTEN_KEEPALIVE void fsxx_fileLoaded(int id, const char* data, int length) {
      fileLoadPromises[id]->resolve(std::string(data, length));
      fileLoadPromises.erase(id);
    }
    EMSCRIPTEN_KEEPALIVE void fsxx_fileFailed(int id) {
      fileLoadPromises[id]->reject(std::make_exception_ptr(FileNotFoundError("")));
      fileLoadPromises.erase(id);
    }
  }
#endif

  std::shared_ptr<promise::Promise<std::string>> loadFile(std::string path) {
    auto p = std::make_shared<promise::Promise<std::string>>();
#ifndef EMSCRIPTEN
    AssetLoadTask task = { .path = path, .promise = p};
    assetLoadQueue.enqueue(task);
#endif
#ifdef EMSCRIPTEN
    lastFileLoad++;
    fileLoadPromises[lastFileLoad] = p;
    EM_ASM_INT({
      var name = Module.Pointer_stringify($0);
      window.fgfxLoader.loadFile("assets/" + name).then(function(result) {
        var buffer = Module._malloc(result.length);
        Module.writeStringToMemory(result, buffer);
        Module._fsxx_fileLoaded($1, buffer, result.length);
        Module._free(buffer);
      }, function(error) {
        Module._fsxx_fileFailed($1);
      });
    }, path.c_str(), lastFileLoad);
#endif
    return p;
  }

#ifndef EMSCRIPTEN
  std::vector<std::thread> threads;
  bool finished;

  void loader() {
    while(!finished) {
      AssetLoadTask task;
      if(!assetLoadQueue.try_dequeue(task)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(23));
        continue;
      }
      auto& p = task.promise;
      std::string path = task.path;
#ifdef __ANDROID
      AAsset* asset = AAssetManager_open(fsxx::assetManager, path.c_str(), AASSET_MODE_UNKNOWN);
      if (NULL == asset) {
          __android_log_print(ANDROID_LOG_ERROR, "FSXX", "Image not found %s", path.c_str());
          p->reject(std::make_exception_ptr(FileNotFoundError(path)));
      } else {
        long size = AAsset_getLength(asset);
        std::string buffer = std::string(size, 0);
        AAsset_read (asset, (void*)buffer.data(), size);
        AAsset_close(asset);
        p->resolve(buffer);
      }
#else
      try {
        path = "assets/" + path;
        std::ifstream t(path);
        if(!t.is_open()) {
          throw FileNotFoundError(path);
        }
        std::string str((std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());
        p->resolve(str);
      } catch(...) {
        p->reject(std::current_exception());
      }
#endif
    }
  }
#endif

  void init(int loadThreads) {
#ifndef EMSCRIPTEN
    finished = false;
    for(int i = 0; i < loadThreads; i++) threads.push_back(std::thread(loader));
#endif
  }

  void destroy() {
#ifndef EMSCRIPTEN
    finished = true;
    for(auto& thread : threads) {
      if(thread.joinable()) thread.join();
    }
#endif
  }

}
