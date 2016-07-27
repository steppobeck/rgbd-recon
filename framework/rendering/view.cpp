#include "view.hpp"

#include <glbinding/gl/gl.h>
using namespace gl;
#include <globjects/Framebuffer.h>
#include <globjects/Texture.h>
#include <iostream>

namespace kinect {

GLenum internal_to_transfer(GLenum internal);

View::View(unsigned width, unsigned height, bool depth)
 :View{width, height, {GL_RGBA32F}, depth}
{}

View::View(unsigned width, unsigned height, std::vector<GLenum> const& layers, bool depth)
 :m_resolution{width, height}
 ,m_color_clear{0.0f}
 ,m_fbo{new globjects::Framebuffer()}
 ,m_texs_color{}
 ,m_tex_depth{depth ? globjects::Texture::createDefault(GL_TEXTURE_2D) : nullptr}
 ,m_layers{layers}
 ,m_current_fbo(0)
 ,m_viewport_current(0,0,width, height)
{
  std::vector<GLenum> draw_buffers{};
  for (unsigned i = 0; i < m_layers.size(); ++i) {
    m_texs_color.emplace_back(globjects::Texture::createDefault(GL_TEXTURE_2D));
    draw_buffers.emplace_back(GL_COLOR_ATTACHMENT0 + i);
  }
  setResolution(width, height);

  m_fbo->setDrawBuffers(draw_buffers);
}

void View::setResolution(unsigned width, unsigned height) {
  m_resolution = glm::uvec2{width, height};

  for (unsigned i = 0; i < m_layers.size(); ++i) {
    m_texs_color[i]->image2D(0, m_layers[i], width, height, 0, internal_to_transfer(m_layers[i]), GL_UNSIGNED_BYTE, nullptr);
    m_fbo->attachTexture(GL_COLOR_ATTACHMENT0 + i, m_texs_color[i]);
  }
  
  if(m_tex_depth.get() != nullptr) {
    m_tex_depth->image2D(0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);    

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
    for(unsigned i = 1; i < m_layers.size(); ++i) {
      m_fbo->clearBuffer(GL_COLOR, i, glm::fvec4{0.0f});
    }
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
  bindToTextureUnitDepth(start_texture_unit + m_layers.size());
}

void View::bindToTextureUnitDepth(unsigned start_texture_unit){
  if (m_tex_depth.get() != nullptr) {
    // m_tex_depth->bindActive(start_texture_unit);
    glActiveTexture(GL_TEXTURE0 + start_texture_unit);
    m_tex_depth->bind();
  }
}

void View::bindToTextureUnitRGBA(unsigned start_texture_unit){
  // m_texs_color->bindActive(start_texture_unit);
  for (unsigned i = 0; i < m_layers.size(); ++i) {
    glActiveTexture(GL_TEXTURE0 + start_texture_unit + i);
    m_texs_color[i]->bind();
  }
}

glm::uvec2 const& View::resolution() const {
  return m_resolution;
}

void View::setClearColor(glm::fvec4 const& color) {
  m_color_clear = color;
}

globjects::Texture const* View::getColorTex(unsigned i) const {
  return m_texs_color[i];
}

void View::getWidthHeight(unsigned& x, unsigned& y, unsigned& width, unsigned& height){
  GLsizei vp_params[4];
  glGetIntegerv(GL_VIEWPORT,vp_params);
  x = vp_params[0];
  y = vp_params[1];
  width  = vp_params[2];
  height = vp_params[3];
}

GLenum internal_to_transfer(GLenum internal) {
  switch (internal) {
    case GL_RED : 
    case GL_R8 : 
    case GL_R8_SNORM : 
    case GL_R16 : 
    case GL_R16_SNORM : 
    case GL_R16F : 
    case GL_R32F : 
    case GL_R8I : 
    case GL_R8UI : 
    case GL_R16I : 
    case GL_R16UI : 
    case GL_R32I : 
    case GL_R32UI :
      return GL_RED;
    case GL_RG :
    case GL_RG8 : 
    case GL_RG8_SNORM : 
    case GL_RG16 : 
    case GL_RG16_SNORM : 
    case GL_RG16F : 
    case GL_RG32F : 
    case GL_RG8I : 
    case GL_RG8UI : 
    case GL_RG16I : 
    case GL_RG16UI : 
    case GL_RG32I : 
    case GL_RG32UI :
      return GL_RG;
    case GL_RGB :
    case GL_R3_G3_B2 : 
    case GL_RGB4 : 
    case GL_RGB5 : 
    case GL_RGB8 : 
    case GL_RGB8_SNORM : 
    case GL_RGB10 : 
    case GL_RGB12 : 
    case GL_RGB16_SNORM : 
    case GL_RGBA2 : 
    case GL_RGBA4 : 
    case GL_SRGB8 : 
    case GL_RGB16F : 
    case GL_RGB32F : 
    case GL_R11F_G11F_B10F : 
    case GL_RGB9_E5 : 
    case GL_RGB8I : 
    case GL_RGB8UI : 
    case GL_RGB16I : 
    case GL_RGB16UI : 
    case GL_RGB32I : 
    case GL_RGB32UI :
      return GL_RGB;
    case GL_RGBA :
    case GL_RGB5_A1 : 
    case GL_RGBA8 : 
    case GL_RGBA8_SNORM : 
    case GL_RGB10_A2 : 
    case GL_RGB10_A2UI : 
    case GL_RGBA12 : 
    case GL_RGBA16 : 
    case GL_SRGB8_ALPHA8 : 
    case GL_RGBA16F : 
    case GL_RGBA32F : 
    case GL_RGBA8I : 
    case GL_RGBA8UI : 
    case GL_RGBA16I : 
    case GL_RGBA16UI : 
    case GL_RGBA32I : 
    case GL_RGBA32UI :
      return GL_RGBA;
    default :
      return GL_NONE; 
  }
}

}