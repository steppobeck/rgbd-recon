#ifndef VOLUME_SAMPLER_HPP
#define VOLUME_SAMPLER_HPP

#include <glm/gtc/type_precision.hpp>

#include <globjects/Buffer.h>
#include <globjects/VertexArray.h>

class VolumeSampler {
 public:
  VolumeSampler(glm::uvec3 dimensions);
  ~VolumeSampler();
  
  void sample();
  
 private:
  glm::uvec3              m_dimensions;
  globjects::VertexArray* m_va_samples;
  globjects::Buffer*      m_buffer_samples;
};

#endif //VOLUME_SAMPLER_HPP