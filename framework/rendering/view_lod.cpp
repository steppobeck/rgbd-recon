#include "view_lod.hpp"

#include <glbinding/gl/gl.h>
using namespace gl;
#include <globjects/Framebuffer.h>
#include <globjects/Texture.h>

namespace kinect {

ViewLod::ViewLod(unsigned width, unsigned height, unsigned num_lods)
  :m_width(width),
   m_height(height),
   m_num_lods(num_lods),
   m_fbo{new globjects::Framebuffer()},
   m_tex_color{globjects::Texture::createDefault(GL_TEXTURE_2D)},
   m_tex_depth{globjects::Texture::createDefault(GL_TEXTURE_2D)},
   m_current_fbo(0),
   m_viewport_current(0,0,width, height)
{
  m_tex_color->image2D(0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, (void*)nullptr);
  m_tex_depth->image2D(0, GL_DEPTH_COMPONENT32, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, (void*)nullptr);
  m_tex_depth->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  m_tex_depth->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  m_fbo->setDrawBuffers({GL_COLOR_ATTACHMENT0});
}

void ViewLod::enable(unsigned lod, bool clear_color) {
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_current_fbo);

  m_fbo->bind();
  m_fbo->attachTexture(GL_COLOR_ATTACHMENT0, m_tex_color, lod);
  m_fbo->attachTexture(GL_DEPTH_ATTACHMENT, m_tex_depth, lod);
  // m_fbo->printStatus();
  unsigned x;
  unsigned y;
  unsigned w;
  unsigned h;
  getWidthHeight(x,y,w,h);
  m_viewport_current.set(x,y,w,h);
  glViewport(0, 0, m_width, m_height);

  if(clear_color) {
    glClearColor(0.0,0.0,0.0,0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
  else {
    glClear(GL_DEPTH_BUFFER_BIT);
  }
}

void ViewLod::disable(){
  glBindFramebuffer(GL_FRAMEBUFFER, m_current_fbo);
  m_viewport_current.enter(false);
}

void ViewLod::bindToTextureUnits(unsigned start_texture_unit){
  m_tex_color->bindActive(start_texture_unit);
  m_tex_depth->bindActive(start_texture_unit + 1);
}

void ViewLod::bindToTextureUnitDepth(unsigned start_texture_unit){
  m_tex_depth->bindActive(start_texture_unit);
}

void ViewLod::bindToTextureUnitRGBA(unsigned start_texture_unit){
  m_tex_color->bindActive(start_texture_unit);
}

unsigned ViewLod::getWidth(){
  return m_width;
}

unsigned ViewLod::getHeight(){
  return m_height;
}

unsigned ViewLod::getNumLods(){
  return m_num_lods;
}

void ViewLod::getWidthHeight(unsigned& x, unsigned& y, unsigned& width, unsigned& height){
  GLsizei vp_params[4];
  glGetIntegerv(GL_VIEWPORT,vp_params);
  x = vp_params[0];
  y = vp_params[1];
  width  = vp_params[2];
  height = vp_params[3];
}
}