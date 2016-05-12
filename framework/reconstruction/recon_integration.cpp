#include "recon_integration.hpp"

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

static int start_image_unit = 3;
static float limit = 0.01f;

ReconIntegration::ReconIntegration(CalibrationFiles const& cfs, CalibVolumes const* cv, gloost::BoundingBox const&  bbox)
 :Reconstruction(cfs, cv, bbox)
 ,m_program{new globjects::Program()}
 ,m_program_integration{new globjects::Program()}
 ,m_sampler{cv->getVolumeRes()}
 ,m_volume_tsdf{}
 ,m_mat_vol_to_world{1.0f}
{
  m_program->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/tsdf_raymarch.vs"),
    globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/tsdf_raymarch.fs")
  );
  m_program_integration->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/tsdf_integration.vs")
  );
  m_program_integration->setUniform("cv_xyz_inv", m_cv->getXYZVolumeUnitsInv());
  m_program_integration->setUniform("cv_uv_inv", m_cv->getUVVolumeUnitsInv());
  m_program->setUniform("volume_tsdf", 29);

  glm::fvec3 bbox_dimensions = glm::fvec3{m_bbox.getPMax()[0] - m_bbox.getPMin()[0],
                                          m_bbox.getPMax()[1] - m_bbox.getPMin()[1],
                                          m_bbox.getPMax()[2] - m_bbox.getPMin()[2]};
  glm::fvec3 bbox_translation = glm::fvec3{m_bbox.getPMin()[0], m_bbox.getPMin()[1], m_bbox.getPMin()[2]};

  m_mat_vol_to_world = glm::scale(glm::fmat4{1.0f}, bbox_dimensions);
  m_mat_vol_to_world = glm::translate(glm::fmat4{1.0f}, bbox_translation) * m_mat_vol_to_world;
  // auto world_to_vol(glm::inverse(vol_to_world));
  auto volume_res = m_cv->getVolumeRes();
  m_program->setUniform("VolumeDimensions", glm::fvec3{volume_res});
  m_program->setUniform("vol_to_world", m_mat_vol_to_world);

  m_program_integration->setUniform("volume_tsdf", start_image_unit);
  m_program_integration->setUniform("kinect_colors",1);
  m_program_integration->setUniform("kinect_depths",2);
  m_program_integration->setUniform("kinect_qualities",3);

  m_program_integration->setUniform("num_kinects", m_num_kinects);
  m_program_integration->setUniform("res_depth", glm::uvec2{m_cf->getWidth(), m_cf->getHeight()});
  m_program_integration->setUniform("res_tsdf", volume_res);
  m_program_integration->setUniform("limit", limit);
  m_program->setUniform("limit", limit);

  m_volume_tsdf = globjects::Texture::createDefault(GL_TEXTURE_3D);
  std::vector<float> empty_tsdf(volume_res.x * volume_res.y * volume_res.z, -limit);
  m_volume_tsdf->image3D(0, GL_R32F, volume_res.x, volume_res.y, volume_res.z, 0, GL_RED, GL_FLOAT, empty_tsdf.data());
  m_volume_tsdf->bindActive(GL_TEXTURE0 + 29);
}

ReconIntegration::~ReconIntegration() {
  m_program->destroy();
  m_program_integration->destroy();
}

void ReconIntegration::draw(){
  integrate();

  m_program->use();

  gloost::Matrix modelview;
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview.data());
  glm::fmat4 model_view{modelview};
  glm::fmat4 normal_matrix = glm::inverseTranspose(model_view * m_mat_vol_to_world);
  m_program->setUniform("NormalMatrix", normal_matrix);
  // upload camera pos in volume space for correct raycasting dir 
  glm::fvec4 camera_world{glm::inverse(model_view) * glm::fvec4{0.0f, 0.0f, 0.0f, 1.0f}};
  glm::vec3 camera_texturespace{glm::inverse(m_mat_vol_to_world) * camera_world};

  m_program->setUniform("CameraPos", camera_texturespace);

  UnitCube::draw();

  m_program->release();  
}

void ReconIntegration::integrate() {
  
  glEnable(GL_RASTERIZER_DISCARD);
  m_program_integration->use();

  m_volume_tsdf->bindImageTexture(start_image_unit, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);

  m_sampler.sample();

  m_program_integration->release();
  glDisable(GL_RASTERIZER_DISCARD);
}

}