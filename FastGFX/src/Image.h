//
// Created by Michał Łaszczewski on 09/12/16.
//

#ifndef FGFX_IMAGE_H
#define FGFX_IMAGE_H

#ifndef __ANDROID
#include <string>
#include <fstream>
#include <streambuf>
#endif

#include <png.h>
#include <memory.h>
#include <sstream>
#include <Promise.h>

namespace fgfx {

  struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    Color(unsigned char rp, unsigned char gp, unsigned char bp, unsigned char ap) {
      r = rp;
      g = gp;
      b = bp;
      a = ap;
    }
    Color() {
      r = 0;
      g = 0;
      b = 0;
      a = 0;
    }
  };
  class Image {
  public:
    int width;
    int height;
    Color* data;

    Image(int w, int h);
    ~Image();
    void clear();
    void clear(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void clearRect(int xmin, int ymin, int xmax, int ymax,
                   unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void put(std::shared_ptr<Image> that, int x, int y);
    Color& at(int x, int y) { return data[x + y * width]; }
  };

  class PngError : public std::exception {
  };

  class ImageNotFoundError : public std::exception {
  };

  std::shared_ptr<promise::Promise<std::shared_ptr<Image>>> loadPng(std::string path);
}

#endif //FGFX_IMAGE_H
