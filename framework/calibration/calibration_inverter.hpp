#ifndef CALIBRATION_INVERTER_HPP
#define CALIBRATION_INVERTER_HPP

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

class CalibrationInverter{

public:
  CalibrationInverter(std::vector<std::string> const& calib_volume_files, gloost::BoundingBox const& bbox);

  void calculateInverseVolumes(glm::uvec3 const& volume_res);

  void writeInverseVolumes(std::string const& path) const;

private:
  std::vector<sample_t> getXyzSamples(std::size_t i);

  std::vector<std::string> m_cv_xyz_filenames;

  std::vector<CalibrationVolume<xyz>>    m_data_volumes_xyz;
  std::vector<CalibrationVolume<uv>>    m_data_volumes_uv;
  std::vector<CalibrationVolume<glm::fvec4>>    m_data_volumes_xyz_inv;

  gloost::BoundingBox m_bbox;

 protected:
  void addVolume(std::string const& filename_xyz);
};

}

#endif // #ifndef CALIBRATION_INVERTER_HPP