#ifndef DOUBLE_BUFFER_HPP
#define DOUBLE_BUFFER_HPP

#include <algorithm>

template<typename T>
struct double_buffer{
  double_buffer(T const& f, T const& b)
   :front{f}
   ,back{b}
   ,dirty{false}
  {}

  T const& get() const{
    if(dirty) {
      swapBuffers();
    }
    return front;
  }    

  // T& back() {
  //   return ptr;
  // }

  T front;
  T back;
  bool dirty;

  void swapBuffers(){
    std::swap(front, back);
    dirty = false;
  }
};

#endif