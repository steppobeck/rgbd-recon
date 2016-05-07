#include "recon_calibs.hpp"

#include "calibration_files.hpp"
#include <KinectCalibrationFile.h>
#include <CalibVolume.h>

// #include <glm/gtc/type_precision.hpp>
#include <globjects/Shader.h>
#include <globjects/globjects.h>

namespace kinect{

static glm::uvec3 volume_res{128,256,128};
static unsigned start_image_unit = 1;
ReconCalibs::ReconCalibs(CalibrationFiles const& cfs, CalibVolume const* cv, gloost::BoundingBox const&  bbox)
 :Reconstruction(cfs, cv, bbox)
 ,m_program{new globjects::Program()}
 ,m_sampler{glm::uvec3{m_cv->m_cv_widths[0], m_cv->m_cv_heights[0], m_cv->m_cv_depths[0]}}
 ,m_num_kinect{0}
 ,m_volumes_xyz{}
 ,m_volumes_uv{}
{
  m_program->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/calib_sample.vs"),
    globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/calib_sample.fs")
  );

  m_program->setUniform("cv_xyz", m_cv->getXYZVolumeUnits());
  m_program->setUniform("cv_uv", m_cv->getUVVolumeUnits());
  m_program->setUniform("volume_xyz", start_image_unit);
  m_program->setUniform("volume_uv", start_image_unit + 1);
  // int num_slots =  globjects::getInteger(GL_MAX_IMAGE_UNITS);
  // std::cout << num_slots << "slots!" << std::endl;
  std::vector<xyz> empty_xyz(volume_res.x * volume_res.y * volume_res.z, {-1.0f, -1.0f, -1.0f});
  std::vector<uv> empty_uv(volume_res.x * volume_res.y * volume_res.z, {-1.0f, -1.0f});
  for (unsigned i = 0; i < cfs.num(); ++i) {
    auto volume_xyz = globjects::Texture::createDefault(GL_TEXTURE_3D);
    volume_xyz->image3D(0, GL_RGB32F, volume_res.x, volume_res.y, volume_res.z, 0, GL_RGB, GL_FLOAT, empty_xyz.data());
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
  m_program->setUniform("volume_res", volume_res);

  for(unsigned i = 0; i < m_num_kinects; ++i) {
    m_program->setUniform("layer", i);
    // m_volumes_xyz[i]->bindImageTexture(start_image_unit, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    // m_volumes_uv[i]->bindImageTexture(start_image_unit + 1, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RG32F);
    
    m_sampler.sample();
  }
    // m_program->setUniform("layer", m_num_kinect);
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
  m_program->use();
  m_program->setUniform("volume_res", volume_res);

  for(unsigned i = 0; i < m_num_kinects; ++i) {
    m_program->setUniform("layer", i);
    m_volumes_xyz[i]->bindImageTexture(start_image_unit, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    m_volumes_uv[i]->bindImageTexture(start_image_unit + 1, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RG32F);
    
    m_sampler.sample();
  }

  m_program->release();
}

void ReconCalibs::setActiveKinect(unsigned num_kinect) {
  m_num_kinect = num_kinect;
}

}