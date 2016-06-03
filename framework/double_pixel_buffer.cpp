#include "double_pixel_buffer.hpp"

#include <glbinding/gl/enum.h>
#include <glbinding/gl/bitfield.h>
using namespace gl;
#include <globjects/Buffer.h>

namespace kinect {

double_pbo::double_pbo()
 :size{0}
 ,ptr{nullptr}
 ,front{nullptr}
 ,back{nullptr}
 ,dirty{false}
{}

double_pbo::double_pbo(std::size_t s)
 :size{s}
 ,ptr{nullptr}
 ,front{new globjects::Buffer()}
 ,back{new globjects::Buffer()}
 ,dirty{false}
{
  front->setData(size, nullptr, GL_DYNAMIC_DRAW);
  front->bind(GL_PIXEL_PACK_BUFFER);
  back->setData(size, nullptr, GL_DYNAMIC_DRAW);
  back->bind(GL_PIXEL_PACK_BUFFER);
  // unbind to prevent interference with downloads
  globjects::Buffer::unbind(GL_PIXEL_PACK_BUFFER);

  map();
}

globjects::Buffer const* double_pbo::get() {
  if(dirty) {
    swapBuffers();
  }
  return front.get();
}    

byte* double_pbo::pointer() {
  return ptr;
}

double_pbo& double_pbo::operator=(double_pbo&& pbo) {
  swap(pbo);
  return * this;
}

std::size_t size;
byte* ptr;
globjects::ref_ptr<globjects::Buffer> front;
globjects::ref_ptr<globjects::Buffer> back;
bool dirty;

void double_pbo::swapBuffers(){
  unmap();
  
  std::swap(front, back);

  map();

  dirty = false;
}

void double_pbo::swap(double_pbo& b) {
  std::swap(size, b.size);
  std::swap(ptr, b.ptr);
  std::swap(front, b.front);
  std::swap(back, b.back);
  std::swap(dirty, b.dirty);
}

void double_pbo::unmap() {
  back->unmap();
  ptr = nullptr;
}
void double_pbo::map() {
  ptr = (byte*)back->mapRange(0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
}

void swap(double_pbo& a, double_pbo& b) {
  a.swap(b);
}

}