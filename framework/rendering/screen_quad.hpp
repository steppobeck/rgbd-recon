#ifndef SCREEN_QUAD_HPP
#define SCREEN_QUAD_HPP

#include <globjects/Buffer.h>
#include <globjects/VertexArray.h>

// quad singleton
class ScreenQuad {
 public:
  static void draw();
  
 private:
  // prevent construction by user
  ScreenQuad();
  ~ScreenQuad();
  ScreenQuad(ScreenQuad const&) = delete;
  ScreenQuad& operator=(ScreenQuad const&) = delete;

  globjects::VertexArray*              m_quad;
  globjects::Buffer*                  m_tri_buffer;
};
// // get singleton sinstance
// ScreenQuad& configurator();

#endif //SCREEN_QUAD_HPP