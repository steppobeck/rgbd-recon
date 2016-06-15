#include "recon_integration.hpp"

#include "calibration_files.hpp"
#include "unit_cube.hpp"
#include "screen_quad.hpp"
#include <KinectCalibrationFile.h>
#include "CalibVolumes.hpp"
#include "view_lod.hpp"
#include "texture_blitter.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/io.hpp>

#include <glbinding/gl/gl.h>
using namespace gl;
#include <globjects/Buffer.h>
#include <globjects/Program.h>
#include <globjects/VertexArray.h>
#include <globjects/Texture.h>
#include <globjects/Framebuffer.h>

#include <globjects/Shader.h>
#include <globjects/globjects.h>

namespace kinect{

static int start_image_unit = 3;

ReconIntegration::ReconIntegration(CalibrationFiles const& cfs, CalibVolumes const* cv, gloost::BoundingBox const&  bbox, float limit, float size)
 :Reconstruction(cfs, cv, bbox)
 ,m_program{new globjects::Program()}
 ,m_program_integration{new globjects::Program()}
 ,m_program_inpaint{new globjects::Program()}
 ,m_program_transfer{new globjects::Program()}
 ,m_res_volume{glm::ceil(glm::fvec3{bbox.getPMax()[0] - bbox.getPMin()[0],
                                    bbox.getPMax()[1] - bbox.getPMin()[1],
                                    bbox.getPMax()[2] - bbox.getPMin()[2]} / size)}
 ,m_sampler{m_res_volume}
 ,m_volume_tsdf{globjects::Texture::createDefault(GL_TEXTURE_3D)}
 ,m_mat_vol_to_world{1.0f}
 ,m_limit{limit}
 ,m_voxel_size{size}
 ,m_timer_integration{}
{
  m_program->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/tsdf_raymarch.vs"),
    globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/tsdf_raymarch.fs")
  );

  glm::fvec3 bbox_dimensions = glm::fvec3{m_bbox.getPMax()[0] - m_bbox.getPMin()[0],
                                          m_bbox.getPMax()[1] - m_bbox.getPMin()[1],
                                          m_bbox.getPMax()[2] - m_bbox.getPMin()[2]};
  glm::fvec3 bbox_translation = glm::fvec3{m_bbox.getPMin()[0], m_bbox.getPMin()[1], m_bbox.getPMin()[2]};

  m_mat_vol_to_world = glm::scale(glm::fmat4{1.0f}, bbox_dimensions);
  m_mat_vol_to_world = glm::translate(glm::fmat4{1.0f}, bbox_translation) * m_mat_vol_to_world;

  m_program->setUniform("vol_to_world", m_mat_vol_to_world);
  m_program->setUniform("kinect_colors",1);
  m_program->setUniform("kinect_depths",2);
  m_program->setUniform("kinect_qualities",3);
  m_program->setUniform("kinect_normals",4);
  m_program->setUniform("cv_xyz_inv", m_cv->getXYZVolumeUnitsInv());
  m_program->setUniform("cv_uv", m_cv->getUVVolumeUnits());
  m_program->setUniform("camera_positions", m_cv->getCameraPositions());
  m_program->setUniform("num_kinects", m_num_kinects);
  m_program->setUniform("limit", m_limit);
  
  m_program_integration->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/tsdf_integration.vs")
  );
  m_program_integration->setUniform("cv_xyz_inv", m_cv->getXYZVolumeUnitsInv());
  m_program->setUniform("volume_tsdf", 29);

  m_program_integration->setUniform("volume_tsdf", start_image_unit);
  m_program_integration->setUniform("kinect_colors",1);
  m_program_integration->setUniform("kinect_depths",2);
  m_program_integration->setUniform("kinect_qualities",3);
  m_program_integration->setUniform("kinect_normals",4);
  m_program_integration->setUniform("kinect_silhouettes",5);

  m_program_integration->setUniform("num_kinects", m_num_kinects);
  m_program_integration->setUniform("res_depth", glm::uvec2{m_cf->getWidth(), m_cf->getHeight()});
  m_program_integration->setUniform("res_tsdf", m_res_volume);
  m_program_integration->setUniform("limit", m_limit);

  m_volume_tsdf->image3D(0, GL_R32F, glm::ivec3{m_res_volume}, 0, GL_RED, GL_FLOAT, nullptr);
  m_volume_tsdf->bindActive(GL_TEXTURE0 + 29);
  std::cout << "resolution " << m_res_volume.x << ", " << m_res_volume.y << ", " << m_res_volume.z << std::endl;
  
  m_program_inpaint->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/texture_passthrough.vs"),
    globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/tsdf_inpaint.fs")
  );
  m_program_inpaint->setUniform("texture_color", 15);
  m_program_inpaint->setUniform("texture_depth", 16);

  m_program_transfer->attach(
   globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/texture_passthrough.vs")
  ,globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/framebuffer_transfer.fs")
  );
  m_program_transfer->setUniform("texture_color", 15);
  m_program_transfer->setUniform("texture_depth", 16);
}

void ReconIntegration::drawF() {
  m_timer_integration.begin();
  integrate();
  m_timer_integration.end();
  Reconstruction::drawF();
  fillColors();
}

void ReconIntegration::draw(){
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

  m_view_inpaint->enable(1);
  UnitCube::draw();
  m_view_inpaint->disable();

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

void ReconIntegration::fillColors() {
  m_view_inpaint->bindToTextureUnits(15);

  m_view_inpaint2->enable(0);
  m_program_transfer->use();
  m_program_transfer->setUniform("resolution_tex", m_view_inpaint->resolution(1));
  m_program_transfer->setUniform("lod", 1);
  ScreenQuad::draw();
  m_view_inpaint2->disable();
  m_program_transfer->release();

  m_view_inpaint2->bindToTextureUnits(15);

  m_program_transfer->use();
  m_program_transfer->setUniform("resolution_tex", m_view_inpaint2->resolution(0));
  m_program_transfer->setUniform("lod", 0);

  ScreenQuad::draw();
  m_program_transfer->release();  
}

void ReconIntegration::setVoxelSize(float size) {
  m_voxel_size = size;
  m_res_volume = glm::ceil(glm::fvec3{m_bbox.getPMax()[0] - m_bbox.getPMin()[0],
                                      m_bbox.getPMax()[1] - m_bbox.getPMin()[1],
                                      m_bbox.getPMax()[2] - m_bbox.getPMin()[2]} / m_voxel_size);
  m_sampler.resize(m_res_volume);
  m_program_integration->setUniform("res_tsdf", m_res_volume);
  m_volume_tsdf->image3D(0, GL_R32F, glm::ivec3{m_res_volume}, 0, GL_RED, GL_FLOAT, nullptr);
  m_volume_tsdf->bindActive(GL_TEXTURE0 + 29);
  std::cout << "resolution " << m_res_volume.x << ", " << m_res_volume.y << ", " << m_res_volume.z << std::endl;
}
void ReconIntegration::setTsdfLimit(float limit) {
  m_limit = limit;
  m_program->setUniform("limit", m_limit);
  m_program_integration->setUniform("limit", m_limit);
}

std::uint64_t ReconIntegration::integrationTime() const {
  return m_timer_integration.duration();
}

void ReconIntegration::resize(std::size_t width, std::size_t height) {
  m_view_inpaint = std::unique_ptr<ViewLod>{new ViewLod(width, height, 4)};
  m_view_inpaint2 = std::unique_ptr<ViewLod>{new ViewLod(width, height, 4)};
}

}