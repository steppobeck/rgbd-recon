#ifndef VOLUME_SAMPLER_HPP
#define VOLUME_SAMPLER_HPP

#include <glm/gtc/type_precision.hpp>

#include <globjects/base/ref_ptr.h>
namespace globjects {
  class Buffer;
  class VertexArray;
}

#include <vector>

class VolumeSampler {
 public:
  VolumeSampler(glm::uvec3 const& dimensions);
  
  void resize(glm::uvec3 const& dimensions);
  void sample() const;
  void sample(std::vector<unsigned> const& indices) const;
  std::vector<glm::fvec3> const& voxelPositions() const;
 private:
  glm::uvec3              m_dimensions;
  globjects::ref_ptr<globjects::VertexArray> m_va_samples;
  globjects::ref_ptr<globjects::Buffer>      m_buffer_samples;
  std::vector<glm::fvec3> m_pos_voxels;
};

#endif //VOLUME_SAMPLER_HPP