#ifndef MVT_TEXTUREARRAY_H
#define MVT_TEXTUREARRAY_H

#include <glbinding/gl/enum.h>
using namespace gl;

#include <globjects/base/ref_ptr.h>
namespace globjects {
  class Texture;
}

namespace kinect {

class TextureArray {

 public:
/* kinect color: GL_RGB32F, GL_RGB, GL_FLOAT*/
/* kinect depth: GL_LUMINANCE32F_ARB, GL_RED, GL_FLOAT*/
/* opengl color: GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE*/
/* opengl depth: GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT*/
  TextureArray(unsigned width, unsigned height, unsigned depth,
    	 GLenum internalFormat/* = GL_RGBA*/, GLenum pixelFormat /* = GL_RGBA*/, GLenum pixelType /* = GL_FLOAT*/, unsigned storage = 0);

  // void fillLayer(unsigned layer, void* data);
  // void fillLayers(void* data);
  void fillLayersFromPBO(unsigned id);
  void bind();
  void unbind();

  globjects::Texture* getTexture() const;

  unsigned getGLHandle();

  // bool copyToCPU(unsigned layer, void* data);
  // bool copyToCPUAll(void* data);

  void setMAGMINFilter(GLenum f);

 private:
  unsigned m_width;
  unsigned m_height;
  unsigned m_depth;
  GLenum m_type; /*GL_TEXTURE_2D_ARRAY_EXT*/
  GLenum m_internalFormat;
  GLenum m_pixelFormat;
  GLenum m_pixelType;

  globjects::ref_ptr<globjects::Texture> m_texture;
  unsigned m_storage;
};

}

#endif // #ifndef  MVT_TEXTUREARRAY_H