#include "calibration_inverter.hpp"
#include <KinectCalibrationFile.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>

#include <stdexcept>

namespace kinect{

CalibrationInverter::CalibrationInverter(std::vector<std::string> const& calib_volume_files, gloost::BoundingBox const& bbox)
 :m_cv_xyz_filenames()
 ,m_bbox{bbox}
{
  for(auto const& calib_file : calib_volume_files){
  	std::string basefile = calib_file;
  	basefile.replace( basefile.end() - 3, basefile.end(), "");
  	m_cv_xyz_filenames.push_back(basefile + "cv_xyz");
  }

  for(unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i){
    addVolume(m_cv_xyz_filenames[i]);
  }
}


void CalibrationInverter::writeInverseVolumes(std::string const& path) const {
  for(unsigned i = 0; i < m_data_volumes_xyz_inv.size(); ++i) {
    std::string name_source{ m_cv_xyz_filenames[i].substr( m_cv_xyz_filenames[i].find_last_of("/\\") + 1)};
    std::string name_output{path + name_source + "_inv"};
    std::cout << "writing to file " << name_output << std::endl;
    m_data_volumes_xyz_inv[i].write(name_output);
  }
}

std::vector<sample_t> CalibrationInverter::getXyzSamples(std::size_t i) {
  std::vector<sample_t> calib_samples{};
  auto const& calib = m_data_volumes_xyz[i];
  calib_samples.reserve(calib.numVoxels());
  glm::uvec3 const& dims{calib.res()};

  for(unsigned x = 0; x < dims.x; ++x) {
    for(unsigned y = 0; y < dims.y; ++y) {
      for(unsigned z = 0; z < dims.z; ++z) {
        calib_samples.emplace_back(calib.volume()[z * dims.x * dims.y + y * dims.x + x], glm::uvec3{x, y, z});
      }
    }
  }

  return calib_samples;
}

static glm::fvec3 inverseDistance(glm::fvec3 const& curr_point, std::vector<sample_t> const& neighbours){
  float total_weight = 0.0f;
  glm::fvec3 weighted_index{0.0f};
  for(auto const& sample : neighbours) {
    float weight = 1.0f / glm::distance(curr_point, sample.pos);
    weighted_index += weight * glm::fvec3{sample.index};
    total_weight += weight;
  }
  weighted_index /= total_weight;
  return weighted_index;
}

void CalibrationInverter::calculateInverseVolumes(glm::uvec3 const& volume_res) {
  glm::fvec3 bbox_dimensions = glm::fvec3{m_bbox.getPMax()[0] - m_bbox.getPMin()[0],
                                          m_bbox.getPMax()[1] - m_bbox.getPMin()[1],
                                          m_bbox.getPMax()[2] - m_bbox.getPMin()[2]};
  glm::fvec3 bbox_translation = glm::fvec3{m_bbox.getPMin()[0], m_bbox.getPMin()[1], m_bbox.getPMin()[2]};

  glm::fvec3 volume_step{glm::fvec3{1.0f} / glm::fvec3{volume_res}};
  glm::fvec3 sample_step{bbox_dimensions * volume_step};
  // important, start with offset of a half voxel
  glm::fvec3 sample_start = bbox_translation + sample_step * 0.5f;
  glm::fvec3 sample_pos = sample_start;

  for(unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i) {
    glm::fvec3 curr_calib_dims{m_data_volumes_xyz[i].res()};
    auto curr_calib_samples(getXyzSamples(i));
    std::cout << "building nn search structure " << i << std::endl;
    NearestNeighbourSearch curr_calib_search{curr_calib_samples}; 
    std::cout << "start neighbour search" << std::endl;

    std::vector<glm::fvec4> curr_volume_inv(volume_res.x * volume_res.y * volume_res.z, glm::fvec4{-1.0f});
    #pragma omp parallel for
    for(unsigned x = 0; x < volume_res.x; ++x) {
      for(unsigned y = 0; y < volume_res.y; ++y) {
        for(unsigned z = 0; z < volume_res.z; ++z) {
          glm::fvec3 sample_pos = sample_start + glm::fvec3{x,y,z} * sample_step; 
          auto samples = curr_calib_search.search(sample_pos, 8);
          auto weighted_index = inverseDistance(sample_pos, samples);
          curr_volume_inv[z * volume_res.x * volume_res.y + y * volume_res.x + x] = glm::fvec4{(weighted_index + glm::fvec3{0.5f}) / curr_calib_dims, 1.0f};

          sample_pos.z += sample_step.z;
        }
        // std::cout << "y slice " << y << " done" << std::endl;
        sample_pos.y += sample_step.y;
        sample_pos.z = sample_start.z;
      }
      // std::cout << "x slice " << x << " done" << std::endl;
      sample_pos.x += sample_step.x;
      sample_pos.y = sample_start.y;
    }
    m_data_volumes_xyz_inv.emplace_back(volume_res, glm::fvec2{0.5f, 4.5f}, curr_volume_inv);
  }
}

void CalibrationInverter::addVolume(std::string const& filename_xyz) {
  std::cerr << "loading " << filename_xyz << std::endl;
  m_data_volumes_xyz.emplace_back(filename_xyz);
  auto const& calib_xyz(m_data_volumes_xyz.back());
  std::cout << "dimensions xyz - " << calib_xyz.res().x << ", " << calib_xyz.res().y << ", " << calib_xyz.res().z 
            << " minmax d - " << calib_xyz.depthLimits().x << ", " << calib_xyz.depthLimits().y << std::endl;
}

}