#include "recon_calibs.hpp"

#include "calibration_files.hpp"
#include <KinectCalibrationFile.h>
#include <CalibVolume.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>
#include <globjects/Shader.h>
#include <globjects/globjects.h>

namespace kinect{

ReconCalibs::ReconCalibs(CalibrationFiles const& cfs, CalibVolume const* cv, gloost::BoundingBox const&  bbox)
 :Reconstruction(cfs, cv, bbox)
 ,m_program{new globjects::Program()}
 ,m_sampler{cv->getVolumeRes()}
 ,m_active_kinect{0}
{
  m_program->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/calib_vis.vs"),
    globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/calib_vis.fs")
  );
  m_program->setUniform("cv_xyz_inv", m_cv->getXYZVolumeUnitsInv());
  m_program->setUniform("cv_uv_inv", m_cv->getUVVolumeUnitsInv());
  m_program->setUniform("cv_xyz", m_cv->getXYZVolumeUnits());
  m_program->setUniform("cv_uv", m_cv->getUVVolumeUnits());
}

ReconCalibs::~ReconCalibs() {
  m_program->destroy();
}

void ReconCalibs::draw(){
  m_program->use();
  m_program->setUniform("layer", m_active_kinect);

  m_sampler.sample();

  m_program->release();
}

void ReconCalibs::setActiveKinect(unsigned num_kinect) {
  m_active_kinect = num_kinect;
}

}