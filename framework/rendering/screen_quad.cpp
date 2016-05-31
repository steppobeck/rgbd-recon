#include "screen_quad.hpp"

#include <glbinding/gl/enum.h>
using namespace gl;
#include <globjects/VertexAttributeBinding.h>

ScreenQuad::ScreenQuad()
 :m_quad{new globjects::VertexArray()}
 ,m_tri_buffer{new globjects::Buffer()}
{
  std::vector<float> data = {
    -1.0f, -1.0f, 0.0f, 0.0f,
     3.0f, -1.0f, 2.0f, 0.0f,
    -1.0f, 3.0f, 0.0f, 2.0f
  };

  m_tri_buffer->setData(data, GL_STATIC_DRAW);

  m_quad->enable(0);
  m_quad->binding(0)->setAttribute(0);
  m_quad->binding(0)->setBuffer(m_tri_buffer, 0, sizeof(float) * 4);
  m_quad->binding(0)->setFormat(2, GL_FLOAT);
  m_quad->enable(1);
  m_quad->binding(1)->setAttribute(1);
  m_quad->binding(1)->setBuffer(m_tri_buffer, sizeof(float) * 2, sizeof(float) * 4);
  m_quad->binding(1)->setFormat(2, GL_FLOAT);
  // register to destroy at context destruction
  m_quad->ref();
  m_tri_buffer->ref();
}

void ScreenQuad::draw() {
  static ScreenQuad instance{};
  instance.m_quad->drawArrays(GL_TRIANGLES, 0, 3);
}