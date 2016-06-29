#include "view_lod.hpp"

#include <glbinding/gl/gl.h>
using namespace gl;
#include <globjects/Framebuffer.h>
#include <globjects/Texture.h>
#include <iostream>

namespace kinect {

ViewLod::ViewLod(unsigned width, unsigned height)
  :m_resolution_full{0}
   ,m_fbo{new globjects::Framebuffer()}
   ,m_tex_color{globjects::Texture::createDefault(GL_TEXTURE_2D)}
   ,m_tex_depth{globjects::Texture::createDefault(GL_TEXTURE_2D)}
   ,m_current_fbo(0)
   ,m_viewport_current(0,0,width, height)
{
  setResolution(width, height);

  m_fbo->setDrawBuffers({GL_COLOR_ATTACHMENT0});
}

void ViewLod::setResolution(unsigned width, unsigned height) {
  unsigned num_lods = 1 + glm::floor(glm::log2(float(glm::min(width, height))));
  m_resolutions.resize(num_lods);
  m_offsets.resize(num_lods);
  // the resolution of the full texture
  m_resolution_full = glm::uvec2{width * 1.5f, height};
  // m_resolutions.clear();
  std::vector<float> test(m_resolution_full.x * m_resolution_full.y * 4, 0.5f);
  m_tex_color->image2D(0, GL_RGBA32F, m_resolution_full.x, height, 0, GL_RGBA, GL_FLOAT, test.data());
  // std::cout << "lod " << i << " res "<< m_resolution_full.x << ", " << m_resolution_full.y << std::endl; 
  std::vector<float> test2(m_resolution_full.x * m_resolution_full.y, 0.5f);
  m_tex_depth->image2D(0, GL_DEPTH_COMPONENT32, m_resolution_full.x, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, test2.data());    

  glm::uvec2 offset{width, height};
  for(unsigned i = 0; i < num_lods; ++i) {
    glm::uvec2 lod_res{glm::floor(width / glm::pow(2.0f, float(i))), glm::floor(height / glm::pow(2.0f, float(i)))};
    m_resolutions[i] = lod_res;

    if(i > 0) {
      offset.y -= lod_res.y;
      m_offsets[i] = offset;
    }
    else {
      m_offsets[i] = glm::uvec2{0};
    } 
    // std::cout << "offset " << offset.x << ", " << offset.y << " resolution " << lod_res.x << ", " << lod_res.y << std::endl;
  }

  m_tex_color->setParameter(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  m_tex_color->setParameter(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  m_tex_depth->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  m_tex_depth->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  m_fbo->attachTexture(GL_COLOR_ATTACHMENT0, m_tex_color);
  m_fbo->attachTexture(GL_DEPTH_ATTACHMENT, m_tex_depth);
  glMemoryBarrier(GL_ALL_BARRIER_BITS);

}
void ViewLod::enable(unsigned lod, bool clear_color, bool clear_depth) {
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_current_fbo);

  m_fbo->bind();

  unsigned x;
  unsigned y;
  unsigned w;
  unsigned h;
  getWidthHeight(x,y,w,h);
  m_viewport_current.set(x,y,w,h);
  glViewport(m_offsets[lod].x, m_offsets[lod].y, m_resolutions[lod].x, m_resolutions[lod].y);
  // std::cout << "lod " << lod << " viewport " << m_offsets[lod].x << ", " << m_offsets[lod].y << ", " << m_resolutions[lod].x << ", " << m_resolutions[lod].y << std::endl;
  if(clear_color) {
    glClearColor(0.0,1.0,0.0,0.0);
    glClear(GL_COLOR_BUFFER_BIT);
  }
  if(clear_depth) {
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

glm::uvec2 const& ViewLod::resolution(unsigned i) const {
  return m_resolutions[i];
}

std::vector<glm::uvec2> const& ViewLod::resolutions() const {
  return m_resolutions;
}

glm::uvec2 const& ViewLod::resolution_full() const {
  return m_resolution_full;
}

glm::uvec2 const& ViewLod::offset(unsigned i) const {
  return m_offsets[i];
}

std::vector<glm::uvec2> const& ViewLod::offsets() const {
  return m_offsets;
}

unsigned ViewLod::numLods() const {
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