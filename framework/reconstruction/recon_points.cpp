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

ReconPoints::ReconPoints(CalibrationFiles const& cfs, CalibVolume const* cv)
 :Reconstruction(cfs, cv)
 ,m_shader()
 ,m_uniforms()
{
  m_uniforms.set_int("kinect_colors", 0);
  m_uniforms.set_int("kinect_depths", 1);

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
  viewport_scale.setScale(width * 0.5, height * 0.5, 0.5f);
  gloost::Matrix image_to_eye =  viewport_scale * viewport_translate * projection_matrix;
  image_to_eye.invert();

  glEnable(GL_DEPTH_TEST);
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  // m_va.enable(0, false, &ox, &oy, false);
  for(unsigned layer = 0; layer < m_num_kinects; ++layer){
    m_uniforms.set_int("layer",  layer);
    m_uniforms.set_int("cv_xyz",m_cv->getStartTextureUnit() + layer * 2);
    m_uniforms.set_int("cv_uv",m_cv->getStartTextureUnit() + layer * 2 + 1);
    m_uniforms.set_float("cv_min_d",m_cv->m_cv_min_ds[layer]);
    m_uniforms.set_float("cv_max_d",m_cv->m_cv_max_ds[layer]);
    {
    	glPushMatrix();
    	{
    	  m_shader.set();
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
    	  m_shader.disable();
    	}
    	glPopMatrix();
    }
    
  }
  glPopAttrib();
}

void
ReconPoints::reload(){
  m_shader = gloost::Shader("glsl/points.vs", "glsl/points.fs", "glsl/points.gs");
  m_shader.setProgramParameter(GL_GEOMETRY_INPUT_TYPE_EXT ,GL_POINTS);
  m_shader.setProgramParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT ,GL_POINTS);
  m_shader.setProgramParameter(GL_GEOMETRY_VERTICES_OUT_EXT ,1);
}

}