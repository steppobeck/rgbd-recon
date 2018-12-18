#include "calibration_inverter.hpp"
#include <KinectCalibrationFile.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>

#include <stdexcept>

namespace kinect{

CalibrationInverter::CalibrationInverter(std::vector<std::string> const& calib_volume_files, gloost::BoundingBox const& bbox)
 :m_cv_xyz_filenames()
 ,m_frustums{}
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
        calib_samples.emplace_back(calib(x,y,z), glm::uvec3{x, y, z});
      }
    }
  }

  return calib_samples;
}

static glm::fvec3 inverseDistance(glm::fvec3 const& curr_point, std::vector<sample_t> const& neighbours){
  //std::cout << "begin inverseDistance for world position: (" << curr_point.x << ", " << curr_point.y << ", " << curr_point.z << ")" << std::endl;
  float total_weight = 0.0f;
  glm::fvec3 weighted_index{0.0f};
  for(auto const& sample : neighbours) {
    //std::cout << "sample.index: (" << sample.index.x << ", " << sample.index.y <<  ", " << sample.index.z << ") sample.pos: (" << sample.pos.x << ", " <<sample.pos.y  << ", " << sample.pos.z << ")"<< std::endl;
    float weight = 1.0f / glm::distance(curr_point, sample.pos);
    weighted_index += weight * glm::fvec3{sample.index};
    total_weight += weight;
  }
  weighted_index /= total_weight;
  //std::cout << "resulting in weighted index: (" << weighted_index.x << ", " << weighted_index.y << ", " << weighted_index.z << ")" << std::endl;
  //std::cout << "end inverseDistance" << std::endl;
  return weighted_index;
}

#if 0
static glm::fvec3 calcuateDepthToWorldCoordinate(glm::fvec3 const& posW, sample_t const& nearestD, const CalibrationVolume<xyz>& calib){
  //std::cout << "posW: (" << posW.x << ", " << posW.y << ", " << posW.z << ")" << std::endl;

  // 1. decision along x coordinate
  const glm::uvec3 xA(nearestD.index);

  const glm::uvec3 x_l(xA.x > 0 ? xA.x - 1 : xA.x, xA.y, xA.z);
  const glm::uvec3 x_r(xA.x < (calib.res().x - 1) ? xA.x + 1 : xA.x, xA.y, xA.z);
  const auto pos_x_l = calib(x_l.x, x_l.y, x_l.z);
  const auto pos_x_r = calib(x_r.x, x_r.y, x_r.z);
  const glm::uvec3 xB = glm::distance(glm::fvec3(pos_x_l.x, pos_x_l.y, pos_x_l.z), posW) < glm::distance(glm::fvec3(pos_x_r.x, pos_x_r.y, pos_x_r.z), posW) ? x_l : x_r;



  const auto posW_test = getTrilinear((xyz*) calib.volume().data() , calib.res().x, calib.res().y, calib.res().z, xA.x + 0.5 , xA.y + 0.5, xA.z + 0.5);
  std::cout << "posW_test: (" << posW_test.x << ", " << posW_test.y << ", " << posW_test.z << ")" << std::endl;
  std::cout << std::endl;

  glm::fvec3 posD(nearestD.index);
  std::cout << "posD: (" << posD.x << ", " << posD.y << ", " << posD.z << ")" << std::endl;
  auto const& posWtmp = calib(unsigned(posD.x), unsigned(posD.y), unsigned(posD.z));
  std::cout << "posWtmp: (" << posWtmp.x << ", " << posWtmp.y << ", " << posWtmp.z << ")" << std::endl;
  std::cout << "end calcuateDepthToWorldCoordinate" << std::endl;
  return posD;
}
#endif

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
    auto const& calib = m_data_volumes_xyz[i];
    #pragma omp parallel for
    for(unsigned x = 0; x < volume_res.x; ++x) {
      for(unsigned y = 0; y < volume_res.y; ++y) {
        for(unsigned z = 0; z < volume_res.z; ++z) {
          glm::fvec3 sample_pos = sample_start + glm::fvec3{x,y,z} * sample_step;
          // invalidate if point is not visible from camera
          if (!m_frustums[i].inside(sample_pos)) {
            curr_volume_inv[z * volume_res.x * volume_res.y + y * volume_res.x + x] = glm::fvec4{-1.0f};
            continue;
          }

          
          // old
          auto samples = curr_calib_search.search(sample_pos, 8);
          auto weighted_index = inverseDistance(sample_pos, samples);
#if 0
          // new
          auto nearestD = curr_calib_search.search(sample_pos, 1)[0];
          auto weighted_index = calcuateDepthToWorldCoordinate(sample_pos, nearestD, calib);
#endif
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

static std::array<glm::fvec3, 8> getCornerPoints(CalibrationVolume<xyz> const& curr_volume) {
  glm::uvec3 end_points{curr_volume.res() - glm::uvec3{1}};
  std::array<glm::fvec3, 8> points_corner{};
  // 
  points_corner[0] = (curr_volume(0,          0,          0));
  points_corner[1] = (curr_volume(0,          end_points.y, 0));
  points_corner[2] = (curr_volume(end_points.x, end_points.y, 0));
  points_corner[3] = (curr_volume(end_points.x, 0,          0));

  points_corner[4] = (curr_volume(0,          0,          end_points.z));
  points_corner[5] = (curr_volume(0,          end_points.y, end_points.z));
  points_corner[6] = (curr_volume(end_points.x, end_points.y, end_points.z));
  points_corner[7] = (curr_volume(end_points.x, 0,          end_points.z));

  return points_corner;
}

void CalibrationInverter::addVolume(std::string const& filename_xyz) {
  std::cerr << "loading " << filename_xyz << std::endl;
  m_data_volumes_xyz.emplace_back(filename_xyz);
  auto const& calib_xyz(m_data_volumes_xyz.back());
  std::cout << "dimensions xyz - " << calib_xyz.res().x << ", " << calib_xyz.res().y << ", " << calib_xyz.res().z 
            << " minmax d - " << calib_xyz.depthLimits().x << ", " << calib_xyz.depthLimits().y << std::endl;
            
  m_frustums.emplace_back(getCornerPoints(calib_xyz));
}

}