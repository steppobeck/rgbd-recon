#include "NetKinectArray.h"

#include "calibration_files.hpp"
#include "texture_blitter.hpp"
#include "screen_quad.hpp"
#include <FileBuffer.h>
#include <Timer.h>
#include <TextureArray.h>
#include <KinectCalibrationFile.h>
#include "CalibVolumes.hpp"
#include <timevalue.h>
#include <clock.h>
#include <DXTCompressor.h>

#include <gl_util.h>
#include <Viewport.h>
#include <gloostHelper.h>

#include <glbinding/gl/functions-patches.h>
#include <globjects/Shader.h>

#include "squish/squish.h"
#include <zmq.hpp>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

namespace kinect{

  NetKinectArray::NetKinectArray(std::string const& serverport, CalibrationFiles const* calibs, CalibVolumes const* vols, bool readfromfile)
    : m_width(0),
      m_widthc(0),
      m_height(0),
      m_heightc(0),
      m_numLayers(0),
      m_colorArray(0),
      m_depthArray(0),
      m_textures_quality{globjects::Texture::createDefault(GL_TEXTURE_2D_ARRAY)},
      m_textures_normal{globjects::Texture::createDefault(GL_TEXTURE_2D_ARRAY)},
      m_colorArray_back(0),
      m_depthArray_back(0),
      m_program_filter{new globjects::Program()},
      m_fboID(0),
      m_colorsize(0),
      m_depthsize(0),
      m_pbo_colors(),
      m_pbo_depths(),
      m_mutex(new boost::mutex),
      m_readThread(0),
      m_running(true),
      m_filter_textures(true),
      m_serverport(serverport),
      m_start_texture_unit(0),
      m_calib_files{calibs},
      m_calib_vols{vols},
      depth_compression_lex(false),
      depth_compression_ratio(100.0f)
  {
    init();

    if(readfromfile){
      readFromFiles();
    }
    else{
      m_readThread = new boost::thread(boost::bind(&NetKinectArray::readLoop, this));
    }

    m_program_filter->attach(
     globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/texture_passthrough.vs")
    ,globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/depth_process.fs")
    );
  }

  bool
  NetKinectArray::init(){
    m_numLayers = m_calib_files->num();
    m_width   = m_calib_files->getWidth();
    m_widthc  = m_calib_files->getWidthC();
    m_height  = m_calib_files->getHeight();
    m_heightc = m_calib_files->getHeightC();

    if(m_calib_files->isCompressedRGB() == 1){
      mvt::DXTCompressor dxt;
      dxt.init(m_calib_files->getWidthC(), m_calib_files->getHeightC(), FORMAT_DXT1);
      m_colorsize = dxt.getStorageSize();
    }
    else if(m_calib_files->isCompressedRGB() == 5){
      std::cerr << "NetKinectArray: using DXT5" << std::endl;
      m_colorsize = 307200;
    }
    else{
      m_colorsize = m_widthc * m_heightc * 3 * sizeof(byte);
    }

    m_pbo_colors = double_pbo{m_colorsize * m_numLayers};

    if(m_calib_files->isCompressedDepth()){
      m_pbo_depths.size = m_width * m_height * m_numLayers * sizeof(byte);
      m_depthsize =  m_width * m_height * sizeof(byte);
    }
    else{
      m_pbo_depths.size = m_width * m_height * m_numLayers * sizeof(float);
      m_depthsize =  m_width * m_height * sizeof(float);
    }

    m_pbo_depths = double_pbo{m_depthsize * m_numLayers};

    /* kinect color: GL_RGB32F, GL_RGB, GL_FLOAT*/
    /* kinect depth: GL_LUMINANCE32F_ARB, GL_RED, GL_FLOAT*/
    //m_colorArray = new mvt::TextureArray(m_width, m_height, m_numLayers, GL_RGB32F, GL_RGB, GL_FLOAT);
    if(m_calib_files->isCompressedRGB() == 1){
      m_colorArray = new mvt::TextureArray(m_widthc, m_heightc, m_numLayers, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, m_colorsize);
    }
    else if(m_calib_files->isCompressedRGB() == 5){
      m_colorArray = new mvt::TextureArray(m_widthc, m_heightc, m_numLayers, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_UNSIGNED_BYTE, m_colorsize);
    }
    else{
      m_colorArray = new mvt::TextureArray(m_widthc, m_heightc, m_numLayers, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
    }

    m_colorArray_back = new mvt::TextureArray(m_widthc, m_heightc, m_numLayers, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
    
    m_colorArray->getGLHandle();
    check_gl_errors("after m_colorArray->getGLHandle()", false);

    m_textures_quality->image3D(0, GL_LUMINANCE32F_ARB, m_width, m_height, m_numLayers, 0, GL_RED, GL_FLOAT, (void*)nullptr);
    m_textures_normal->image3D(0, GL_RGB32F, m_width, m_height, m_numLayers, 0, GL_RGB, GL_FLOAT, (void*)nullptr);

    m_depthArray = new mvt::TextureArray(m_width, m_height, m_numLayers, GL_LUMINANCE32F_ARB, GL_RED, GL_FLOAT);

    if(m_calib_files->isCompressedDepth()){
      m_depthArray_back = new mvt::TextureArray(m_width, m_height, m_numLayers, GL_LUMINANCE, GL_RED, GL_UNSIGNED_BYTE);
    }
    else{
      m_depthArray_back = new mvt::TextureArray(m_width, m_height, m_numLayers, GL_LUMINANCE32F_ARB, GL_RED, GL_FLOAT);
    }
    
    m_depthArray->getGLHandle();
    check_gl_errors("after m_depthArray->getGLHandle()", false);

    m_colorArray_back->getGLHandle();
    m_depthArray_back->getGLHandle();

    glGenFramebuffersEXT(1, &m_fboID);
    m_program_filter->setUniform("kinect_depths",40);
    m_program_filter->setUniform("texSizeInv", glm::fvec2(1.0f/m_width, 1.0f/m_height));

    std::cout << "NetKinectArray::NetKinectArray: " << this << std::endl;

    return true;
  }

  NetKinectArray::~NetKinectArray(){
    delete m_colorArray;
    delete m_depthArray;
    delete m_colorArray_back;
    delete m_depthArray_back;
    glDeleteFramebuffersEXT(1, &m_fboID);

    m_running = false;
    m_readThread->join();
    delete m_readThread;
    delete m_mutex;

    m_textures_quality->destroy();
    m_textures_normal->destroy();
    m_program_filter->destroy();
  }

  void
  NetKinectArray::update() {
    boost::mutex::scoped_lock lock(*m_mutex);
    // skip if no new frame was received
    if(!m_pbo_colors.needSwap || !m_pbo_depths.needSwap) return;

  	m_pbo_colors.swapBuffers();
  	m_pbo_depths.swapBuffers();

    m_colorArray->fillLayersFromPBO(m_pbo_colors.front->id());
    m_depthArray->fillLayersFromPBO(m_pbo_depths.front->id());

    bindToTextureUnits();

    processTextures();
  }

glm::uvec2 NetKinectArray::getDepthResolution() const {
  return glm::uvec2{m_width, m_height};
}
glm::uvec2 NetKinectArray::getColorResolution() const {
  return glm::uvec2{m_widthc, m_heightc};
}

void NetKinectArray::bindToFramebuffer(GLuint array_handle, GLuint layer) {
  //
  //std::cerr << current_fbo << std::endl;
  // render to depth
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboID);
  GLenum buffers[] = {GL_COLOR_ATTACHMENT0_EXT};
  glDrawBuffers(1, buffers);
  glFramebufferTextureLayerEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, array_handle, 0, layer);
  //glFramebufferTextureLayerEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,  m_ogldepthArray->getGLHandle(), 0, i);

  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  switch(status){
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        printf("Error:Frame buffer not supported in NetKinectArray::update().\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
        printf("Error:Frame buffer attachments not supported in NetKinectArray::update().\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        printf("Error:Missing color attachment in NetKinectArray::update().\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        printf("Error:All textures attached to frame buffer must have same dimension in NetKinectArray::update().\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        printf("Error:All textures attached to frame buffer must have same format in NetKinectArray::update().\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        printf("Error:Missing draw buffer in NetKinectArray::update().\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        printf("Error:Missing read buffer in NetKinectArray::update().\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT:
        printf("Error:Missing layer targets in NetKinectArray::update().\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_EXT:
        printf("Error:Incomplete layer count in NetKinectArray::update().\n");
        break;
    case GL_FRAMEBUFFER_COMPLETE_EXT:
    default:
      break;
  }
}
  void
  NetKinectArray::processTextures(){

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    GLint current_fbo;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &current_fbo);
    GLsizei old_vp_params[4];
    glGetIntegerv(GL_VIEWPORT, old_vp_params);
    glViewport(0, 0, m_width, m_height);

  	glActiveTexture(GL_TEXTURE0 + 40);
  	m_depthArray->bind();
    m_program_filter->use();

    for(unsigned i = 0; i < m_calib_files->num(); ++i){
      m_program_filter->setUniform("cv_min_ds",m_calib_vols->getDepthLimits(i).x);
      m_program_filter->setUniform("cv_max_ds",m_calib_vols->getDepthLimits(i).y);
      //
      //std::cerr << current_fbo << std::endl;
      // render to depth
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboID);
      std::vector<GLenum> attachments{GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT};
      glDrawBuffers(attachments.size(), attachments.data());
      glFramebufferTextureLayerEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, m_depthArray_back->getGLHandle(), 0, i);
      glFramebufferTextureLayerEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, m_textures_quality->id(), 0, i);
      glFramebufferTextureLayerEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, m_textures_normal->id(), 0, i);

      m_program_filter->setUniform("filter_textures", m_filter_textures);
      GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
      switch(status){
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            printf("Error:Frame buffer not supported in NetKinectArray::update().\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            printf("Error:Frame buffer attachments not supported in NetKinectArray::update().\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            printf("Error:Missing color attachment in NetKinectArray::update().\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            printf("Error:All textures attached to frame buffer must have same dimension in NetKinectArray::update().\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            printf("Error:All textures attached to frame buffer must have same format in NetKinectArray::update().\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            printf("Error:Missing draw buffer in NetKinectArray::update().\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            printf("Error:Missing read buffer in NetKinectArray::update().\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT:
            printf("Error:Missing layer targets in NetKinectArray::update().\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_EXT:
            printf("Error:Incomplete layer count in NetKinectArray::update().\n");
            break;
        case GL_FRAMEBUFFER_COMPLETE_EXT:
        default:
          break;
      }

      m_program_filter->setUniform("layer",i);
      m_program_filter->setUniform("compress",m_calib_files->getCalibs()[i].isCompressedDepth());
      const float near = m_calib_files->getCalibs()[i].getNear();
      const float far  = m_calib_files->getCalibs()[i].getFar();
      const float scale = (far - near);
      m_program_filter->setUniform("scale",scale);
      m_program_filter->setUniform("near",near);
      m_program_filter->setUniform("scaled_near",scale/255.0f);
 
      m_program_filter->setUniform("cv_xyz", m_calib_vols->getXYZVolumeUnits());
      m_program_filter->setUniform("cv_uv", m_calib_vols->getUVVolumeUnits());

      ScreenQuad::draw();
    }
    
    m_program_filter->release();

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, current_fbo);
    glViewport ((GLsizei)old_vp_params[0],
                (GLsizei)old_vp_params[1],
                (GLsizei)old_vp_params[2],
                (GLsizei)old_vp_params[3]);
 
    glPopAttrib();

    bindBackToTextureUnits();
  }

void NetKinectArray::setStartTextureUnit(unsigned start_texture_unit) {
  m_start_texture_unit = start_texture_unit;
}

void NetKinectArray::bindToTextureUnits() const {
  glActiveTexture(GL_TEXTURE0 + m_start_texture_unit);
  m_colorArray->bind();
  glActiveTexture(GL_TEXTURE0 + m_start_texture_unit + 1);
  m_depthArray->bind();
  glActiveTexture(GL_TEXTURE0 + m_start_texture_unit + 2);
  m_textures_quality->bind();
  glActiveTexture(GL_TEXTURE0 + m_start_texture_unit + 3);
  m_textures_normal->bind();
}


void NetKinectArray::bindBackToTextureUnits() const {
  glActiveTexture(GL_TEXTURE0 + m_start_texture_unit);
  m_colorArray->bind();
  glActiveTexture(GL_TEXTURE0 + m_start_texture_unit + 1);
  m_depthArray_back->bind();
}

unsigned NetKinectArray::getStartTextureUnit() const {
  return m_start_texture_unit;
}

void NetKinectArray::filterTextures(bool filter) {
  m_filter_textures = filter;
  // processwith new settings
  processTextures();
}

mvt::TextureArray*
NetKinectArray::getDepthArrayBack(){
  return m_depthArray_back;
}

mvt::TextureArray*
NetKinectArray::getDepthArray(){
  return m_depthArray;
}

  void
  NetKinectArray::readLoop(){
    // open multicast listening connection to server and port

    zmq::context_t ctx(1); // means single threaded
    zmq::socket_t  socket(ctx, ZMQ_SUB); // means a subscriber

    socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    uint64_t hwm = 1;
    socket.setsockopt(ZMQ_HWM,&hwm, sizeof(hwm));

    std::string endpoint("tcp://" + m_serverport);
    socket.connect(endpoint.c_str());

    //const unsigned pixelcountc = m_calib_files->getWidthC() * m_calib_files->getHeightC();    
    const unsigned colorsize = m_colorsize;
    const unsigned depthsize = m_depthsize;//pixelcount * sizeof(float);

    bool drop = false;
    sensor::timevalue ts(sensor::clock::time());

    while(m_running){
      zmq::message_t zmqm((colorsize + depthsize) * m_calib_files->num());
      
      socket.recv(&zmqm); // blocking
      
      if(!drop){
      	while(m_pbo_colors.needSwap || m_pbo_depths.needSwap){
      	  ;
      	}

      	unsigned offset = 0;
      	// receive data
        const unsigned number_of_kinects = m_calib_files->num(); // is 5 in the current example
        // this loop goes over each kinect like K1_frame_1 K2_frame_1 K3_frame_1 
      	for(unsigned i = 0; i < number_of_kinects; ++i){
      	  memcpy((byte*) m_pbo_colors.pointer() + i*colorsize , (byte*) zmqm.data() + offset, colorsize);
      	  offset += colorsize;
      	  memcpy((byte*) m_pbo_depths.pointer() + i*depthsize , (byte*) zmqm.data() + offset, depthsize);

      	  offset += depthsize;
      	}
      }

      if(!drop){ // swap
      	boost::mutex::scoped_lock lock(*m_mutex);
        m_pbo_colors.needSwap = true;
      	m_pbo_depths.needSwap = true;
      }
    }
  }

  void
  NetKinectArray::writeCurrentTexture(std::string prefix){
    //depths
    if (m_calib_files->isCompressedDepth())
    {
      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      
      glBindTexture(GL_TEXTURE_2D_ARRAY,m_depthArray->getGLHandle());
      int width, height, depth;
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_WIDTH, &width);
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_HEIGHT, &height);
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_DEPTH, &depth);
      
      std::vector<std::uint8_t> depths;
      depths.resize(width*height*depth);
      
      glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RED, GL_UNSIGNED_BYTE, (void*)&depths[0]); 
      
      int offset = 0;
      
      for (int k = 0; k < depth; ++k)
      {
        std::stringstream sstr;
        sstr << "output/" << prefix << "_d_" << k << ".bmp";
        std::string filename (sstr.str());
        std::cout << "writing depth texture for kinect " << k << " to file " << filename << std::endl;
 
        offset += width*height;
        
        writeBMP(filename, depths, offset, 1);
        offset += width*height;
      }
    }
    else
    {
      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      
      glBindTexture(GL_TEXTURE_2D_ARRAY,m_depthArray->getGLHandle());
      int width, height, depth;
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_WIDTH, &width);
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_HEIGHT, &height);
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_DEPTH, &depth);
      
      std::vector<float> depthsTmp;
      depthsTmp.resize(width*height*depth);
      
      glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RED, GL_FLOAT, (void*)&depthsTmp[0]); 
      
      std::vector<std::uint8_t> depths;
      depths.resize(depthsTmp.size());
      
      for (int i = 0; i < width*height*depth; ++i)
      {
        depths[i] = (std::uint8_t)depthsTmp[i] * 255.0f;
      }
      
      int offset = 0;
      
      for (int k = 0; k < depth; ++k)
      {
        std::stringstream sstr;
        sstr << "output/" << prefix << "_d_" << k << ".bmp";
        std::string filename (sstr.str());
        std::cout << "writing depth texture for kinect " << k << " to file " << filename << " (values are compressed to 8bit)" << std::endl;

        writeBMP(filename, depths, offset, 1);
        offset += width*height;
      }
    }
    
    //color
    if (m_calib_files->isCompressedRGB() == 1)
    {
      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      
      glBindTexture(GL_TEXTURE_2D_ARRAY,m_colorArray->getGLHandle());
      int size;
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &size);
      
      std::vector<std::uint8_t> data;
      data.resize(size);
      
      glGetCompressedTexImage(GL_TEXTURE_2D_ARRAY, 0, (void*)&data[0]);
      
      std::vector<std::uint8_t> colors;
      colors.resize(4*m_widthc*m_heightc);
    
      for (unsigned k = 0; k < m_numLayers; ++k)
      {
        squish::DecompressImage (&colors[0], m_widthc, m_heightc, &data[k*m_colorsize], squish::kDxt1);
        
        std::stringstream sstr;
        sstr << "output/" << prefix << "_col_" << k << ".bmp";
        std::string filename (sstr.str());
        std::cout << "writing color texture for kinect " << k << " to file " << filename << std::endl;

        writeBMP(filename, colors, 0, 4);
      }
    }
    else
    {
      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      
      glBindTexture(GL_TEXTURE_2D_ARRAY,m_colorArray->getGLHandle());
      int width, height, depth;
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_WIDTH, &width);
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_HEIGHT, &height);
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_DEPTH, &depth);
      
      std::vector<std::uint8_t> colors;
      colors.resize(3*width*height*depth);
      
      glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)&colors[0]); 
      
      int offset = 0;
      
      for (int k = 0; k < depth; ++k)
      {
        std::stringstream sstr;
        sstr << "output/" << prefix << "_col_" << k << ".bmp";
        std::string filename (sstr.str());
        std::cout << "writing color texture for kinect " << k << " to file " << filename << std::endl;

        writeBMP(filename, colors, offset, 3);
        offset += 3 * width*height;
      }
    }

  }
  
  // no universal use! very unflexible, resolution depth = resolution color, no row padding
  void NetKinectArray::writeBMP(std::string filename, std::vector<std::uint8_t> const& data, unsigned int offset, unsigned int bytesPerPixel)
  {
    std::ofstream file (filename, std::ofstream::binary);
    char c;
    short s;
    int i;
    
    c = 'B'; file.write(&c, 1);
    c = 'M'; file.write(&c, 1);
    i = m_widthc * m_heightc * 3 + 54; file.write((char const*) &i, 4);
    i = 0; file.write((char const*) &i,4);
    i = 54; file.write((char const*) &i, 4);
    i = 40; file.write((char const*) &i, 4);
    i = m_widthc; file.write((char const*) &i, 4);
    i = m_heightc; file.write((char const*) &i, 4);
    s = 1; file.write((char const*) &s, 2);
    s = 24; file.write((char const*) &s, 2);
    i = 0; file.write((char const*) &i, 4);
    i = m_widthc * m_heightc * 3; file.write((char const*) &i, 4);
    i = 0; file.write((char const*) &i, 4);
    i = 0; file.write((char const*) &i, 4);
    i = 0; file.write((char const*) &i, 4);
    i = 0; file.write((char const*) &i, 4);
    
    
    for (unsigned int h = m_heightc; h > 0; --h)
    {
    	for (unsigned int w = 0; w < m_widthc * bytesPerPixel; w += bytesPerPixel)
    	{
    	  if (bytesPerPixel == 1)
          {
            file.write((char const*) &data[offset + w + (h-1) * m_widthc * bytesPerPixel], 1);
            file.write((char const*) &data[offset + w + (h-1) * m_widthc * bytesPerPixel], 1);
            file.write((char const*) &data[offset + w + (h-1) * m_widthc * bytesPerPixel], 1);
          }
          else if (bytesPerPixel == 3 || bytesPerPixel == 4)
          {
            file.write((char const*) &data[offset + w+2 + (h-1) * m_widthc * bytesPerPixel], 1);
            file.write((char const*) &data[offset + w+1 + (h-1) * m_widthc * bytesPerPixel], 1);
            file.write((char const*) &data[offset + w+0 + (h-1) * m_widthc * bytesPerPixel], 1);  
          }
    	}
    }

    file.close();
  }

  void
  NetKinectArray::readFromFiles(){
    std::vector<sys::FileBuffer*> fbs;

    for(unsigned i = 0 ; i < m_calib_files->num(); ++i){
      std::string yml(m_calib_files->getCalibs()[i]._filePath);
      std::string base((const char*) basename((char *) yml.c_str()));
      base.replace( base.end() - 4, base.end(), "");
      std::string filename = std::string("recordings/" + base + ".stream");

      fbs.push_back(new sys::FileBuffer(filename.c_str()));
      if(!fbs.back()->open("r")){
      	std::cerr << "error opening " << filename << " exiting..." << std::endl;
      	exit(1);
      }
      fbs.back()->setLooping(/*true*/false);
    }

    const unsigned colorsize = m_colorsize;
    const unsigned depthsize = m_depthsize;

    while(m_pbo_colors.needSwap || m_pbo_depths.needSwap){
      ;
    }

    unsigned offset = 0;
    // receive data
    for(unsigned i = 0; i < m_calib_files->num(); ++i){
      //memcpy((byte*) m_pbo_colors.pointer() + i*colorsize , (byte*) zmqm.data() + offset, colorsize);
      fbs[i]->read((byte*) m_pbo_colors.pointer() + i*colorsize, colorsize);
      offset += colorsize;

      //memcpy((byte*) m_pbo_depths.pointer() + i*depthsize , (byte*) zmqm.data() + offset, depthsize);
      fbs[i]->read((byte*) m_pbo_depths.pointer() + i*depthsize, depthsize);
      
      offset += depthsize;
    }

    { // swap
      boost::mutex::scoped_lock lock(*m_mutex);
      m_pbo_colors.needSwap = true;
      m_pbo_depths.needSwap = true;
    }
 
  }

}