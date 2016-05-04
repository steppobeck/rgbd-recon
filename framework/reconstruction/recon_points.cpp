#include "recon_points.hpp"

#include "calibration_files.hpp"
#include <KinectCalibrationFile.h>
#include <CalibVolume.h>

#include <Matrix.h>

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
 ,m_shader()
 ,m_uniforms()
{
  m_uniforms.set_int("kinect_colors", 1);
  m_uniforms.set_int("kinect_depths", 2);
  m_uniforms.set_int("kinect_qualities", 3);
  m_uniforms.set_vec3("bbox_min",m_bbox.getPMin());
  m_uniforms.set_vec3("bbox_max",m_bbox.getPMax());

  reload();
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
  // std::cout << "got " << width << ", " << height << std::endl;
  // std::cout << "tex " << m_tex_width << ", " << m_tex_height << std::endl;
  viewport_scale.setScale(width * 0.5, height * 0.5, 0.5f);
  gloost::Matrix image_to_eye =  viewport_scale * viewport_translate * projection_matrix;
  image_to_eye.invert();
  projection_matrix.invert();

  glEnable(GL_DEPTH_TEST);
  m_shader.set();
  m_uniforms.set_vec2("viewportSizeInv", gloost::vec2(1.0f/width, 1.0f/height));
  m_uniforms.set_mat4("img_to_eye_curr", image_to_eye);
  m_uniforms.set_mat4("projection_inv", projection_matrix);
  m_uniforms.set_float("epsilon" , 0.075);
  gloost::Matrix modelview_matrix;
  gloost::Matrix modelview_inv;
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix.data());
  modelview_inv = modelview_matrix;
  modelview_inv.invert();
  // std::cout << "orig " << modelview_matrix <<  " inv " << modelview_inv<< std::endl;
  m_uniforms.set_mat4("modelview_inv", modelview_inv);
  gloost::Point3 p {0.5f, 0.25f, -0.15f};
  gloost::Point3 p2 = modelview_matrix * p;
  gloost::Point3 p3 = modelview_inv * p2;
  // std::cout << "vectors " << p << ", " << p2 << ", " << p3 << std::endl;

  for(unsigned layer = 0; layer < m_num_kinects; ++layer) {
    m_uniforms.set_int("layer",  layer);
    m_uniforms.set_int("cv_xyz",m_cv->getStartTextureUnit() + layer * 2);
    m_uniforms.set_int("cv_uv",m_cv->getStartTextureUnit() + layer * 2 + 1);
    m_uniforms.set_float("cv_min_d",m_cv->m_cv_min_ds[layer]);
    m_uniforms.set_float("cv_max_d",m_cv->m_cv_max_ds[layer]);

	  m_uniforms.applyToShader(&m_shader);
    
    glBegin(GL_POINTS);
    const float stepX = 1.0f / m_tex_width;
    const float stepY = 1.0f / m_tex_height;
    for(unsigned y = 0; y < m_tex_height; ++y) {
      for(unsigned x = 0; x < m_tex_width; ++x) {
        glVertex2f( (x+0.5) * stepX, (y + 0.5) * stepY );
      }
    }
	  glEnd();
  }
  m_shader.disable();
}

void
ReconPoints::reload(){
  m_shader = gloost::Shader("glsl/points.vs", "glsl/points.fs", "glsl/points.gs");
  m_shader.setProgramParameter(GL_GEOMETRY_INPUT_TYPE_EXT ,GL_POINTS);
  m_shader.setProgramParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT ,GL_POINTS);
  m_shader.setProgramParameter(GL_GEOMETRY_VERTICES_OUT_EXT ,1);
}

}