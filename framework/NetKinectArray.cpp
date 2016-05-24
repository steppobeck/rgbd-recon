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
#include <globjects/logging.h>

#include "squish/squish.h"
#include <zmq.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <thread>

namespace kinect{
  static const std::size_t s_num_bg_frames = 20;
  NetKinectArray::NetKinectArray(std::string const& serverport, CalibrationFiles const* calibs, CalibVolumes const* vols, bool readfromfile)
    : m_width(0),
      m_widthc(0),
      m_height(0),
      m_heightc(0),
      m_numLayers(0),
      m_colorArray(),
      m_depthArray_raw(),
      m_textures_depth{globjects::Texture::createDefault(GL_TEXTURE_2D_ARRAY)},
      m_textures_quality{globjects::Texture::createDefault(GL_TEXTURE_2D_ARRAY)},
      m_textures_normal{globjects::Texture::createDefault(GL_TEXTURE_2D_ARRAY)},
      m_textures_bg{globjects::Texture::createDefault(GL_TEXTURE_2D_ARRAY)},
      m_textures_bg_back{globjects::Texture::createDefault(GL_TEXTURE_2D_ARRAY)},
      m_textures_silhouette{globjects::Texture::createDefault(GL_TEXTURE_2D_ARRAY)},
      m_fbo{new globjects::Framebuffer()},
      m_colorArray_back(),
      m_program_filter{new globjects::Program()},
      m_program_normal{new globjects::Program()},
      m_program_quality{new globjects::Program()},
      m_program_bg{new globjects::Program()},
      m_colorsize(0),
      m_depthsize(0),
      m_pbo_colors(),
      m_pbo_depths(),
      m_mutex(),
      m_readThread(),
      m_running(true),
      m_filter_textures(true),
      m_serverport(serverport),
      m_num_frame{0},
      m_curr_frametime{0.0},
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
      m_readThread = std::unique_ptr<std::thread>{new std::thread(std::bind(&NetKinectArray::readLoop, this))};
    }

    m_program_filter->attach(
     globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/texture_passthrough.vs")
    ,globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/pre_depth.fs")
    );
    m_program_normal->attach(
     globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/texture_passthrough.vs")
    ,globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/pre_normal.fs")
    );
    m_program_quality->attach(
     globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/texture_passthrough.vs")
    ,globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/pre_quality.fs")
    );
    m_program_bg->attach(
     globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/texture_passthrough.vs")
    ,globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/pre_background.fs")
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
      std::cout << "Color DXT 1 compressed" << std::endl;
      m_colorArray = std::unique_ptr<mvt::TextureArray>{new mvt::TextureArray(m_widthc, m_heightc, m_numLayers, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, m_colorsize)};
    }
    else if(m_calib_files->isCompressedRGB() == 5){
      std::cout << "Color DXT 5 compressed" << std::endl;
      m_colorArray = std::unique_ptr<mvt::TextureArray>{new mvt::TextureArray(m_widthc, m_heightc, m_numLayers, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_UNSIGNED_BYTE, m_colorsize)};
    }
    else{
      m_colorArray = std::unique_ptr<mvt::TextureArray>{new mvt::TextureArray(m_widthc, m_heightc, m_numLayers, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE)};
    }
    m_colorArray_back = std::unique_ptr<mvt::TextureArray>{new mvt::TextureArray(m_widthc, m_heightc, m_numLayers, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE)};

    m_textures_quality->image3D(0, GL_LUMINANCE32F_ARB, m_width, m_height, m_numLayers, 0, GL_RED, GL_FLOAT, (void*)nullptr);
    m_textures_normal->image3D(0, GL_RGB32F, m_width, m_height, m_numLayers, 0, GL_RGB, GL_FLOAT, (void*)nullptr);
    std::vector<glm::fvec2> empty_bg_tex(m_width * m_height * m_numLayers, glm::fvec2{0.0f});
    m_textures_bg->image3D(0, GL_RG32F, m_width, m_height, m_numLayers, 0, GL_RG, GL_FLOAT, empty_bg_tex.data());
    m_textures_bg_back->image3D(0, GL_RG32F, m_width, m_height, m_numLayers, 0, GL_RG, GL_FLOAT, (void*)nullptr);
    m_textures_silhouette->image3D(0, GL_R32F, m_width, m_height, m_numLayers, 0, GL_RED, GL_FLOAT, (void*)nullptr);

    if(m_calib_files->isCompressedDepth()){
      m_depthArray_raw = std::unique_ptr<mvt::TextureArray>{new mvt::TextureArray(m_width, m_height, m_numLayers, GL_LUMINANCE, GL_RED, GL_UNSIGNED_BYTE)};
    }
    else{
      m_depthArray_raw = std::unique_ptr<mvt::TextureArray>{new mvt::TextureArray(m_width, m_height, m_numLayers, GL_LUMINANCE32F_ARB, GL_RED, GL_FLOAT)};
    }
    m_textures_depth->image3D(0, GL_LUMINANCE32F_ARB, m_width, m_height, m_numLayers, 0, GL_RED, GL_FLOAT, (void*)nullptr);
    // m_depthArray_back = std::unique_ptr<mvt::TextureArray>{new mvt::TextureArray(m_width, m_height, m_numLayers, GL_LUMINANCE32F_ARB, GL_RED, GL_FLOAT)};
    // m_textures_depth_raw->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // m_textures_depth_raw->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    m_depthArray_raw->setMAGMINFilter(GL_NEAREST);
    m_textures_depth->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    m_textures_depth->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    m_program_filter->setUniform("kinect_depths", 40);
    glm::fvec2 tex_size_inv{1.0f/m_width, 1.0f/m_height};
    m_program_filter->setUniform("texSizeInv", tex_size_inv);
    m_program_normal->setUniform("texSizeInv", tex_size_inv);
    m_program_quality->setUniform("texSizeInv", tex_size_inv);
    m_program_bg->setUniform("texSizeInv", tex_size_inv);
    m_program_bg->setUniform("bg_depths", 41);

    std::cout << "NetKinectArray::NetKinectArray: " << this << std::endl;

    return true;
  }

  NetKinectArray::~NetKinectArray(){
    m_running = false;
    m_readThread->join();
  }

  void
  NetKinectArray::update() {
    std::unique_lock<std::mutex> lock(m_mutex);
    // skip if no new frame was received
    if(!m_pbo_colors.needSwap || !m_pbo_depths.needSwap) return;

  	m_pbo_colors.swapBuffers();
  	m_pbo_depths.swapBuffers();

    m_colorArray->fillLayersFromPBO(m_pbo_colors.front->id());
    m_depthArray_raw->fillLayersFromPBO(m_pbo_depths.front->id());

    processTextures();
  }

glm::uvec2 NetKinectArray::getDepthResolution() const {
  return glm::uvec2{m_width, m_height};
}
glm::uvec2 NetKinectArray::getColorResolution() const {
  return glm::uvec2{m_widthc, m_heightc};
}

void NetKinectArray::processBackground() {
  m_textures_bg->bindActive(41);
  m_program_bg->use();

  for(unsigned i = 0; i < m_calib_files->num(); ++i){
    m_fbo->attachTextureLayer(GL_COLOR_ATTACHMENT0, m_textures_bg_back, 0, i);

    m_program_bg->setUniform("layer", i);

    ScreenQuad::draw();
  }
  m_program_bg->release();
  std::swap(m_textures_bg, m_textures_bg_back);
  m_textures_bg->bindActive(m_start_texture_unit + 5);

  ++m_num_frame;
}

void NetKinectArray::processTextures(){

  glPushAttrib(GL_ALL_ATTRIB_BITS);

  GLint current_fbo;
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &current_fbo);
  GLsizei old_vp_params[4];
  glGetIntegerv(GL_VIEWPORT, old_vp_params);
  glViewport(0, 0, m_width, m_height);

	glActiveTexture(GL_TEXTURE0 + 40);
	m_depthArray_raw->bind();
  m_program_filter->use();

  m_fbo->bind();
  m_fbo->setDrawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});

  m_program_filter->setUniform("filter_textures", m_filter_textures);
// depth and old quality
  for(unsigned i = 0; i < m_calib_files->num(); ++i){
    m_program_filter->setUniform("cv_min_ds", m_calib_vols->getDepthLimits(i).x);
    m_program_filter->setUniform("cv_max_ds", m_calib_vols->getDepthLimits(i).y);

    m_fbo->attachTextureLayer(GL_COLOR_ATTACHMENT0, m_textures_depth, 0, i);
    m_fbo->attachTextureLayer(GL_COLOR_ATTACHMENT1, m_textures_quality, 0, i);
    m_fbo->attachTextureLayer(GL_COLOR_ATTACHMENT2, m_textures_silhouette, 0, i);
    m_program_filter->setUniform("layer", i);
    m_program_filter->setUniform("compress", m_calib_files->getCalibs()[i].isCompressedDepth());
    const float near = m_calib_files->getCalibs()[i].getNear();
    const float far  = m_calib_files->getCalibs()[i].getFar();
    const float scale = (far - near);
    m_program_filter->setUniform("scale", scale);
    m_program_filter->setUniform("near", near);
    m_program_filter->setUniform("scaled_near", scale/255.0f);

    ScreenQuad::draw();
  }
  
  m_program_filter->release();
// normals
  m_program_normal->use();
  m_program_normal->setUniform("cv_xyz", m_calib_vols->getXYZVolumeUnits());
  m_program_normal->setUniform("cv_uv", m_calib_vols->getUVVolumeUnits());

  m_fbo->setDrawBuffers({GL_COLOR_ATTACHMENT0});

  for(unsigned i = 0; i < m_calib_files->num(); ++i){
    m_fbo->attachTextureLayer(GL_COLOR_ATTACHMENT0, m_textures_normal, 0, i);

    m_program_normal->setUniform("layer", i);

    ScreenQuad::draw();
  }
  
  m_program_normal->release();
// quality
  m_program_quality->use();
  m_program_quality->setUniform("camera_positions", m_calib_vols->getCameraPositions());

  for(unsigned i = 0; i < m_calib_files->num(); ++i){
    m_fbo->attachTextureLayer(GL_COLOR_ATTACHMENT0, m_textures_quality, 0, i);

    m_program_quality->setUniform("layer", i);

    ScreenQuad::draw();
  }
  
  m_program_quality->release();
  if(m_num_frame < s_num_bg_frames && m_curr_frametime < 0.5) {
    processBackground();
  }

  m_fbo->unbind();
  
  glViewport ((GLsizei)old_vp_params[0],
              (GLsizei)old_vp_params[1],
              (GLsizei)old_vp_params[2],
              (GLsizei)old_vp_params[3]);

  glPopAttrib();
}

void NetKinectArray::setStartTextureUnit(unsigned start_texture_unit) {
  m_start_texture_unit = start_texture_unit;

  bindToTextureUnits();

  m_program_normal->setUniform("kinect_depths", GLint(m_start_texture_unit + 1));
  m_program_quality->setUniform("kinect_depths", GLint(m_start_texture_unit + 1));
  m_program_bg->setUniform("kinect_depths", GLint(m_start_texture_unit + 1));
  m_program_bg->setUniform("kinect_depths", 40);
  m_program_filter->setUniform("bg_depths", GLint(m_start_texture_unit + 5));
}

void NetKinectArray::bindToTextureUnits() const {
  glActiveTexture(GL_TEXTURE0 + m_start_texture_unit);
  m_colorArray->bind();
  m_textures_depth->bindActive(m_start_texture_unit + 1);
  m_textures_quality->bindActive(m_start_texture_unit + 2);
  m_textures_normal->bindActive(m_start_texture_unit + 3);
  m_textures_silhouette->bindActive(m_start_texture_unit + 4);
  m_textures_bg->bindActive(m_start_texture_unit + 5);
  glActiveTexture(GL_TEXTURE0 + 40);
  m_depthArray_raw->bind();
}

unsigned NetKinectArray::getStartTextureUnit() const {
  return m_start_texture_unit;
}

void NetKinectArray::filterTextures(bool filter) {
  m_filter_textures = filter;
  // process with new settings
  processTextures();
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

    double current_time = 0.0;

    while(m_running){
      zmq::message_t zmqm((colorsize + depthsize) * m_calib_files->num());
      
      socket.recv(&zmqm); // blocking
      
      if(!drop){
      	while(m_pbo_colors.needSwap || m_pbo_depths.needSwap) {
      	  ;
      	}

    	  memcpy(&current_time, (byte*)zmqm.data(), sizeof(double));
        std::cout << "time " << current_time << std::endl;
        m_curr_frametime = current_time;
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
      	std::unique_lock<std::mutex> lock(m_mutex);
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
      
      glBindTexture(GL_TEXTURE_2D_ARRAY,m_depthArray_raw->getGLHandle());
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
      
      glBindTexture(GL_TEXTURE_2D_ARRAY,m_depthArray_raw->getGLHandle());
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
      std::unique_lock<std::mutex> lock(m_mutex);
      m_pbo_colors.needSwap = true;
      m_pbo_depths.needSwap = true;
    }
 
  }

}