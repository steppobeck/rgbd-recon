#include "recon_calibs.hpp"

#include "calibration_files.hpp"
#include <KinectCalibrationFile.h>
#include <CalibVolume.h>

// #include <glm/gtc/type_precision.hpp>
#include <globjects/Shader.h>

namespace kinect{

ReconCalibs::ReconCalibs(CalibrationFiles const& cfs, CalibVolume const* cv, gloost::BoundingBox const&  bbox)
 :Reconstruction(cfs, cv, bbox)
 ,m_program{new globjects::Program()}
 ,m_sampler{glm::uvec3{m_cv->m_cv_widths[0], m_cv->m_cv_heights[0], m_cv->m_cv_depths[0]}}
 ,m_num_kinect{0}
{
  m_program->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/calib_sample.vs"),
    globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/calib_sample.fs")
  );

  m_program->setUniform("cv_xyz", m_cv->getXYZVolumeUnits());
  m_program->setUniform("cv_uv", m_cv->getUVVolumeUnits());
}

ReconCalibs::~ReconCalibs() {
  m_program->destroy();
}

void
ReconCalibs::draw(){
  m_program->use();

  // for(unsigned layer = 0; layer < m_num_kinects; ++layer) {
    m_program->setUniform("layer", m_num_kinect);
    m_sampler.sample();
  // }

  m_program->release();
}

void ReconCalibs::setActiveKinect(unsigned num_kinect) {
  m_num_kinect = num_kinect;
}

}