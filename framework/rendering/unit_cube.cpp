#include "unit_cube.hpp"

#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
using namespace gl;
#include <globjects/VertexAttributeBinding.h>
#include <glm/gtc/type_precision.hpp>

#include <vector>

UnitCube::UnitCube()
 :m_quad{new globjects::VertexArray()}
 ,m_tri_buffer{new globjects::Buffer()}
{
  std::vector<float> vertices{
     1.0f,  1.0f,  1.0f, // front, up, right
    0.0f,  1.0f,  1.0f, // front, back, right
     1.0f,  1.0f, 0.0f, // right, up, back
    0.0f,  1.0f, 0.0f, // left, up, back
     1.0f, 0.0f,  1.0f, // right, down, front
    0.0f, 0.0f,  1.0f, // left, down, front
    0.0f, 0.0f, 0.0f, // left, down, back
     1.0f, 0.0f, -0.0f //right down back
  };

  m_tri_buffer->setData(vertices, GL_STATIC_DRAW);

  m_quad->enable(0);
  m_quad->binding(0)->setAttribute(0);
  m_quad->binding(0)->setBuffer(m_tri_buffer, 0, sizeof(float) * 3);
  m_quad->binding(0)->setFormat(3, GL_FLOAT);

  m_quad->ref();
  m_tri_buffer->ref();
}

UnitCube::~UnitCube() {
  // if destroyed before context, free resources
  m_quad->unref();
  m_tri_buffer->unref();
}

void UnitCube::draw() {
  static UnitCube instance{};
  static std::vector<GLubyte> indices {
    3, 2, 6, 7, 4, 2, 0,
    3, 1, 6, 5, 4, 1, 0
  };
  glDisable(GL_CULL_FACE);
  instance.m_quad->drawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_BYTE, indices.data());
  glEnable(GL_CULL_FACE);
}