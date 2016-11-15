#include "unit_cube.hpp"

#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
using namespace gl;

#include <globjects/Buffer.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>

#include <glm/gtc/type_precision.hpp>

#include <vector>

UnitCube::UnitCube()
 :m_cube{new globjects::VertexArray()}
 ,m_tri_buffer{new globjects::Buffer()}
 ,m_index_buffer{new globjects::Buffer()}
{
  std::vector<float> vertices{
    1.0f, 1.0f, 1.0f, // front, up, right
    0.0f, 1.0f, 1.0f, // front, back, right
    1.0f, 1.0f, 0.0f, // right, up, back
    0.0f, 1.0f, 0.0f, // left, up, back
    1.0f, 0.0f, 1.0f, // right, down, front
    0.0f, 0.0f, 1.0f, // left, down, front
    0.0f, 0.0f, 0.0f, // left, down, back
    1.0f, 0.0f, 0.0f //right down back
  };

  m_tri_buffer->setData(vertices, GL_STATIC_DRAW);

  m_cube->enable(0);
  m_cube->binding(0)->setAttribute(0);
  m_cube->binding(0)->setBuffer(m_tri_buffer, 0, sizeof(float) * 3);
  m_cube->binding(0)->setFormat(3, GL_FLOAT);
  
  // m_index_buffer->bind(GL_ELEMENT_ARRAY_BUFFER);
  // m_index_buffer->setData(indices, GL_STATIC_DRAW);
}

void UnitCube::draw() {
  static UnitCube instance{};
  static unsigned num_indices = 14;
  static std::vector<GLubyte> indices {
    3, 2, 6, 7, 4, 2, 0,
    3, 1, 6, 5, 4, 1, 0
  };
  // instance.m_cube->drawElements(GL_TRIANGLE_STRIP, num_indices, GL_UNSIGNED_BYTE, nullptr);
  instance.m_cube->drawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_BYTE, indices.data());
}

void UnitCube::drawInstanced(unsigned count) {
  static UnitCube instance{};
  static unsigned num_indices = 14;
  static std::vector<GLubyte> indices {
    3, 2, 6, 7, 4, 2, 0,
    3, 1, 6, 5, 4, 1, 0
  };
  // instance.m_cube->drawElements(GL_TRIANGLE_STRIP, num_indices, GL_UNSIGNED_BYTE, nullptr);
  instance.m_cube->drawElementsInstanced(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_BYTE, indices.data(), count);
}

void UnitCube::drawWire() {
  static UnitCube instance{};
  static std::vector<GLubyte> indices {
    0,1, 0,2, 0,4,
    5,1, 5,4, 5,6,
    3,1, 3,6, 3,2,
    7,2, 7,4, 7,6
  };
  instance.m_cube->drawElements(GL_LINES, indices.size(), GL_UNSIGNED_BYTE, indices.data());
}
void UnitCube::drawWireInstanced(unsigned count) {
  static UnitCube instance{};
  static std::vector<GLubyte> indices {
    0,1, 0,2, 0,4,
    5,1, 5,4, 5,6,
    3,1, 3,6, 3,2,
    7,2, 7,4, 7,6
  };
  instance.m_cube->drawElementsInstanced(GL_LINES, indices.size(), GL_UNSIGNED_BYTE, indices.data(), count);
}