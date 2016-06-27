#include "volume_sampler.hpp"

#include <glbinding/gl/enum.h>
using namespace gl;
#include <globjects/Buffer.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>

VolumeSampler::VolumeSampler(glm::uvec3 const& dimensions)
 :m_dimensions{dimensions}
 ,m_va_samples{new globjects::VertexArray()}
 ,m_buffer_samples{new globjects::Buffer()}
{
  float stepX = 1.0f / m_dimensions.x;
  float stepY = 1.0f / m_dimensions.y;
  float stepZ = 1.0f / m_dimensions.z;
  for(unsigned y = 0; y < m_dimensions.y; ++y) {
    for(unsigned x = 0; x < m_dimensions.x; ++x) {
      for(unsigned z = 0; z < m_dimensions.z; ++z) {
        m_pos_voxels.emplace_back(( x+ 0.5f) * stepX, (y + 0.5f) * stepY, (z + 0.5f) * stepZ);
      }
    }
  }

  m_buffer_samples->setData(m_pos_voxels, GL_STATIC_DRAW);

  m_va_samples->enable(0);
  m_va_samples->binding(0)->setAttribute(0);
  m_va_samples->binding(0)->setBuffer(m_buffer_samples, 0, sizeof(float) * 3);
  m_va_samples->binding(0)->setFormat(3, GL_FLOAT);
}

void VolumeSampler::resize(glm::uvec3 const& dimensions) {
  m_pos_voxels.clear();
  m_dimensions = dimensions;
  float stepX = 1.0f / m_dimensions.x;
  float stepY = 1.0f / m_dimensions.y;
  float stepZ = 1.0f / m_dimensions.z;
  for(unsigned y = 0; y < m_dimensions.y; ++y) {
    for(unsigned x = 0; x < m_dimensions.x; ++x) {
      for(unsigned z = 0; z < m_dimensions.z; ++z) {
        m_pos_voxels.emplace_back(( x+ 0.5f) * stepX, (y + 0.5f) * stepY, (z + 0.5f) * stepZ);
      }
    }
  }
  m_buffer_samples->setData(m_pos_voxels, GL_STATIC_DRAW);
  m_va_samples->binding(0)->setBuffer(m_buffer_samples, 0, sizeof(float) * 3);
}

void VolumeSampler::sample() const {
  m_va_samples->drawArrays(GL_POINTS, 0, m_dimensions.x * m_dimensions.y * m_dimensions.z);
}
void VolumeSampler::sample(std::vector<unsigned> const& indices) const {
  m_va_samples->drawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, indices.data());
}

std::vector<glm::fvec3> const& VolumeSampler::voxelPositions() const {
  return m_pos_voxels;
}
