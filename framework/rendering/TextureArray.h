#ifndef MVT_TEXTUREARRAY_H
#define MVT_TEXTUREARRAY_H


#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>

namespace mvt{

  class TextureArray{

  public:
    /* kinect color: GL_RGB32F, GL_RGB, GL_FLOAT*/
    /* kinect depth: GL_LUMINANCE32F_ARB, GL_RED, GL_FLOAT*/
    /* opengl color: GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE*/
    /* opengl depth: GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT*/
    TextureArray(unsigned width, unsigned height, unsigned depth,
		 GLint internalFormat/* = GL_RGBA*/, GLenum pixelFormat /* = GL_RGBA*/, GLenum pixelType /* = GL_FLOAT*/, unsigned storage = 0);
    ~TextureArray();
  private:
    void init();
  public:
    void fillLayer(unsigned layer, void* data);
    void fillLayers(void* data);
    void fillLayersFromPBO(unsigned id);
    void bind();
    void unbind();

    GLuint getGLHandle();

    bool copyToCPU(unsigned layer, void* data);
    bool copyToCPUAll(void* data);
    void resetAccumBuffer();

    void setMAGMINFilter(int f);

  private:

    unsigned m_width;
    unsigned m_height;
    unsigned m_depth;
    unsigned m_type; /*GL_TEXTURE_2D_ARRAY_EXT*/
    unsigned m_internalFormat;
    unsigned m_pixelFormat;
    unsigned m_pixelType;
    unsigned m_glHandle;

    // only used if m_pixelType == GL_FLOAT
    std::vector<float*>   m_accumbuffer;
    unsigned m_accumbufferSize;


    unsigned m_storage;

  public:
    static unsigned s_accumbufferSize;

  };


}// namespace mvt



#endif // #ifndef  MVT_TEXTUREARRAY_H
