#ifndef UNIT_CUBE_HPP
#define UNIT_CUBE_HPP

#include <globjects/Buffer.h>
#include <globjects/VertexArray.h>

// quad singleton
class UnitCube {
 public:
  static void draw();
  
 private:
  // prevent construction by user
  UnitCube();
  ~UnitCube();
  UnitCube(UnitCube const&) = delete;
  UnitCube& operator=(UnitCube const&) = delete;

  globjects::VertexArray*              m_quad;
  globjects::Buffer*                  m_tri_buffer;
};
// // get singleton sinstance
// UnitCube& configurator();

#endif //UNIT_CUBE_HPP