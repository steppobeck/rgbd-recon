#include "CalibVolume.h"
#include <KinectCalibrationFile.h>
#include <timevalue.h>

#include <glbinding/gl/functions-patches.h>
#include <globjects/Shader.h>

#include <glm/gtc/matrix_transform.hpp>

namespace kinect{

static glm::uvec3 volume_res{128,256,128};
static int start_image_unit = 1;

CalibVolume::CalibVolume(std::vector<std::string> const& calib_volume_files, gloost::BoundingBox const& bbox)
 :m_cv_xyz_filenames()
 ,m_cv_uv_filenames()
 ,m_volumes_xyz{}
 ,m_volumes_uv{}
 ,m_volumes_xyz_inv{}
 ,m_volumes_uv_inv{}
 ,m_program{new globjects::Program()}
 ,m_sampler{volume_res}
 ,m_bbox{bbox}
 ,m_cv_min_ds()
 ,m_cv_max_ds()
 ,m_cv_widths()
 ,m_cv_heights()
 ,m_cv_depths()
 ,m_start_texture_unit(-1)
 ,m_start_texture_unit_inv(-1)
{
  for(auto const& calib_file : calib_volume_files){
  	std::string basefile = calib_file;
  	basefile.replace( basefile.end() - 3, basefile.end(), "");
  	m_cv_xyz_filenames.push_back(basefile + "cv_xyz");
  	m_cv_uv_filenames.push_back(basefile + "cv_uv");
  }

  m_program->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/calib_sample.vs"),
    globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/calib_vis.fs")
  );

  m_program->setUniform("volume_xyz", start_image_unit);
  m_program->setUniform("volume_uv", start_image_unit + 1);
  m_program->setUniform("volume_res", volume_res);
  auto vol_to_world(glm::scale(glm::fmat4{1.0f}, glm::fvec3{m_bbox.getPMax()[0] - m_bbox.getPMin()[0],
                                                            m_bbox.getPMax()[1] - m_bbox.getPMin()[1],
                                                            m_bbox.getPMax()[2] - m_bbox.getPMin()[2]}));

  vol_to_world = glm::translate(glm::fmat4{1.0f}, glm::fvec3{m_bbox.getPMin()[0], m_bbox.getPMin()[1], m_bbox.getPMin()[2]}) * vol_to_world;
  auto world_to_vol(glm::inverse(vol_to_world));
  m_program->setUniform("world_to_vol", world_to_vol);

  std::vector<float> empty_xyz(volume_res.x * volume_res.y * volume_res.z * 4, -0.01f);
  std::vector<uv> empty_uv(volume_res.x * volume_res.y * volume_res.z, {0.0f, 1.0f});
  for (unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i) {
    auto volume_xyz = globjects::Texture::createDefault(GL_TEXTURE_3D);
    volume_xyz->image3D(0, GL_RGBA32F, volume_res.x, volume_res.y, volume_res.z, 0, GL_RGBA, GL_FLOAT, empty_xyz.data());
    m_volumes_xyz_inv.emplace_back(volume_xyz);

    auto volume_uv = globjects::Texture::createDefault(GL_TEXTURE_3D);
    volume_uv->image3D(0, GL_RG32F, volume_res.x, volume_res.y, volume_res.z, 0, GL_RG, GL_FLOAT, empty_uv.data());
    m_volumes_uv_inv.emplace_back(volume_uv);
  }

  reload();
}

/*virtual*/
CalibVolume::~CalibVolume(){
  for(unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i){
    m_volumes_xyz[i]->destroy();
    m_volumes_uv[i]->destroy();
    m_volumes_xyz_inv[i]->destroy();
    m_volumes_uv_inv[i]->destroy();
  }
}

void CalibVolume::reload(){
  if (!m_volumes_xyz.empty()) { 
    for(unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i){
      m_volumes_xyz[i]->destroy();
      m_volumes_uv[i]->destroy();
    }
  }

  m_cv_widths.clear();
  m_cv_heights.clear();
  m_cv_depths.clear();
  m_cv_min_ds.clear();
  m_cv_max_ds.clear();
  m_volumes_xyz.clear();
  m_volumes_uv.clear();

  for(unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i){
    addVolume(m_cv_xyz_filenames[i], m_cv_uv_filenames[i]);
  }

  if(m_start_texture_unit >= 0) {
    bindToTextureUnits();
  }
}

void CalibVolume::calculateInverseVolumes(){
  m_program->setUniform("cv_xyz", getXYZVolumeUnits());
  m_program->setUniform("cv_uv", getUVVolumeUnits());

  m_program->use();
  glEnable(GL_RASTERIZER_DISCARD);

  for(unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i) {
    m_program->setUniform("layer", i);
    m_volumes_xyz_inv[i]->bindImageTexture(start_image_unit, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    m_volumes_uv_inv[i]->bindImageTexture(start_image_unit + 1, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RG32F);
    
    m_sampler.sample();
  }

  glDisable(GL_RASTERIZER_DISCARD);
  m_program->release();

  if(m_start_texture_unit_inv >= 0) {
    bindToTextureUnitsInv();
  }
}

std::vector<int> CalibVolume::getXYZVolumeUnitsInv() const {
  std::vector<int> units(5, 0);
  for(int i = 0; i < int(m_cv_xyz_filenames.size()); ++i) {
    units[i] = m_start_texture_unit_inv + i * 2;
  }
  return units;
}
std::vector<int> CalibVolume::getUVVolumeUnitsInv() const {
  std::vector<int> units(5, 0);
  for(int i = 0; i < int(m_cv_xyz_filenames.size()); ++i) {
    units[i] = m_start_texture_unit_inv  + i * 2 + 1;
  }
  return units;
}

glm::uvec3 CalibVolume::getVolumeRes() const {
  return volume_res;
}

void CalibVolume::addVolume(std::string const& filename_xyz, std::string filename_uv) {
  unsigned width_xyz = 0;
  unsigned height_xyz = 0;
  unsigned depth_xyz = 0;

  unsigned width_uv = 0;
  unsigned height_uv = 0;
  unsigned depth_uv = 0;

  float min_d_xyz = 0.0f;
  float max_d_xyz = 0.0f;
  float min_d_uv = 0.0f;
  float max_d_uv = 0.0f;

  std::cerr << "loading " << filename_xyz << std::endl;
  FILE* file_xyz = fopen( filename_xyz.c_str(), "rb");
  fread(&width_xyz, sizeof(unsigned), 1, file_xyz);
  fread(&height_xyz, sizeof(unsigned), 1, file_xyz);
  fread(&depth_xyz, sizeof(unsigned), 1, file_xyz);
  fread(&min_d_xyz, sizeof(float), 1, file_xyz);
  fread(&max_d_xyz, sizeof(float), 1, file_xyz);
  std::cout << "dimensions xyz - " << width_xyz << ", " << height_xyz << ", " << depth_xyz << " minmax d - " << min_d_xyz << ", " << max_d_xyz << std::endl;
  std::vector<xyz> storage_xyz(width_xyz * height_xyz * depth_xyz);
  fread(storage_xyz.data(), sizeof(xyz), width_xyz * height_xyz * depth_xyz, file_xyz);
  fclose(file_xyz);

  std::cerr << "loading " << filename_uv << std::endl;
  FILE* file_uv = fopen( filename_uv.c_str(), "rb");
  fread(&width_uv, sizeof(unsigned), 1, file_uv);
  fread(&height_uv, sizeof(unsigned), 1, file_uv);
  fread(&depth_uv, sizeof(unsigned), 1, file_uv);
  fread(&min_d_uv, sizeof(float), 1, file_uv);
  fread(&max_d_uv, sizeof(float), 1, file_uv);
  std::cout << "dimensions uv - " << width_uv << ", " << height_uv << ", " << depth_uv << " minmax d - " << min_d_uv << ", " << max_d_uv << std::endl;
  std::vector<uv> storage_uv(width_uv * height_uv * depth_uv);
  fread(storage_uv.data(), sizeof(uv), width_uv * height_uv * depth_uv, file_uv);
  fclose(file_uv);
  
  auto volume_xyz = globjects::Texture::createDefault(GL_TEXTURE_3D);
  volume_xyz->image3D(0, GL_RGB32F, width_xyz, height_xyz, depth_xyz, 0, GL_RGB, GL_FLOAT, storage_xyz.data());
  auto volume_uv = globjects::Texture::createDefault(GL_TEXTURE_3D);
  volume_uv->image3D(0, GL_RG32F, width_uv, height_uv, depth_uv, 0, GL_RG, GL_FLOAT, storage_uv.data());

  m_volumes_xyz.push_back(volume_xyz);
  m_volumes_uv.push_back(volume_uv);

  m_cv_widths.push_back(width_xyz);
  m_cv_heights.push_back(height_xyz);
  m_cv_depths.push_back(depth_xyz);
  m_cv_min_ds.push_back(min_d_xyz);
  m_cv_max_ds.push_back(max_d_xyz);
}

std::vector<int> CalibVolume::getXYZVolumeUnits() const {
  std::vector<int> units(5, 0);
  for(int i = 0; i < int(m_cv_xyz_filenames.size()); ++i) {
    units[i] = m_start_texture_unit + i * 2;
  }
  return units;
}
std::vector<int> CalibVolume::getUVVolumeUnits() const {
  std::vector<int> units(5, 0);
  for(int i = 0; i < int(m_cv_xyz_filenames.size()); ++i) {
    units[i] = m_start_texture_unit + i * 2 + 1;
  }
  return units;
}

void
CalibVolume::bindToTextureUnits() {
  for(unsigned layer = 0; layer < m_cv_xyz_filenames.size(); ++layer){
    m_volumes_xyz[layer]->bindActive(GL_TEXTURE0 + m_start_texture_unit + layer * 2);
    m_volumes_uv[layer]->bindActive(GL_TEXTURE0 + m_start_texture_unit + layer * 2 + 1);
  }
  glActiveTexture(GL_TEXTURE0);
}

void
CalibVolume::bindToTextureUnitsInv() {
  for(unsigned layer = 0; layer < m_cv_xyz_filenames.size(); ++layer){
    m_volumes_xyz_inv[layer]->bindActive(GL_TEXTURE0 + m_start_texture_unit_inv + layer * 2);
    m_volumes_uv_inv[layer]->bindActive(GL_TEXTURE0 + m_start_texture_unit_inv + layer * 2 + 1);
  }
  glActiveTexture(GL_TEXTURE0);
}

void CalibVolume::setStartTextureUnit(unsigned start_texture_unit) {
  m_start_texture_unit = start_texture_unit;
  bindToTextureUnits();
}

void CalibVolume::setStartTextureUnitInv(unsigned start_texture_unit) {
  m_start_texture_unit_inv = start_texture_unit;
  bindToTextureUnitsInv();
}

}