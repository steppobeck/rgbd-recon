#ifndef KINECT_CalibVolumes_H
#define KINECT_CalibVolumes_H

#include "calibration_volume.hpp"
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

class CalibVolumes{

public:
  CalibVolumes(std::vector<std::string> const& calib_volume_files, gloost::BoundingBox const& bbox);
  ~CalibVolumes();
  
  void setStartTextureUnit(unsigned start_texture_unit);
  void setStartTextureUnitInv(unsigned start_texture_unit);

  std::vector<int> getXYZVolumeUnits() const;
  std::vector<int> getUVVolumeUnits() const;

  std::vector<int> getXYZVolumeUnitsInv() const;

  void calculateInverseVolumes();
  void calculateInverseVolumes2();

  glm::uvec3 getVolumeRes() const;
  glm::fvec2 getDepthLimits(unsigned i) const;

  void writeInverseCalibs(std::string const& path) const;
  void loadInverseCalibs(std::string const& path);


private:
  void bindToTextureUnits();
  void bindToTextureUnitsInv();

  void createVolumeTextures();
  std::vector<sample_t> getXyzSamples(std::size_t i);

  std::vector<std::string> m_cv_xyz_filenames;
  std::vector<std::string> m_cv_uv_filenames;

  std::vector<globjects::Texture*> m_volumes_xyz;
  std::vector<globjects::Texture*> m_volumes_uv;

  std::vector<globjects::Texture*> m_volumes_xyz_inv;

  std::vector<CalibrationVolume<xyz>>    m_data_volumes_xyz;
  std::vector<CalibrationVolume<uv>>    m_data_volumes_uv;
  std::vector<CalibrationVolume<glm::fvec4>>    m_data_volumes_xyz_inv;

  gloost::BoundingBox m_bbox;

 protected:
  int m_start_texture_unit;
  int m_start_texture_unit_inv;

  void addVolume(std::string const& filename_xyz, std::string const& filename_uv);
};

}

#endif // #ifndef KINECT_CalibVolumes_H