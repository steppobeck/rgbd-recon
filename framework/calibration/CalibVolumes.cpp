#include "CalibVolumes.hpp"
#include <KinectCalibrationFile.h>
#include <timevalue.h>

#include <glbinding/gl/functions-patches.h>
#include <globjects/Shader.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>

#include <stdexcept>

namespace kinect{

static glm::uvec3 volume_res{128,256,128};
static int start_image_unit = 1;

CalibVolumes::CalibVolumes(std::vector<std::string> const& calib_volume_files, gloost::BoundingBox const& bbox)
 :m_cv_xyz_filenames()
 ,m_cv_uv_filenames()
 ,m_volumes_xyz{}
 ,m_volumes_uv{}
 ,m_volumes_xyz_inv{}
 // ,m_program{new globjects::Program()}
 // ,m_sampler{volume_res}
 ,m_bbox{bbox}
 ,m_start_texture_unit(-1)
 ,m_start_texture_unit_inv(-1)
{
  for(auto const& calib_file : calib_volume_files){
  	std::string basefile = calib_file;
  	basefile.replace( basefile.end() - 3, basefile.end(), "");
  	m_cv_xyz_filenames.push_back(basefile + "cv_xyz");
  	m_cv_uv_filenames.push_back(basefile + "cv_uv");
  }

  // m_program->attach(
  //   globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/calib_sample.vs"),
  //   globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/calib_vis.fs")
  // );

  // m_program->setUniform("volume_xyz", start_image_unit);
  // m_program->setUniform("volume_uv", start_image_unit + 1);
  // m_program->setUniform("volume_res", volume_res);
  // auto vol_to_world(glm::scale(glm::fmat4{1.0f}, glm::fvec3{m_bbox.getPMax()[0] - m_bbox.getPMin()[0],
  //                                                           m_bbox.getPMax()[1] - m_bbox.getPMin()[1],
  //                                                           m_bbox.getPMax()[2] - m_bbox.getPMin()[2]}));

  // vol_to_world = glm::translate(glm::fmat4{1.0f}, glm::fvec3{m_bbox.getPMin()[0], m_bbox.getPMin()[1], m_bbox.getPMin()[2]}) * vol_to_world;
  // auto world_to_vol(glm::inverse(vol_to_world));
  // m_program->setUniform("world_to_vol", world_to_vol);

  std::vector<float> empty_xyz(volume_res.x * volume_res.y * volume_res.z * 4, -0.01f);
  std::vector<uv> empty_uv(volume_res.x * volume_res.y * volume_res.z, {0.0f, 1.0f});
  for (unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i) {
    auto volume_xyz = globjects::Texture::createDefault(GL_TEXTURE_3D);
    volume_xyz->image3D(0, GL_RGBA32F, glm::ivec3{volume_res}, 0, GL_RGBA, GL_FLOAT, empty_xyz.data());
    m_volumes_xyz_inv.emplace_back(volume_xyz);
  }

  // reload();
  for(unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i){
    addVolume(m_cv_xyz_filenames[i], m_cv_uv_filenames[i]);
  }

  createVolumeTextures();
}

CalibVolumes::~CalibVolumes(){
  for(unsigned i = 0; i < m_volumes_xyz.size(); ++i){
    m_volumes_xyz[i]->destroy();
    m_volumes_uv[i]->destroy();
    m_volumes_xyz_inv[i]->destroy();
  }
}

void CalibVolumes::reload(){
  if (!m_volumes_xyz.empty()) { 
    for(unsigned i = 0; i < m_volumes_xyz.size(); ++i){
      m_volumes_xyz[i]->destroy();
      m_volumes_uv[i]->destroy();
      m_volumes_xyz_inv[i]->destroy();
    }
  }

  m_volumes_xyz.clear();
  m_volumes_uv.clear();
  m_volumes_xyz_inv.clear();

  for(unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i){
    addVolume(m_cv_xyz_filenames[i], m_cv_uv_filenames[i]);
  }

  if(m_start_texture_unit >= 0) {
    bindToTextureUnits();
  }
}

void CalibVolumes::writeInverseCalibs(std::string const& path) const {
  for(unsigned i = 0; i < m_data_volumes_xyz_inv.size(); ++i) {
    m_data_volumes_xyz_inv[i].write(path + m_cv_xyz_filenames[i] + "_inv");
  }
}

void CalibVolumes::loadInverseCalibs(std::string const& path) {
  for(unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i){
    std::string name_source{m_cv_xyz_filenames[i].substr( m_cv_xyz_filenames[i].find_last_of("/\\") + 1)};
    std::string name_input{path + name_source + "_inv"};
    m_data_volumes_xyz_inv.emplace_back(name_input);
    auto const& calib{m_data_volumes_xyz.back()};
    std::cout << "dimensions xyz - " << calib.res().x << ", " << calib.res().y << ", " << calib.res().z 
              << " minmax d - " << calib.depthLimits().x << ", " << calib.depthLimits().y << std::endl;
  }

  for (unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i) {
    m_volumes_xyz_inv[i]->image3D(0, GL_RGBA32F, glm::ivec3{volume_res}, 0, GL_RGBA, GL_FLOAT, m_data_volumes_xyz_inv[i].volume().data());
  }
}

void CalibVolumes::calculateInverseVolumes(){

  calculateInverseVolumes2();
  std::cout << "uploading inverted data" << std::endl;
  for (unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i) {
    m_volumes_xyz_inv[i]->image3D(0, GL_RGBA32F, glm::ivec3{volume_res}, 0, GL_RGBA, GL_FLOAT, m_data_volumes_xyz_inv[i].volume().data());
  }

  if(m_start_texture_unit_inv >= 0) {
    bindToTextureUnitsInv();
  }
}

std::vector<sample_t> CalibVolumes::getXyzSamples(std::size_t i) {
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

void CalibVolumes::calculateInverseVolumes2() {
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
    auto curr_calib_samples{getXyzSamples(i)};
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
      std::cout << "x slice " << x << " done" << std::endl;
      sample_pos.x += sample_step.x;
      sample_pos.y = sample_start.y;
    }
    std::cout << "storing volume " << i << std::endl;
    m_data_volumes_xyz_inv.emplace_back(volume_res, glm::fvec2{0.5f, 4.5f}, curr_volume_inv);
  }
}

std::vector<int> CalibVolumes::getXYZVolumeUnitsInv() const {
  std::vector<int> units(5, 0);
  for(int i = 0; i < int(m_cv_xyz_filenames.size()); ++i) {
    units[i] = m_start_texture_unit_inv + i;
  }
  return units;
}

glm::uvec3 CalibVolumes::getVolumeRes() const {
  return volume_res;
}

glm::fvec2 CalibVolumes::getDepthLimits(unsigned i) const {
  return m_data_volumes_xyz[i].depthLimits();
}

void CalibVolumes::addVolume(std::string const& filename_xyz, std::string const& filename_uv) {
  std::cerr << "loading " << filename_xyz << std::endl;
  m_data_volumes_xyz.emplace_back(filename_xyz);
  auto const& calib_xyz{m_data_volumes_xyz.back()};
  std::cout << "dimensions xyz - " << calib_xyz.res().x << ", " << calib_xyz.res().y << ", " << calib_xyz.res().z 
            << " minmax d - " << calib_xyz.depthLimits().x << ", " << calib_xyz.depthLimits().y << std::endl;

  std::cerr << "loading " << filename_uv << std::endl;
  m_data_volumes_uv.emplace_back(filename_uv);
  auto const& calib_uv{m_data_volumes_uv.back()};
  std::cout << "dimensions uv - " << calib_uv.res().x << ", " << calib_uv.res().y << ", " << calib_uv.res().z 
            << " minmax d - " << calib_uv.depthLimits().x << ", " << calib_uv.depthLimits().y << std::endl;

}

void CalibVolumes::createVolumeTextures() {
  for(unsigned i = 0; i < m_data_volumes_xyz.size(); ++i){
    auto const& calib_xyz = m_data_volumes_xyz[i];
    auto const& calib_uv = m_data_volumes_uv[i];
    auto volume_xyz = globjects::Texture::createDefault(GL_TEXTURE_3D);
    volume_xyz->image3D(0, GL_RGB32F, glm::ivec3{calib_xyz.res()}, 0, GL_RGB, GL_FLOAT, calib_xyz.volume().data());
    auto volume_uv = globjects::Texture::createDefault(GL_TEXTURE_3D);
    volume_uv->image3D(0, GL_RG32F, glm::ivec3{calib_uv.res()}, 0, GL_RG, GL_FLOAT, calib_uv.volume().data());

    m_volumes_xyz.push_back(volume_xyz);
    m_volumes_uv.push_back(volume_uv);
  }
}

std::vector<int> CalibVolumes::getXYZVolumeUnits() const {
  std::vector<int> units(5, 0);
  for(int i = 0; i < int(m_cv_xyz_filenames.size()); ++i) {
    units[i] = m_start_texture_unit + i * 2;
  }
  return units;
}
std::vector<int> CalibVolumes::getUVVolumeUnits() const {
  std::vector<int> units(5, 0);
  for(int i = 0; i < int(m_cv_xyz_filenames.size()); ++i) {
    units[i] = m_start_texture_unit + i * 2 + 1;
  }
  return units;
}

void
CalibVolumes::bindToTextureUnits() {
  for(unsigned layer = 0; layer < m_cv_xyz_filenames.size(); ++layer){
    m_volumes_xyz[layer]->bindActive(GL_TEXTURE0 + m_start_texture_unit + layer * 2);
    m_volumes_uv[layer]->bindActive(GL_TEXTURE0 + m_start_texture_unit + layer * 2 + 1);
  }
  glActiveTexture(GL_TEXTURE0);
}

void
CalibVolumes::bindToTextureUnitsInv() {
  for(unsigned layer = 0; layer < m_cv_xyz_filenames.size(); ++layer){
    m_volumes_xyz_inv[layer]->bindActive(GL_TEXTURE0 + m_start_texture_unit_inv + layer);
  }
  glActiveTexture(GL_TEXTURE0);
}

void CalibVolumes::setStartTextureUnit(unsigned start_texture_unit) {
  m_start_texture_unit = start_texture_unit;
  bindToTextureUnits();
}

void CalibVolumes::setStartTextureUnitInv(unsigned start_texture_unit) {
  m_start_texture_unit_inv = start_texture_unit;
  bindToTextureUnitsInv();
}

}