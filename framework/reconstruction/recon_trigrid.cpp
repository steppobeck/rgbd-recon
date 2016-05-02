#include "recon_trigrid.hpp"

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

ReconTrigrid::ReconTrigrid(CalibrationFiles const& cfs, CalibVolume const* cv)
 :Reconstruction(cfs, cv)
 ,m_shader_pass_depth()
 ,m_shader_pass_accum()
 ,m_shader_pass_normalize()
 ,m_uniforms_pass_depth()
 ,m_uniforms_pass_accum()
 ,m_uniforms_pass_normalize()
 ,m_proxyMesh()
 ,m_va_pass_depth()
 ,m_va_pass_accum()
{
  m_proxyMesh = std::unique_ptr<mvt::ProxyMeshGridV2>{new mvt::ProxyMeshGridV2(m_tex_width,
           m_tex_height)};

  m_uniforms_pass_depth = std::unique_ptr<gloost::UniformSet>{new gloost::UniformSet};
  m_uniforms_pass_depth->set_int("kinect_colors",0);
  m_uniforms_pass_depth->set_int("kinect_depths",1);


  m_uniforms_pass_accum = std::unique_ptr<gloost::UniformSet>{new gloost::UniformSet};
  m_uniforms_pass_accum->set_int("kinect_colors",0);
  m_uniforms_pass_accum->set_int("kinect_depths",1);
  m_uniforms_pass_accum->set_int("depth_map_curr",12);

  m_uniforms_pass_normalize = std::unique_ptr<gloost::UniformSet>{new gloost::UniformSet};
  m_uniforms_pass_normalize->set_int("color_map",13);
  m_uniforms_pass_normalize->set_int("depth_map",14);

  reload();
  
  m_va_pass_depth = std::unique_ptr<mvt::ViewArray>{new mvt::ViewArray(1920,1200, 1)};
  m_va_pass_depth->init();

  m_va_pass_accum = std::unique_ptr<mvt::ViewArray>{new mvt::ViewArray(1920,1200, 1)};
  m_va_pass_accum->init();
}

void
ReconTrigrid::draw(){
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

  const float min_length = m_min_length;
  //std::cerr << "min_length: " << min_length << std::endl;

  unsigned ox;
  unsigned oy;

  // pass 1 goes to depth buffer only
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  m_va_pass_depth->enable(0, false, &ox, &oy, false);
  m_uniforms_pass_depth->set_float("min_length", min_length);
  m_uniforms_pass_depth->set_int("stage", 0);
  for(unsigned layer = 0; layer < m_num_kinects; ++layer){
    m_uniforms_pass_depth->set_int("layer",  layer);
    m_uniforms_pass_depth->set_vec2("tex_size_inv", gloost::vec2(1.0f/m_tex_width, 1.0f/m_tex_height));
    m_uniforms_pass_depth->set_int("cv_xyz",m_cv->getStartTextureUnit() + layer * 2);
    m_uniforms_pass_depth->set_int("cv_uv",m_cv->getStartTextureUnit() + layer * 2 + 1);
    m_uniforms_pass_depth->set_float("cv_min_d",m_cv->m_cv_min_ds[layer]);
    m_uniforms_pass_depth->set_float("cv_max_d",m_cv->m_cv_max_ds[layer]);
    {
    	glDisable(GL_CULL_FACE);
    	glPushMatrix();
    	{
    	  m_shader_pass_depth->set();
    	  m_uniforms_pass_depth->applyToShader(m_shader_pass_depth.get());
    	  
    	  
    	  m_proxyMesh->draw();
    	  m_shader_pass_depth->disable();
    	}
    	glPopMatrix();
    }
    //glPopAttrib();
    
  }
  m_va_pass_depth->disable(false);
  glPopAttrib();
  // end pass 1
#if 1
  // pass 2 goes to accumulation buffer
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND); // glEnablei(GL_BLEND, GL_COLOR_ATTACHMENT0_EXT);
  glBlendFuncSeparateEXT(GL_ONE,GL_ONE,GL_ONE,GL_ONE);
  glBlendEquationSeparateEXT(GL_FUNC_ADD, GL_FUNC_ADD);
  m_va_pass_accum->enable(0, false, &ox, &oy);
  m_uniforms_pass_accum->set_int("stage", 1);
  m_uniforms_pass_accum->set_float("min_length", min_length);
  m_uniforms_pass_accum->set_vec2("viewportSizeInv", gloost::vec2(1.0f/m_va_pass_depth->getWidth(), 1.0f/m_va_pass_depth->getHeight()));
  m_uniforms_pass_accum->set_vec2("offset"         , gloost::vec2(1.0f*ox,                          1.0f*oy));
  m_uniforms_pass_accum->set_mat4("img_to_eye_curr", image_to_eye);
  m_uniforms_pass_accum->set_float("epsilon"    , 0.075);
  m_va_pass_depth->bindToTextureUnitDepth(12);

  for(unsigned layer = 0; layer < m_num_kinects; ++layer){
    m_uniforms_pass_accum->set_int("layer",  layer);
    m_uniforms_pass_accum->set_vec2("tex_size_inv", gloost::vec2(1.0f/m_tex_width, 1.0f/m_tex_height));

    m_uniforms_pass_accum->set_int("cv_xyz",m_cv->getStartTextureUnit() + layer * 2);
    m_uniforms_pass_accum->set_int("cv_uv",m_cv->getStartTextureUnit() + layer * 2 + 1);
    m_uniforms_pass_accum->set_float("cv_min_d",m_cv->m_cv_min_ds[layer]);
    m_uniforms_pass_accum->set_float("cv_max_d",m_cv->m_cv_max_ds[layer]);
    {
    	glDisable(GL_CULL_FACE);
    	glPushMatrix();
    	{
    	  m_shader_pass_accum->set();
    	  m_uniforms_pass_accum->applyToShader(m_shader_pass_accum.get());
    	  

    	  m_proxyMesh->draw();
    	  m_shader_pass_accum->disable();
    	}
      glPopMatrix();
    }      
  }

  m_va_pass_accum->disable(false);
  glDisable(GL_BLEND);
  glPopAttrib();
  // end pass 2
#endif

#if 1
  // normalize pass outputs best quality color and depth to framebuffer of parent renderstage
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.0,1.0,0.0,1.0,1.0,-1.0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  m_shader_pass_normalize->set();
  m_uniforms_pass_normalize->set_vec2("texSizeInv", gloost::vec2(1.0f/m_va_pass_depth->getWidth(), 1.0f/m_va_pass_depth->getHeight()));
  m_uniforms_pass_normalize->set_vec2("offset"    , gloost::vec2(1.0f*ox,                          1.0f*oy));
  
  m_uniforms_pass_normalize->applyToShader(m_shader_pass_normalize.get());

  m_va_pass_accum->bindToTextureUnits(13);

  glBegin(GL_QUADS);
  {
    glVertex3f  (0.0f, 0.0f, 0.0f);
    glVertex3f  (1.0f, 0.0f, 0.0f);
    glVertex3f  (1.0f, 1.0f, 0.0f);
    glVertex3f  (0.0f, 1.0f, 0.0f);
  }
  glEnd();

  m_shader_pass_normalize->disable();
  
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glPopAttrib();
  // end pass normalize
#endif
}

void
ReconTrigrid::reload(){
  m_shader_pass_depth.reset(new gloost::Shader("glsl/ksv3_vertex.vs",
				     "glsl/ksv3_fragment.fs",
				     "glsl/ksv3_geometry.gs"));
  m_shader_pass_depth->setProgramParameter(GL_GEOMETRY_INPUT_TYPE_EXT ,GL_TRIANGLES);
  m_shader_pass_depth->setProgramParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT ,GL_TRIANGLE_STRIP);
  m_shader_pass_depth->setProgramParameter(GL_GEOMETRY_VERTICES_OUT_EXT ,3);


  m_shader_pass_accum.reset(new gloost::Shader("glsl/ksv3_vertex.vs",
				     "glsl/ksv3_fragment.fs",
				     "glsl/ksv3_geometry.gs"));
  m_shader_pass_accum->setProgramParameter(GL_GEOMETRY_INPUT_TYPE_EXT ,GL_TRIANGLES);
  m_shader_pass_accum->setProgramParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT ,GL_TRIANGLE_STRIP);
  m_shader_pass_accum->setProgramParameter(GL_GEOMETRY_VERTICES_OUT_EXT ,3);

  m_shader_pass_normalize.reset(new gloost::Shader("glsl/pass_normalize.vs",
					 "glsl/pass_normalize.fs"));
}

}