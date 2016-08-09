#ifndef UNIT_CUBE_HPP
#define UNIT_CUBE_HPP

#include <globjects/base/ref_ptr.h>
namespace globjects {
  class Buffer;
  class VertexArray;
}

// quad singleton
class UnitCube {
 public:
  static void draw();
  static void drawInstanced(unsigned count);
  static void drawWire();
  static void drawWireInstanced(unsigned count);
  
 private:
  // prevent construction by user
  UnitCube();
  UnitCube(UnitCube const&) = delete;
  UnitCube& operator=(UnitCube const&) = delete;

  globjects::ref_ptr<globjects::VertexArray> m_cube;
  globjects::ref_ptr<globjects::Buffer>      m_tri_buffer;
  globjects::ref_ptr<globjects::Buffer>      m_index_buffer;
};

#endif //UNIT_CUBE_HPP