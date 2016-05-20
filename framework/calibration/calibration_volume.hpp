#ifndef CALIB_VOLUME_HPP
#define CALIB_VOLUME_HPP

#include <glm/gtc/type_precision.hpp>

#include <cstdio>
#include <vector>
#include <string>
#include <iostream>

namespace kinect {

template<typename T>
class CalibrationVolume {
 public:
  CalibrationVolume(std::string const& filename)
   :m_resolution{0}
   ,m_depth_limits{0}
   ,m_volume{}
  {
    read(filename);
  }

  CalibrationVolume(glm::uvec3 const& res, glm::fvec2 const& depth, std::vector<T> const& vol)
   :m_resolution{res}
   ,m_depth_limits{depth}
   ,m_volume{vol}
  {}
  
  void write(std::string const& filename) const {
    FILE* file_output = fopen(filename.c_str(), "wb");
    fwrite(&m_resolution.x, sizeof(unsigned), 1, file_output);
    fwrite(&m_resolution.y, sizeof(unsigned), 1, file_output);
    fwrite(&m_resolution.z, sizeof(unsigned), 1, file_output);
    fwrite(&m_depth_limits.x, sizeof(float), 1, file_output);
    fwrite(&m_depth_limits.y, sizeof(float), 1, file_output);
    fwrite(m_volume.data(), sizeof(T), m_volume.size(), file_output);
    fclose(file_output);
  }

  glm::uvec3 const& res() const {
    return m_resolution;
  }

  glm::fvec2 const& depthLimits() const {
    return m_depth_limits;
  }

  std::size_t numVoxels() const {
    return m_resolution.x * m_resolution.y * m_resolution.z;
  }

  std::vector<T> const& volume() const {
    return m_volume;
  }

  T const& operator()(unsigned x, unsigned y, unsigned z) const {
    return m_volume[z * m_resolution.x * m_resolution.y + y * m_resolution.x + x];
  }

 private:
  void read(std::string const& filename) {
    FILE* file_input = fopen(filename.c_str(), "rb");
    std::size_t res = 0;
    res = fread(&m_resolution.x, sizeof(unsigned), 1, file_input);
    assert(res == 1);
    res = fread(&m_resolution.y, sizeof(unsigned), 1, file_input);
    assert(res == 1);
    res = fread(&m_resolution.z, sizeof(unsigned), 1, file_input);
    assert(res == 1);
    res = fread(&m_depth_limits.x, sizeof(float), 1, file_input);
    assert(res == 1);
    res = fread(&m_depth_limits.y, sizeof(float), 1, file_input);
    assert(res == 1);
    m_volume.resize(m_resolution.x * m_resolution.y * m_resolution.z);
    res = fread(m_volume.data(), sizeof(T), m_volume.size(), file_input);
    assert(res == m_resolution.x * m_resolution.y * m_resolution.z);
    fclose(file_input);    
  }

  glm::uvec3 m_resolution;
  glm::fvec2 m_depth_limits;
  std::vector<T> m_volume;
};

}
#endif