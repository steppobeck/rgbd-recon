#include "recon_points.hpp"

#include "calibration_files.hpp"
#include <KinectCalibrationFile.h>
#include <CalibVolume.h>

#include <Matrix.h>
#include <glm/gtc/type_precision.hpp>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/Shader.h>

namespace kinect{

void
static getWidthHeight(unsigned& width, unsigned& height){
  GLsizei vp_params[4];
  glGetIntegerv(GL_VIEWPORT,vp_params);
  width  = vp_params[2];
  height = vp_params[3];
}

ReconPoints::ReconPoints(CalibrationFiles const& cfs, CalibVolume const* cv, gloost::BoundingBox const&  bbox)
 :Reconstruction(cfs, cv, bbox)
 ,m_point_grid{new globjects::VertexArray()}
 ,m_point_buffer{new globjects::Buffer()}
 ,m_program{new globjects::Program()}
{
  m_program->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/points.vs"),
    globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/points.fs"),
    globjects::Shader::fromFile(GL_GEOMETRY_SHADER, "glsl/points.gs")
  );

  m_program->setUniform("kinect_colors", 1);
  m_program->setUniform("kinect_depths", 2);
  m_program->setUniform("kinect_qualities", 3);
  m_program->setUniform("bbox_min",m_bbox.getPMin());
  m_program->setUniform("bbox_max",m_bbox.getPMax());

  std::vector<glm::fvec2> data{};
  float stepX = 1.0f / m_tex_width;
  float stepY = 1.0f / m_tex_height;
  for(unsigned y = 0; y < m_tex_height; ++y) {
    for(unsigned x = 0; x < m_tex_width; ++x) {
      data.emplace_back((x+0.5) * stepX, (y + 0.5) * stepY);
    }
  }
  m_point_buffer->setData(data, GL_STATIC_DRAW);

  m_point_grid->enable(0);
  m_point_grid->binding(0)->setAttribute(0);
  m_point_grid->binding(0)->setBuffer(m_point_buffer, 0, sizeof(glm::fvec2));
  m_point_grid->binding(0)->setFormat(2, GL_FLOAT);

  reload();
}

ReconPoints::~ReconPoints() {
  m_point_grid->destroy();
  m_point_buffer->destroy();
  m_program->destroy();
}

void
ReconPoints::draw(){
  // calculate img_to_eye for this view
  gloost::Matrix projection_matrix;
  glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix.data());
  gloost::Matrix viewport_translate;
  viewport_translate.setIdentity();
  viewport_translate.setTranslate(1.0,1.0,1.0);
  gloost::Matrix viewport_scale;
  viewport_scale.setIdentity();
  unsigned width  = 0;
  unsigned height = 0;
  getWidthHeight(width, height);

  viewport_scale.setScale(width * 0.5, height * 0.5, 0.5f);
  gloost::Matrix image_to_eye =  viewport_scale * viewport_translate * projection_matrix;
  image_to_eye.invert();
  projection_matrix.invert();

  glEnable(GL_DEPTH_TEST);

  m_program->setUniform("viewportSizeInv", glm::fvec2(1.0f/width, 1.0f/height));
  m_program->setUniform("img_to_eye_curr", image_to_eye);
  m_program->setUniform("projection_inv", projection_matrix);
  m_program->setUniform("epsilon" , 0.075f);

  gloost::Matrix modelview_matrix;
  gloost::Matrix modelview_inv;
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix.data());
  modelview_inv = modelview_matrix;
  modelview_inv.invert();

  m_program->setUniform("modelview_inv", modelview_inv);
  m_program->use();

  for(unsigned layer = 0; layer < m_num_kinects; ++layer) {
    m_program->setUniform("layer",  layer);
    m_program->setUniform("cv_xyz", int(m_cv->getStartTextureUnit() + layer * 2));
    m_program->setUniform("cv_uv", int(m_cv->getStartTextureUnit() + layer * 2 + 1));
    m_program->setUniform("cv_min_d",m_cv->m_cv_min_ds[layer]);
    m_program->setUniform("cv_max_d",m_cv->m_cv_max_ds[layer]);

    m_point_grid->drawArrays(GL_POINTS, 0, m_tex_width * m_tex_height);
  }
  m_program->release();
}

}