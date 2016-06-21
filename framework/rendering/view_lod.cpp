#include "view_lod.hpp"

#include <glbinding/gl/gl.h>
using namespace gl;
#include <globjects/Framebuffer.h>
#include <globjects/Texture.h>
#include <iostream>

namespace kinect {

ViewLod::ViewLod(unsigned width, unsigned height, unsigned num_lods)
  :m_width(width),
   m_height(height),
   m_resolutions(num_lods, glm::uvec2{0}),
   m_fbo{new globjects::Framebuffer()},
   m_tex_color{globjects::Texture::createDefault(GL_TEXTURE_2D)},
   m_tex_depth{globjects::Texture::createDefault(GL_TEXTURE_2D)},
   m_current_fbo(0),
   m_viewport_current(0,0,width, height)
{
  setResolution(width, height);


  m_fbo->setDrawBuffers({GL_COLOR_ATTACHMENT0});
}

void ViewLod::setResolution(unsigned width, unsigned height) {
  m_height = height;
  m_width = width;
  // m_resolutions.clear();
  std::vector<float> test2(m_width * 1.5f * m_height, 0.1f);
  std::vector<float> test(m_width * 1.5f * m_height * 4, 1.0f);
  m_tex_color->image2D(0, GL_RGBA32F, width * 1.5f, height, 0, GL_RGBA, GL_FLOAT, test.data());
  // std::cout << "lod " << i << " res "<< m_width << ", " << m_height << std::endl; 
  m_tex_depth->image2D(0, GL_DEPTH_COMPONENT32, width * 1.5f, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, test2.data());    
  for(unsigned i = 0; i < m_resolutions.size(); ++i) {
    // glm::uvec2 lod_res{width, height};
    glm::uvec2 lod_res{glm::floor(width / glm::pow(2.0f, float(i))), glm::floor(height / glm::pow(2.0f, float(i)))};
    // lod_res = glm::max(glm::uvec2{64}, lod_res);
    m_resolutions[i] = lod_res;
  }
  // m_tex_color->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  // m_tex_color->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  // m_tex_color->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  m_tex_depth->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  m_tex_depth->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // set mipmapping values to complete textures
  // m_tex_color->setParameter(GL_TEXTURE_BASE_LEVEL, 0);
  // m_tex_color->setParameter(GL_TEXTURE_MAX_LEVEL, int(m_resolutions.size() - 1));
  // m_tex_depth->setParameter(GL_TEXTURE_BASE_LEVEL, 0);
  // m_tex_depth->setParameter(GL_TEXTURE_MAX_LEVEL, int(m_resolutions.size() - 1));
  glMemoryBarrier(GL_ALL_BARRIER_BITS);

}
void ViewLod::enable(unsigned lod, bool clear_color) {
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_current_fbo);

  m_fbo->bind();
  m_fbo->attachTexture(GL_COLOR_ATTACHMENT0, m_tex_color);
  m_fbo->attachTexture(GL_DEPTH_ATTACHMENT, m_tex_depth);
  // m_fbo->printStatus();
  unsigned x;
  unsigned y;
  unsigned w;
  unsigned h;
  getWidthHeight(x,y,w,h);
  m_viewport_current.set(x,y,w,h);
  glViewport(0, 0, m_resolutions[lod].x, m_resolutions[lod].y);

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
  bindToTextureUnitRGBA(start_texture_unit);
  bindToTextureUnitDepth(start_texture_unit + 1);
}

void ViewLod::bindToTextureUnitDepth(unsigned start_texture_unit){
  // m_tex_depth->bindActive(start_texture_unit);
  glActiveTexture(GL_TEXTURE0 + start_texture_unit);
  m_tex_depth->bind();
}

void ViewLod::bindToTextureUnitRGBA(unsigned start_texture_unit){
  // m_tex_color->bindActive(start_texture_unit);
  glActiveTexture(GL_TEXTURE0 + start_texture_unit);
  m_tex_color->bind();
}

unsigned ViewLod::getWidth(unsigned i) const {
  return m_resolutions[i].x;
}

unsigned ViewLod::getHeight(unsigned i) const {
  return m_resolutions[i].y;
}

glm::uvec2 ViewLod::resolution(unsigned i) const {
  return m_resolutions[i];
}

unsigned ViewLod::getNumLods() const {
  return m_resolutions.size();
}

globjects::Texture const* ViewLod::getColorTex() const {
  return m_tex_color;
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