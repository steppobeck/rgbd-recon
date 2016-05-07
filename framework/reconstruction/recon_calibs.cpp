#include "recon_calibs.hpp"

#include "calibration_files.hpp"
#include <KinectCalibrationFile.h>
#include <CalibVolume.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>
#include <globjects/Shader.h>
#include <globjects/globjects.h>

namespace kinect{

static glm::uvec3 volume_res{128,256,128};
static int start_image_unit = 1;
ReconCalibs::ReconCalibs(CalibrationFiles const& cfs, CalibVolume const* cv, gloost::BoundingBox const&  bbox)
 :Reconstruction(cfs, cv, bbox)
 ,m_program{new globjects::Program()}
 ,m_program_sample{new globjects::Program()}
 ,m_sampler{glm::uvec3{m_cv->m_cv_widths[0], m_cv->m_cv_heights[0], m_cv->m_cv_depths[0]}}
 ,m_num_kinect{0}
 ,m_volumes_xyz{}
 ,m_volumes_uv{}
 ,m_start_texture_unit{30}
{
  m_program->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/calib_vis.vs"),
    globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/calib_vis.fs")
  );
  m_program->setUniform("cv_xyz", getXYZVolumeUnits());
  m_program->setUniform("cv_uv", getUVVolumeUnits());
  // m_program->setUniform("cv_xyz", m_cv->getXYZVolumeUnits());
  // m_program->setUniform("cv_uv", m_cv->getUVVolumeUnits());

  m_program_sample->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/calib_sample.vs"),
    globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/calib_vis.fs")
  );
  m_program_sample->setUniform("cv_xyz", m_cv->getXYZVolumeUnits());
  m_program_sample->setUniform("cv_uv", m_cv->getUVVolumeUnits());
  m_program_sample->setUniform("volume_xyz", start_image_unit);
  m_program_sample->setUniform("volume_uv", start_image_unit + 1);

  // int num_slots =  globjects::getInteger(GL_MAX_IMAGE_UNITS);
  // std::cout << num_slots << "slots!" << std::endl;
  std::vector<float> empty_xyz(volume_res.x * volume_res.y * volume_res.z * 4, -1.0f);
  std::vector<uv> empty_uv(volume_res.x * volume_res.y * volume_res.z, {-1.0f, -1.0f});
  for (unsigned i = 0; i < cfs.num(); ++i) {
    auto volume_xyz = globjects::Texture::createDefault(GL_TEXTURE_3D);
    volume_xyz->image3D(0, GL_RGBA32F, volume_res.x, volume_res.y, volume_res.z, 0, GL_RGBA, GL_FLOAT, empty_xyz.data());
    // volume_xyz->image3D(0, GL_RG32F, volume_res.x, volume_res.y, volume_res.z, 0, GL_RG, GL_FLOAT, empty_xyz.data());
    m_volumes_xyz.emplace_back(volume_xyz);

    auto volume_uv = globjects::Texture::createDefault(GL_TEXTURE_3D);
    volume_uv->image3D(0, GL_RG32F, volume_res.x, volume_res.y, volume_res.z, 0, GL_RG, GL_FLOAT, empty_uv.data());
    m_volumes_uv.emplace_back(volume_uv);
  }
}

ReconCalibs::~ReconCalibs() {
  m_program->destroy();
  for (std::size_t i = 0; i < m_volumes_xyz.size(); ++i) {
    m_volumes_xyz[i]->destroy();
    m_volumes_uv[i]->destroy();
  }
}

void ReconCalibs::draw(){
  m_program->use();
  m_program->setUniform("layer", m_num_kinect);

  // for(unsigned i = 0; i < m_num_kinects; ++i) {
  //   m_program->setUniform("layer", i);
    m_sampler.sample();
  // }
    // glBegin(GL_POINTS);
    //   glVertex3f(0.0f, 0.0f, 0.0f);
    //   glVertex3f(1.0f, 0.0f, 0.0f);
    //   glVertex3f(0.0f, 1.0f, 0.0f);
    //   glVertex3f(0.0f, 0.0f, 1.0f);

    //   glVertex3f(1.0f, 1.0f, 0.0f);
    //   glVertex3f(1.0f, 1.0f, 1.0f);
    //   glVertex3f(0.0f, 1.0f, 1.0f);
    //   glVertex3f(1.0f, 0.0f, 1.0f);
    // glEnd();

  m_program->release();
}
void ReconCalibs::process(){
  m_program_sample->use();
  m_program_sample->setUniform("volume_res", volume_res);
  auto vol_to_world(glm::scale(glm::fmat4{1.0f}, glm::fvec3{m_bbox.getPMax()[0] - m_bbox.getPMin()[0],
                                                            m_bbox.getPMax()[1] - m_bbox.getPMin()[1],
                                                            m_bbox.getPMax()[2] - m_bbox.getPMin()[2]}));

  vol_to_world = glm::translate(glm::fmat4{1.0f}, glm::fvec3{m_bbox.getPMin()[0], m_bbox.getPMin()[1], m_bbox.getPMin()[2]}) * vol_to_world;
  auto world_to_vol(glm::inverse(vol_to_world));
  std::cout << world_to_vol << std::endl;
  m_program_sample->setUniform("world_to_vol", world_to_vol);

 VolumeSampler calib_sampler{volume_res};

  glEnable(GL_RASTERIZER_DISCARD);
  for(unsigned i = 0; i < m_num_kinects; ++i) {
    m_program_sample->setUniform("layer", i);
    m_volumes_xyz[i]->bindImageTexture(start_image_unit, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    m_volumes_uv[i]->bindImageTexture(start_image_unit + 1, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RG32F);
    
    calib_sampler.sample();
  }

  glDisable(GL_RASTERIZER_DISCARD);
  m_program_sample->release();

  glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
  std::vector<float> xyz_data(volume_res.x * volume_res.y * volume_res.z * 4);
  m_volumes_xyz[0]->getImage(0, GL_RGBA, GL_FLOAT, xyz_data.data());
  for(unsigned i = 0; i < xyz_data.size() / 4; ++i) {
    if(xyz_data[i] > 0.0f) {
      std::cout << "value " << xyz_data[i] << ", " << xyz_data[i + 1] << ", "<< xyz_data[i + 2] << ", " << xyz_data[i + 3] << " at " << i << std::endl;
      break;
    } 
    // std::cout << xyz_data[i * 3] << ", " <<xyz_data[i * 3 + 1] << "," << xyz_data[i * 3 + 2] << std::endl;
  }

  bindToTextureUnits();
}

std::vector<int> ReconCalibs::getXYZVolumeUnits() const {
  std::vector<int> units(5, 0);
  for(int i = 0; i < int(m_volumes_xyz.size()); ++i) {
    units[i] = m_start_texture_unit + i * 2;
  }
  return units;
}
std::vector<int> ReconCalibs::getUVVolumeUnits() const {
  std::vector<int> units(5, 0);
  for(int i = 0; i < int(m_volumes_xyz.size()); ++i) {
    units[i] = m_start_texture_unit + i * 2 + 1;
  }
  return units;
}

void ReconCalibs::bindToTextureUnits() {
  for(unsigned layer = 0; layer < m_volumes_xyz.size(); ++layer){
    m_volumes_xyz[layer]->bindActive(GL_TEXTURE0 + m_start_texture_unit + layer * 2);
    m_volumes_uv[layer]->bindActive(GL_TEXTURE0 + m_start_texture_unit + layer * 2 + 1);
  }
  glActiveTexture(GL_TEXTURE0);
  // m_start_texture_unit = start_texture_unit;    
}


void ReconCalibs::setActiveKinect(unsigned num_kinect) {
  m_num_kinect = num_kinect;
}

}