#include "TextureArray.h"

#include <gl_util.h>
#include <iostream>
#include <cstdlib>

#include <string.h>

#include <cmath>
#include <glbinding/gl/functions-patches.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace mvt{

  /* kinect color: GL_RGB32F, GL_RGB, GL_FLOAT*/
  /* kinect depth: GL_LUMINANCE32F_ARB, GL_RED, GL_FLOAT*/
  /* opengl color: GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE*/
  /* opengl depth: GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT*/

  /*static*/ unsigned TextureArray::s_accumbufferSize = 20;

  TextureArray::TextureArray(unsigned width, unsigned height, unsigned depth,
			     GLenum internalFormat/* = GL_RGBA*/, GLenum pixelFormat /* = GL_RGBA*/, GLenum pixelType /* = GL_FLOAT*/, unsigned storage)
    : m_width(width),
      m_height(height),
      m_depth(depth),
      m_type(GL_TEXTURE_2D_ARRAY_EXT),
      m_internalFormat(internalFormat),
      m_pixelFormat(pixelFormat),
      m_pixelType(pixelType),
      m_glHandle(0),
      m_accumbuffer(),
      m_accumbufferSize(),
      m_storage(storage)
  {
    m_accumbufferSize = s_accumbufferSize;
  }


  TextureArray::~TextureArray()
  {
    for(unsigned i = 0; i < m_accumbuffer.size(); ++i){
	delete [] m_accumbuffer[i];
      }
  }


  void
  TextureArray::init(){

    
    glGenTextures(1, &m_glHandle);
    check_gl_errors("after glGenTextures(1, &m_glHandle)", false);
    //glEnable(m_type);
    check_gl_errors("after glEnable(m_type)", false);

    glBindTexture(m_type,m_glHandle);
    //if(0){//(m_internalFormat == GL_LUMINANCE32F_ARB || m_internalFormat == GL_DEPTH_COMPONENT32){
    if(m_internalFormat == GL_LUMINANCE32F_ARB || m_internalFormat == GL_DEPTH_COMPONENT32){
      glTexParametere(m_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParametere(m_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    else{
      glTexParametere(m_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParametere(m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexParametere(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParametere(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParametere(m_type, GL_TEXTURE_WRAP_R, GL_CLAMP);
    if(m_storage){
      glCompressedTexImage3D(m_type, 0, /*GL_COMPRESSED_RGBA_S3TC_DXT1_EXT*/m_internalFormat, m_width, m_height, m_depth, 0, m_storage * m_depth /* ???? */, 0);
    }
    else{
      glTexImage3D(m_type, 0, m_internalFormat, m_width, m_height, m_depth, 0, m_pixelFormat, m_pixelType, 0);
    }
    
    //glDisable(m_type);

    // std::cerr << "TextureArray::init() " << this << std::endl;
    
    if(GL_FLOAT == m_pixelType){
      for(unsigned i = 0; i < (m_accumbufferSize + 1); ++i){
	m_accumbuffer.push_back(new float [m_width * m_height * m_depth]);
      }
    }


  }


  void
  TextureArray::fillLayer(unsigned layer, void* data){

    if(0 == m_glHandle)
      init();
    
    //glEnable(m_type);
    glBindTexture(m_type,m_glHandle);
    if(m_storage){
      glCompressedTexSubImage3D(m_type,0 /*level*/, 0, 0, layer, m_width, m_height, 1/*m_depth*/, /*GL_COMPRESSED_RGBA_S3TC_DXT1_EXT*/m_internalFormat, m_storage, data);
    }
    else{
      glTexSubImage3D(m_type,0 /*level*/, 0, 0, layer, m_width, m_height, 1/*m_depth*/, m_pixelFormat, m_pixelType, data);
    }
    
    glBindTexture(m_type,0);//glDisable(m_type);
    
  }

  void
  TextureArray::fillLayers(void* data){

    if(m_storage){
      std::cerr << "ERROR in TextureArray::fillLayers(void* data): compressed textures not supportet yet" << std::endl;
    }

    if(0 == m_glHandle)
      init();
    
    //glEnable(m_type);
    glBindTexture(m_type,m_glHandle);
    glTexSubImage3D(m_type,0 /*level*/, 0, 0, 0/*zoffset*/, m_width, m_height, m_depth, m_pixelFormat, m_pixelType, data);
    
    glBindTexture(m_type,0);//glDisable(m_type);
    
  }

  void
  TextureArray::fillLayersFromPBO(unsigned id){



    if(0 == m_glHandle)
      init();
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, id);
    //glEnable(m_type);
    glBindTexture(m_type,m_glHandle);

    if(m_storage){
      glCompressedTexSubImage3D(m_type,0 /*level*/, 0, 0, 0/*zoffset*/, m_width, m_height, m_depth, /*GL_COMPRESSED_RGBA_S3TC_DXT1_EXT*/m_internalFormat, m_storage * m_depth, BUFFER_OFFSET(0));
    }
    else{
      glTexSubImage3D(m_type,0 /*level*/, 0, 0, 0/*zoffset*/, m_width, m_height, m_depth, m_pixelFormat, m_pixelType, BUFFER_OFFSET(0));
    }
    glBindTexture(m_type,0);//glDisable(m_type);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);

  }


  void
  TextureArray::bind(){
    if(0 == m_glHandle)
      init();
    glBindTexture(m_type,m_glHandle);
  }

  void
  TextureArray::unbind(){
    if(0 == m_glHandle)
      init();
    glBindTexture(m_type,0);
  }


  GLuint
  TextureArray::getGLHandle(){

    if(0 == m_glHandle)
      init();
    return m_glHandle;
  }

  bool
  TextureArray::copyToCPU(unsigned layer, void* data){

    static unsigned accumlayer = 0;
    static bool valid = false;
    if(GL_FLOAT == m_pixelType){
      

      if(0 == layer){
	//glEnable(m_type);
	glBindTexture(m_type,m_glHandle);
	glGetTexImage( m_type, 0,
		       m_pixelFormat,
		       m_pixelType,
		       m_accumbuffer[accumlayer] );

	glBindTexture(m_type,0);//glDisable(m_type);

	// average
	const float weight = 1.0/m_accumbufferSize;
	for(unsigned i = 0; i < m_accumbufferSize; ++i){
	  for(unsigned j = 0; j < (m_width * m_height * m_depth); ++j){
	    float d_curr = m_accumbuffer[i][j];
	    if(std::isnan(d_curr)){
	      d_curr = 0.0;
	    }
	    if(0 == i){
	      m_accumbuffer[m_accumbufferSize][j] = weight * d_curr;//m_accumbuffer[i][j];
	    }
	    else{
	      m_accumbuffer[m_accumbufferSize][j] += weight * d_curr;//m_accumbuffer[i][j];
	    }

	  }
	}

	// check if average makes sense
	const double globalweight = 1.0/(m_width * m_height * m_depth);
	double globaldistance = 0.0;
	for(unsigned j = 0; j < (m_width * m_height * m_depth); ++j){
	  const double d_curr = std::abs(m_accumbuffer[accumlayer][j] - m_accumbuffer[m_accumbufferSize][j]);
	  if(!std::isnan(d_curr)){
	    globaldistance += globalweight * d_curr;//std::abs(m_accumbuffer[accumlayer][j] - m_accumbuffer[m_accumbufferSize][j]);
	  }
	}
	std::cerr << globaldistance << " valid: " << valid << " " << globalweight << " " << m_depth << std::endl;
#if 1
	valid = globaldistance < 0.1 ? true : false;
#endif
	++accumlayer;
	accumlayer = (accumlayer < m_accumbufferSize) ? accumlayer : 0;
      }

      if(valid || m_accumbufferSize == 1){
	memcpy(data,   (m_accumbuffer[m_accumbufferSize] + (m_width * m_height * layer)), (m_width * m_height) * sizeof(float));
      }

      
    }
    else{
      std::cerr << "ERROR: TextureArray::copyToCPU not supported for RGB" << std::endl;
    }

    return valid;
  }


  bool
  TextureArray::copyToCPUAll(void* data){
    
    glBindTexture(m_type,m_glHandle);
    glGetTexImage( m_type, 0,
		   m_pixelFormat,
		   m_pixelType,
		   data);

    glBindTexture(m_type,0);

    return true;
  }


  void
  TextureArray::resetAccumBuffer(){
    if(GL_FLOAT == m_pixelType){
      for(unsigned i = 0; i < m_accumbufferSize; ++i){
	for(unsigned j = 0; j < (m_width * m_height * m_depth); ++j){
	  m_accumbuffer[i][j] = 0.0;
	}
      }
    }
  }


  void
  TextureArray::setMAGMINFilter(int f){
    glBindTexture(m_type,m_glHandle);
    glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, f);
    glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, f);
  }


}// namespace mvt
