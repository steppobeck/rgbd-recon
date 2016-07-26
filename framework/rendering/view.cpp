#include "view.hpp"

#include <glbinding/gl/gl.h>
using namespace gl;
#include <globjects/Framebuffer.h>
#include <globjects/Texture.h>
#include <iostream>

namespace kinect {

View::View(unsigned width, unsigned height, bool depth)
 :m_resolution{width, height}
 ,m_color_clear{0.0f}
 ,m_fbo{new globjects::Framebuffer()}
 ,m_tex_color{globjects::Texture::createDefault(GL_TEXTURE_2D)}
 ,m_tex_depth{depth ? globjects::Texture::createDefault(GL_TEXTURE_2D) : nullptr}
 ,m_current_fbo(0)
 ,m_viewport_current(0,0,width, height)
{
  setResolution(width, height);

  m_fbo->setDrawBuffers({GL_COLOR_ATTACHMENT0});
}

void View::setResolution(unsigned width, unsigned height) {
  m_resolution = glm::uvec2{width, height};
  std::vector<float> test(m_resolution.x * m_resolution.y * 4, 0.5f);
  m_tex_color->image2D(0, GL_RGBA32F, m_resolution.x, height, 0, GL_RGBA, GL_FLOAT, test.data());
  
  m_fbo->attachTexture(GL_COLOR_ATTACHMENT0, m_tex_color);
  
  if(m_tex_depth.get() != nullptr) {
    std::vector<float> test2(m_resolution.x * m_resolution.y, 0.5f);
    m_tex_depth->image2D(0, GL_DEPTH_COMPONENT32, m_resolution.x, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, test2.data());    

    m_tex_depth->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    m_tex_depth->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    m_fbo->attachTexture(GL_DEPTH_ATTACHMENT, m_tex_depth);
  }
}

void View::enable(bool clear_color, bool clear_depth) {
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_current_fbo);

  m_fbo->bind();

  unsigned x;
  unsigned y;
  unsigned w;
  unsigned h;
  getWidthHeight(x,y,w,h);
  m_viewport_current.set(x,y,w,h);
  glViewport(0, 0, m_resolution.x, m_resolution.y);
  if(clear_color) {
    m_fbo->clearBuffer(GL_COLOR, 0, m_color_clear);
  }
  if(clear_depth && m_tex_depth.get() != nullptr) {
    glClear(GL_DEPTH_BUFFER_BIT);
  }
}

void View::disable(){
  glBindFramebuffer(GL_FRAMEBUFFER, m_current_fbo);
  m_viewport_current.enter(false);
}

void View::bindToTextureUnits(unsigned start_texture_unit){
  bindToTextureUnitRGBA(start_texture_unit);
  bindToTextureUnitDepth(start_texture_unit + 1);
}

void View::bindToTextureUnitDepth(unsigned start_texture_unit){
  if (m_tex_depth.get() != nullptr) {
    // m_tex_depth->bindActive(start_texture_unit);
    glActiveTexture(GL_TEXTURE0 + start_texture_unit);
    m_tex_depth->bind();
  }
}

void View::bindToTextureUnitRGBA(unsigned start_texture_unit){
  // m_tex_color->bindActive(start_texture_unit);
  glActiveTexture(GL_TEXTURE0 + start_texture_unit);
  m_tex_color->bind();
}

glm::uvec2 const& View::resolution() const {
  return m_resolution;
}

void View::setClearColor(glm::fvec4 const& color) {
  m_color_clear = color;
}

globjects::Texture const* View::getColorTex() const {
  return m_tex_color;
}

void View::getWidthHeight(unsigned& x, unsigned& y, unsigned& width, unsigned& height){
  GLsizei vp_params[4];
  glGetIntegerv(GL_VIEWPORT,vp_params);
  x = vp_params[0];
  y = vp_params[1];
  width  = vp_params[2];
  height = vp_params[3];
}
}