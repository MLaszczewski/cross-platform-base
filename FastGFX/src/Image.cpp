//
// Created by Michał Łaszczewski on 09/12/16.
//


#ifndef EMSCRIPTEN ///TODO: implemented with javascript

#include "fastgfx.h"
#include "Image.h"
#include <fsxx.h>

namespace fgfx {
  extern bool finished;

  void ReadDataFromInputStream(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    if(io_ptr == NULL)
      throw PngError();
    std::istream& stream = *(std::istream*)io_ptr;
    stream.read((char*)outBytes, byteCountToRead);
  }
  void parseRGB(std::shared_ptr<Image> image, const png_structp& png_ptr, const png_infop& info_ptr) {
    int w = image->width;
    int h = image->height;
    Color* data = image->data;

    const png_uint_32 bytesPerRow = png_get_rowbytes(png_ptr, info_ptr);
    png_bytep src_row = new png_byte[bytesPerRow];
    for(int y = 0; y < h; y++) {
      png_read_row(png_ptr, src_row, NULL);
      Color* dest_row = data + w * y;
      for(int x = 0; x < w; x++) {
        memcpy(dest_row + x, src_row + (x * 3), 3);
        dest_row[x].a = 255;
      }
    }
  }
  void parseRGBA(std::shared_ptr<Image> image, const png_structp& png_ptr, const png_infop& info_ptr) {
    int w = image->width;
    int h = image->height;
    Color* data = image->data;
    const png_uint_32 bytesPerRow = png_get_rowbytes(png_ptr, info_ptr);
    png_bytep src_row = new png_byte[bytesPerRow];
    for(int y = 0; y < h; y++) {
      png_read_row(png_ptr, src_row, NULL);
      Color* dest_row = data + w * y;
      memcpy(dest_row, src_row, 4 * w);
    }
  }
  static void png_error_handler(png_structp png_ptr, png_const_charp msg) {
    fprintf(stderr, "readpng2 libpng error: %s\n", msg);
    fflush(stderr);
  }

  std::shared_ptr<Image> decodePngImage(std::string data) {
    std::istringstream stream(data);
    unsigned int sig_read = 0;

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, png_error_handler, NULL);
    if(png_ptr == NULL) throw PngError();
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(info_ptr == NULL) {
      png_destroy_read_struct(&png_ptr, NULL, NULL);
      throw PngError();
    }
    if (setjmp(png_jmpbuf(png_ptr))) {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      throw PngError();
    }
    png_set_read_fn(png_ptr, &stream, ReadDataFromInputStream);
    png_set_sig_bytes(png_ptr, sig_read);
    png_read_info(png_ptr, info_ptr);
    png_uint_32 width = 0;
    png_uint_32 height = 0;
    int bitDepth = 0;
    int colorType = -1;
    png_uint_32 retval = png_get_IHDR(png_ptr, info_ptr, &width, &height, &bitDepth, &colorType, NULL, NULL, NULL);
    if(retval != 1) throw new PngError();

    if (colorType == PNG_COLOR_TYPE_PALETTE)
      png_set_expand(png_ptr);
    if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
      png_set_expand(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
      png_set_expand(png_ptr);
    if (colorType == PNG_COLOR_TYPE_GRAY ||
        colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
      png_set_expand_gray_1_2_4_to_8(png_ptr);

    png_read_update_info (png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bitDepth, &colorType, NULL, NULL, NULL);

    std::shared_ptr<Image> image = std::make_shared<Image>(width, height);
    switch(colorType)
    {
      case PNG_COLOR_TYPE_RGB:
        parseRGB(image, png_ptr, info_ptr);
        break;
      case PNG_COLOR_TYPE_RGB_ALPHA:
        parseRGBA(image, png_ptr, info_ptr);
        break;
      default:
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        throw new PngError();
    }
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return image;
  }

  Image::Image(int w, int h) {
    width = w;
    height = h;
    data = new Color[w*h];
  }
  Image::~Image() {
    delete[] data;
  }
  void Image::clear() {
    memset(data, 0,  width*height*sizeof(Color));
  }
  void Image::clear(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    Color* p = data;
    Color* e = data + (width * height);
    while(p < e) {
      p->r = r;
      p->g = g;
      p->b = b;
      p->a = a;
      p++;
    }
  }

  void Image::clearRect(int xmin, int ymin, int xmax, int ymax,
                        unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    for(int y = ymin; y <= ymax; y++)
      for(int x = xmin; x <= xmax; x++) {
        Color& pixel = data[y * width + x];
        pixel.r = r;
        pixel.g = g;
        pixel.b = b;
        pixel.a = a;
      }
  }

  void Image::put(std::shared_ptr<Image> src, int x, int y) {
    int sw = src->width;
    int sh = src->height;
    Color* src_data = src->data;
    Color* dest_data = data;
    int dw = width;
    //int dh = height;
    for (int sy = 0; sy < sh; sy++) {
      for (int sx = 0; sx < sw; sx++) {
        int dx = x + sx;
        int dy = y + sy;
        dest_data[dy * dw + dx] = src_data[sy * sw + sx];
      }
    }
  }

  std::shared_ptr<promise::Promise<std::shared_ptr<Image>>> loadPng(std::string path) {
    return fsxx::loadFile(path)->then<std::shared_ptr<Image>>([](std::string data) -> std::shared_ptr<Image>{
      return decodePngImage(data);
    });
  }
}

#endif