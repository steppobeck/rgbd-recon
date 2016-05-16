#ifndef KINECT_CalibVolumes_H
#define KINECT_CalibVolumes_H

#include <DataTypes.h>
#include "volume_sampler.hpp"
#include "nearest_neighbour_search.hpp"

#include <string>
#include <vector>

#include <globjects/Buffer.h>
#include <globjects/Texture.h>
#include <globjects/Program.h>

#include "gloost/BoundingBox.h"

namespace kinect{

template<typename T>
struct calib_volume_t {
  glm::uvec3 dimensions;
  glm::fvec2 depth_limits;
  std::vector<T> volume;

  calib_volume_t(std::string const& filename)
   :dimensions{0}
   ,depth_limits{0}
   ,volume{}
  {
    read(filename);
  }

  void read(std::string const& filename) {
    FILE* file_input = fopen( filename.c_str(), "rb");
    fread(&dimensions.x, sizeof(unsigned), 1, file_input);
    fread(&dimensions.y, sizeof(unsigned), 1, file_input);
    fread(&dimensions.z, sizeof(unsigned), 1, file_input);
    fread(&depth_limits.x, sizeof(float), 1, file_input);
    fread(&depth_limits.y, sizeof(float), 1, file_input);
    // std::vector<xyz> storage(dimensions.x * dimensions.y * dimensions.z);
    volume.resize(dimensions.x * dimensions.y * dimensions.z);
    fread(volume.data(), sizeof(T), dimensions.x * dimensions.y * dimensions.z, file_input);
    fclose(file_input);    
  }
};

class CalibVolumes{

public:
  CalibVolumes(std::vector<std::string> const& calib_volume_files, gloost::BoundingBox const& bbox);
  ~CalibVolumes();

  void reload();
  
  void setStartTextureUnit(unsigned start_texture_unit);
  void setStartTextureUnitInv(unsigned start_texture_unit);
  // unsigned getStartTextureUnit() const;

  std::vector<int> getXYZVolumeUnits() const;
  std::vector<int> getUVVolumeUnits() const;

  std::vector<int> getXYZVolumeUnitsInv() const;
  std::vector<int> getUVVolumeUnitsInv() const;

  void calculateInverseVolumes();
  void calculateInverseVolumes2();

  glm::uvec3 getVolumeRes() const;
  glm::fvec2 getDepthLimits(unsigned i) const;

private:
  void bindToTextureUnits();
  void bindToTextureUnitsInv();

  std::vector<sample_t> getXyzSamples(std::size_t i);

  std::vector<std::string> m_cv_xyz_filenames;
  std::vector<std::string> m_cv_uv_filenames;

  std::vector<globjects::Texture*> m_volumes_xyz;
  std::vector<globjects::Texture*> m_volumes_uv;

  std::vector<globjects::Texture*> m_volumes_xyz_inv;
  std::vector<globjects::Texture*> m_volumes_uv_inv;

  globjects::Program* m_program;
  VolumeSampler       m_sampler;

  std::vector<calib_volume_t<xyz>>    m_data_volumes_xyz2;
  // std::vector<std::vector<xyz>>    m_data_volumes_xyz;
  std::vector<std::vector<uv>>     m_data_volumes_uv;
  std::vector<std::vector<glm::fvec4>>     m_data_volumes_xyz_inv;

  gloost::BoundingBox m_bbox;

  std::vector<float> m_cv_min_ds;
  std::vector<float> m_cv_max_ds;
  std::vector<unsigned> m_cv_widths;
  std::vector<unsigned> m_cv_heights;
  std::vector<unsigned> m_cv_depths;

 protected:
  int m_start_texture_unit;
  int m_start_texture_unit_inv;

  void addVolume(std::string const& filename_xyz, std::string filename_uv);
};

}

#endif // #ifndef KINECT_CalibVolumes_H