#include "recon_calibs.hpp"

#include "calibration_files.hpp"
#include <KinectCalibrationFile.h>
#include <CalibVolume.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>
#include <globjects/Shader.h>
#include <globjects/globjects.h>

namespace kinect{

static int start_image_unit = 3;
static float limit = 0.01f;

ReconCalibs::ReconCalibs(CalibrationFiles const& cfs, CalibVolume const* cv, gloost::BoundingBox const&  bbox)
 :Reconstruction(cfs, cv, bbox)
 ,m_program{new globjects::Program()}
 ,m_program_integration{new globjects::Program()}
 ,m_sampler{cv->getVolumeRes()}
 ,m_volume_tsdf{}
 ,m_active_kinect{0}
{
  m_program->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/calib_vis.vs"),
    globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/calib_vis.fs")
  );
  m_program_integration->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/tsdf_integration.vs"),
    globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/calib_vis.fs")
  );
  m_program_integration->setUniform("cv_xyz_inv", m_cv->getXYZVolumeUnitsInv());
  m_program_integration->setUniform("cv_uv_inv", m_cv->getUVVolumeUnitsInv());
  // m_program->setUniform("cv_xyz", m_cv->getXYZVolumeUnits());
  // m_program->setUniform("cv_uv", m_cv->getUVVolumeUnits());
  m_program->setUniform("volume_tsdf", 29);
  auto vol_to_world(glm::scale(glm::fmat4{1.0f}, glm::fvec3{m_bbox.getPMax()[0] - m_bbox.getPMin()[0],
                                                            m_bbox.getPMax()[1] - m_bbox.getPMin()[1],
                                                            m_bbox.getPMax()[2] - m_bbox.getPMin()[2]}));

  vol_to_world = glm::translate(glm::fmat4{1.0f}, glm::fvec3{m_bbox.getPMin()[0], m_bbox.getPMin()[1], m_bbox.getPMin()[2]}) * vol_to_world;
  // auto world_to_vol(glm::inverse(vol_to_world));
  m_program->setUniform("vol_to_world", vol_to_world);

  m_program_integration->setUniform("volume_tsdf", start_image_unit);
  m_program_integration->setUniform("kinect_colors",1);
  m_program_integration->setUniform("kinect_depths",2);
  m_program_integration->setUniform("kinect_qualities",3);

  m_program_integration->setUniform("num_kinects", m_num_kinects);
  m_program_integration->setUniform("res_depth", glm::uvec2{m_cf->getWidth(), m_cf->getHeight()});
  auto volume_res = cv->getVolumeRes();
  m_program_integration->setUniform("res_tsdf", volume_res);
  m_program_integration->setUniform("limit", limit);
  m_program->setUniform("limit", limit);

  m_volume_tsdf = globjects::Texture::createDefault(GL_TEXTURE_3D);
  std::vector<float> empty_tsdf(volume_res.x * volume_res.y * volume_res.z, -10.0f);
  m_volume_tsdf->image3D(0, GL_R32F, volume_res.x, volume_res.y, volume_res.z, 0, GL_RED, GL_FLOAT, empty_tsdf.data());
  m_volume_tsdf->bindActive(GL_TEXTURE0 + 29);
}

ReconCalibs::~ReconCalibs() {
  m_program->destroy();
  m_program_integration->destroy();
}

void ReconCalibs::draw(){
  integrate();

  m_program->use();

  m_sampler.sample();

  m_program->release();  
}

void ReconCalibs::integrate() {
  glEnable(GL_RASTERIZER_DISCARD);
  m_program_integration->use();

  m_volume_tsdf->bindImageTexture(start_image_unit, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);

  m_sampler.sample();

  m_program_integration->release();
  glDisable(GL_RASTERIZER_DISCARD);
}

void ReconCalibs::setActiveKinect(unsigned num_kinect) {
  m_active_kinect = num_kinect;
}

}