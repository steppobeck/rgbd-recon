#include "ViewArray.h"

#include <TextureArray.h>
#include <gl_util.h>
#include <stdio.h>
#include <iostream>

namespace mvt{


  ViewArray::ViewArray(unsigned width, unsigned height, unsigned numLayers)
    : m_width(width),
      m_height(height),
      m_numLayers(numLayers),
      m_glHandle(0),
      m_colorArray(0),
      m_depthArray(0),
      m_current_fbo(0),
      m_viewport(0,0,width,height),
      m_viewport_current(0,0,width, height)
  {}


  ViewArray::~ViewArray(){
    delete m_colorArray;
    delete m_depthArray;

    glDeleteFramebuffersEXT(1, &m_glHandle);

  }

  void ViewArray::init() {
    m_colorArray = new TextureArray(m_width, m_height, m_numLayers, GL_RGBA32F, GL_RGBA, GL_FLOAT);

    m_depthArray = new TextureArray(m_width, m_height, m_numLayers, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT);
    m_depthArray->setMAGMINFilter(GL_NEAREST);

    // create fbo
    glGenFramebuffersEXT(1, &m_glHandle);
  }

  void
  ViewArray::enable(unsigned layer, bool use_vp, unsigned* ox, unsigned* oy, bool clearcolor){
    if(0 == m_glHandle)
      init();

    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_current_fbo);
    
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_glHandle);
    GLenum buffers[] = {GL_COLOR_ATTACHMENT0_EXT };
    glDrawBuffers(1, buffers);
    glFramebufferTextureLayerEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, m_colorArray->getGLHandle(), 0, layer);
    glFramebufferTextureLayerEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,  m_depthArray->getGLHandle(), 0, layer);
#if 1
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status){
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            printf("Error:Frame buffer not supported.\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            printf("Error:Frame buffer not supported.\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            printf("Error:Missing color attachment.\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            printf("Error:All textures attached to frame buffer must have same dimension.\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            printf("Error:All textures attached to frame buffer must have same format.\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            printf("Error:Missing draw buffer.\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            printf("Error:Missing read buffer.\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT:
            printf("Error:Missing layer targets.\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_EXT:
            printf("Error:Incomplete layer count.\n");
            break;
    case GL_FRAMEBUFFER_COMPLETE_EXT:
    default:
	  break;
    }
#endif

    if(use_vp){
      m_viewport.enter();
    }
    else{
      unsigned x;
      unsigned y;
      unsigned w;
      unsigned h;
      getWidthHeight(x,y,w,h);
      if(0 == ox || 0 == oy)
	std::cerr << " ViewArray::enable(unsigned layer, bool use_vp, unsigned* ox, unsigned* oy) ERROR" << std::endl;
      *ox = x;
      *oy = y;
      //std::cerr << x << " " << y << " " << w << " " << h << std::endl;
      m_viewport_current.set(x,y,w,h);
      glViewport(0,0,w,h);
    }
    if(clearcolor){
      glClearColor(0.0,0.0,0.0,0.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    else{
      glClear(GL_DEPTH_BUFFER_BIT);
    }

  }

  void
  ViewArray::disable(bool use_vp){
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_current_fbo);
    if(use_vp){
      m_viewport.leave();
    }
    else{
      m_viewport_current.enter(false);
    }
  }

  void
  ViewArray::bindToTextureUnits(unsigned start_texture_unit){
    if(0 == m_glHandle)
      init();

    glActiveTexture(GL_TEXTURE0 + start_texture_unit);
    m_colorArray->bind();
    glActiveTexture(GL_TEXTURE0 + start_texture_unit + 1);
    m_depthArray->bind();
  }

  void
  ViewArray::bindToTextureUnitDepth(unsigned start_texture_unit){
    if(0 == m_glHandle)
      init();
    glActiveTexture(GL_TEXTURE0 + start_texture_unit);
    m_depthArray->bind();
  }

  void
  ViewArray::bindToTextureUnitRGBA(unsigned start_texture_unit){
    if(0 == m_glHandle)
      init();

    glActiveTexture(GL_TEXTURE0 + start_texture_unit);
    m_colorArray->bind();
  }

  unsigned
  ViewArray::getWidth(){
    return m_width;
  }

  unsigned
  ViewArray::getHeight(){
    return m_height;
  }

  unsigned
  ViewArray::getNumLayers(){
    return m_numLayers;
  }

  void
  ViewArray::getWidthHeight(unsigned& x, unsigned& y, unsigned& width, unsigned& height){
    GLsizei vp_params[4];
    glGetIntegerv(GL_VIEWPORT,vp_params);
    x = vp_params[0];
    y = vp_params[1];
    width  = vp_params[2];
    height = vp_params[3];
  }


}
