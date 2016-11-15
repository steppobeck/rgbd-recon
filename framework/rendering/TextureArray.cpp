#include "TextureArray.h"

#include <glbinding/gl/functions.h>
#include <glbinding/gl/functions-patches.h>
#include <globjects/Texture.h>

#include <iostream>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace kinect {

  /* kinect color: GL_RGB32F, GL_RGB, GL_FLOAT*/
  /* kinect depth: GL_LUMINANCE32F_ARB, GL_RED, GL_FLOAT*/
  /* opengl color: GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE*/
  /* opengl depth: GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT*/

TextureArray::TextureArray(unsigned width, unsigned height, unsigned depth,
		     GLenum internalFormat/* = GL_RGBA*/, GLenum pixelFormat /* = GL_RGBA*/, GLenum pixelType /* = GL_FLOAT*/, unsigned storage)
 :m_width(width)
 ,m_height(height)
 ,m_depth(depth)
 ,m_type(GL_TEXTURE_2D_ARRAY_EXT)
 ,m_internalFormat(internalFormat)
 ,m_pixelFormat(pixelFormat)
 ,m_pixelType(pixelType)
 ,m_texture{globjects::Texture::createDefault(GL_TEXTURE_2D_ARRAY)}
 ,m_storage(storage)
{
 if(m_storage > 0) {
    m_texture->compressedImage3D(0, m_internalFormat, m_width, m_height, m_depth, 0, m_storage * m_depth, (void*)nullptr);
  }
  else {
    m_texture->image3D(0, m_internalFormat, m_width, m_height, m_depth, 0, m_pixelFormat, m_pixelType, (void*)nullptr);
  }
}
// void
// TextureArray::fillLayer(unsigned layer, void* data){

//   if(0 == m_glHandle)
//     init();
  
//   //glEnable(m_type);
//   glBindTexture(m_type,m_glHandle);
//   if(m_storage){
//     glCompressedTexSubImage3D(m_type,0 /*level*/, 0, 0, layer, m_width, m_height, 1/*m_depth*/, /*GL_COMPRESSED_RGBA_S3TC_DXT1_EXT*/m_internalFormat, m_storage, data);
//   }
//   else{
//     glTexSubImage3D(m_type,0 /*level*/, 0, 0, layer, m_width, m_height, 1/*m_depth*/, m_pixelFormat, m_pixelType, data);
//   }
  
//   glBindTexture(m_type,0);//glDisable(m_type);
  
// }

// void
// TextureArray::fillLayers(void* data){

//   if(m_storage){
//     std::cerr << "ERROR in TextureArray::fillLayers(void* data): compressed textures not supportet yet" << std::endl;
//   }

//   if(0 == m_glHandle)
//     init();
  
//   //glEnable(m_type);
//   glBindTexture(m_type,m_glHandle);
//   glTexSubImage3D(m_type,0 /*level*/, 0, 0, 0/*zoffset*/, m_width, m_height, m_depth, m_pixelFormat, m_pixelType, data);
//   m_texture->subImage3D(0, 0,0,0, m_width, m_height, m_depth, m_pixelFormat, m_pixelType, data);
  
//   glBindTexture(m_type,0);//glDisable(m_type);
  
// }

void TextureArray::fillLayersFromPBO(unsigned id) {
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, id);

  if(m_storage) {
    m_texture->bind();
    glCompressedTexSubImage3D(m_type,0 /*level*/, 0, 0, 0/*zoffset*/, m_width, m_height, m_depth, /*GL_COMPRESSED_RGBA_S3TC_DXT1_EXT*/m_internalFormat, m_storage * m_depth, BUFFER_OFFSET(0));
    m_texture->unbind();
  }
  else {
    m_texture->subImage3D(0, 0,0,0, m_width, m_height, m_depth, m_pixelFormat, m_pixelType, 0);
  }
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
}

globjects::Texture* TextureArray::getTexture() const {
  return m_texture;
}

void TextureArray::bind() {
  m_texture->bind();
}

void TextureArray::unbind() {
  m_texture->unbind();
}

unsigned TextureArray::getGLHandle() {
  return m_texture->id();
}

// bool
// TextureArray::copyToCPU(unsigned layer, void* data){

//   static unsigned accumlayer = 0;
//   static bool valid = false;
//   if(GL_FLOAT == m_pixelType){
      
//     if(0 == layer) {
//       //glEnable(m_type);
//       glBindTexture(m_type,m_glHandle);
//       glGetTexImage( m_type, 0,
//       	       m_pixelFormat,
//       	       m_pixelType,
//       	       m_accumbuffer[accumlayer] );

//       glBindTexture(m_type,0);//glDisable(m_type);

//       // average
//       const float weight = 1.0/m_accumbufferSize;
//       for(unsigned i = 0; i < m_accumbufferSize; ++i){
//         for(unsigned j = 0; j < (m_width * m_height * m_depth); ++j){
//           float d_curr = m_accumbuffer[i][j];
//           if(std::isnan(d_curr)){
//             d_curr = 0.0;
//           }
//           if(0 == i){
//             m_accumbuffer[m_accumbufferSize][j] = weight * d_curr;//m_accumbuffer[i][j];
//           }
//           else{
//             m_accumbuffer[m_accumbufferSize][j] += weight * d_curr;//m_accumbuffer[i][j];
//           }

//         }
//       }

//       // check if average makes sense
//       const double globalweight = 1.0/(m_width * m_height * m_depth);
//       double globaldistance = 0.0;
//       for(unsigned j = 0; j < (m_width * m_height * m_depth); ++j){
//         const double d_curr = std::abs(m_accumbuffer[accumlayer][j] - m_accumbuffer[m_accumbufferSize][j]);
//         if(!std::isnan(d_curr)){
//           globaldistance += globalweight * d_curr;//std::abs(m_accumbuffer[accumlayer][j] - m_accumbuffer[m_accumbufferSize][j]);
//         }
//       }
//       std::cerr << globaldistance << " valid: " << valid << " " << globalweight << " " << m_depth << std::endl;
// #if 1
//       valid = globaldistance < 0.1 ? true : false;
// #endif
//       ++accumlayer;
//       accumlayer = (accumlayer < m_accumbufferSize) ? accumlayer : 0;
//     }

//     if(valid || m_accumbufferSize == 1) {
//        memcpy(data,   (m_accumbuffer[m_accumbufferSize] + (m_width * m_height * layer)), (m_width * m_height) * sizeof(float));
//     }
//   }
//   else {
//     std::cerr << "ERROR: TextureArray::copyToCPU not supported for RGB" << std::endl;
//   }
//   return valid;
// }

// bool
// TextureArray::copyToCPUAll(void* data){
  
//   glBindTexture(m_type,m_glHandle);
//   glGetTexImage( m_type, 0,
// 	   m_pixelFormat,
// 	   m_pixelType,
// 	   data);

//   glBindTexture(m_type,0);

//   return true;
// }

void TextureArray::setMAGMINFilter(GLenum f) {
  m_texture->setParameter(GL_TEXTURE_MIN_FILTER, f);
  m_texture->setParameter(GL_TEXTURE_MAG_FILTER, f);
}

}// namespace mvt
