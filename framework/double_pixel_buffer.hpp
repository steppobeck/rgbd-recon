#ifndef DOUBLE_PIXEL_BUFFER_HPP
#define DOUBLE_PIXEL_BUFFER_HPP

#include "DataTypes.h"

#include <globjects/base/ref_ptr.h>
namespace globjects {
  class Buffer;
}

namespace kinect{

  struct double_pbo{
    double_pbo();
    double_pbo(std::size_t s);

    globjects::Buffer const* get();
    byte* pointer();
    double_pbo& operator=(double_pbo&& pbo);

    std::size_t size;
    byte* ptr;
    globjects::ref_ptr<globjects::Buffer> front;
    globjects::ref_ptr<globjects::Buffer> back;
    bool dirty;

    void swapBuffers();
    void swap(double_pbo& b);

  private:
    void unmap();
    void map();
  };

void swap(double_pbo& a, double_pbo& b);

}

#endif