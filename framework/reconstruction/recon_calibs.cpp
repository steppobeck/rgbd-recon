#include "recon_calibs.hpp"

#include "calibration_files.hpp"
#include "unit_cube.hpp"
#include <KinectCalibrationFile.h>
#include "CalibVolumes.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/io.hpp>
#include <globjects/Shader.h>
#include <globjects/globjects.h>

namespace kinect{

static float limit = 0.01f;

ReconCalibs::ReconCalibs(CalibrationFiles const& cfs, CalibVolumes const* cv, gloost::BoundingBox const&  bbox)
 :Reconstruction(cfs, cv, bbox)
 ,m_program{new globjects::Program()}
 ,m_sampler{cv->getVolumeRes()}
 ,m_active_kinect{0}
{
  m_program->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/calib_vis.vs"),
    globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/calib_vis.fs")
  );

  m_program->setUniform("volume_tsdf", 29);
  m_program->setUniform("cv_xyz_inv", m_cv->getXYZVolumeUnitsInv());
  m_program->setUniform("cv_uv_inv", m_cv->getUVVolumeUnitsInv());

  glm::fvec3 bbox_dimensions = glm::fvec3{m_bbox.getPMax()[0] - m_bbox.getPMin()[0],
                                          m_bbox.getPMax()[1] - m_bbox.getPMin()[1],
                                          m_bbox.getPMax()[2] - m_bbox.getPMin()[2]};
  glm::fvec3 bbox_translation = glm::fvec3{m_bbox.getPMin()[0], m_bbox.getPMin()[1], m_bbox.getPMin()[2]};

  auto vol_to_world(glm::scale(glm::fmat4{1.0f}, bbox_dimensions));
  vol_to_world = glm::translate(glm::fmat4{1.0f}, bbox_translation) * vol_to_world;
  m_program->setUniform("vol_to_world", vol_to_world);

  m_program->setUniform("limit", limit);
}

ReconCalibs::~ReconCalibs() {
  m_program->destroy();
}

void ReconCalibs::draw(){
  m_program->use();

  m_sampler.sample();

  m_program->release();  
}

void ReconCalibs::setActiveKinect(unsigned num_kinect) {
  m_active_kinect = num_kinect;
}

}