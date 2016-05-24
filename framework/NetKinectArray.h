#ifndef KINECT_NETKINECTARRAY_H
#define KINECT_NETKINECTARRAY_H

#include <glm/gtc/type_precision.hpp>

#include <glbinding/gl/gl.h>
using namespace gl;

#include <Matrix.h>
#include "DataTypes.h"


#include <globjects/Program.h>
#include <globjects/Texture.h>
#include <globjects/Framebuffer.h>
#include <globjects/Buffer.h>

#include <string>
#include <vector>
#include <atomic>
#include <mutex>

namespace std{
  class thread;
}

namespace mvt{
  class TextureArray;
}

namespace kinect{

  struct double_pbo{
    double_pbo()
     :size{0}
     ,ptr{nullptr}
     ,front{nullptr}
     ,back{nullptr}
     ,needSwap{false}
    {}

    double_pbo(std::size_t s)
     :size{s}
     ,ptr{nullptr}
     ,front{new globjects::Buffer()}
     ,back{new globjects::Buffer()}
     ,needSwap{false}
    {
      front->setData(size, nullptr, GL_DYNAMIC_DRAW);
      front->bind(GL_PIXEL_PACK_BUFFER);
      back->setData(size, nullptr, GL_DYNAMIC_DRAW);
      back->bind(GL_PIXEL_PACK_BUFFER);
      // unbind to prevent interference with downloads
      globjects::Buffer::unbind(GL_PIXEL_PACK_BUFFER);

      map();
    }

    byte* pointer() {
      return ptr;
    }

    ~double_pbo() {
      front->destroy();
      back->destroy();
    }

    double_pbo& operator =(double_pbo&& pbo) {
      swap(pbo);
      return * this;
    }

    std::size_t size;
    byte* ptr;
    globjects::Buffer* front;
    globjects::Buffer* back;
    std::atomic<bool> needSwap;

    void swapBuffers(){
      unmap();
      
      std::swap(front, back);

      map();

      needSwap = false;
    }

    void swap(double_pbo& b) {
      std::swap(size, b.size);
      std::swap(ptr, b.ptr);
      std::swap(front, b.front);
      std::swap(back, b.back);

      bool a_swap = needSwap;
      if(b.needSwap) {
        needSwap = true;
      }
      else {
        needSwap = false;
      }
      b.needSwap = a_swap;
    }
  private:
    void unmap() {
      back->unmap();
      ptr = nullptr;
    }
    void map() {
      ptr = (byte*)back->mapRange(0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    }
  };

inline void swap(double_pbo& a, double_pbo& b) {
  a.swap(b);
}

  class KinectCalibrationFile;
  class CalibrationFiles;
  class CalibVolumes;

  class NetKinectArray{

  public:
    NetKinectArray(std::string const& serverport, CalibrationFiles const* calibs, CalibVolumes const* vols, bool readfromfile = false);

    NetKinectArray(std::vector<KinectCalibrationFile*>& calibs);

    virtual ~NetKinectArray();

    virtual void update();

    void processTextures();
    void setStartTextureUnit(unsigned m_start_texture_unit);

    unsigned getStartTextureUnit() const;

    std::vector<KinectCalibrationFile*> const& getCalibs() const;

    void writeCurrentTexture(std::string prefix);
    void writeBMP(std::string, std::vector<std::uint8_t> const&, unsigned int offset, unsigned int bytesPerPixel);
    
    mvt::TextureArray* getDepthArrayBack();
    mvt::TextureArray* getDepthArray();

    void filterTextures(bool filter);

    void bindToTextureUnits() const;

    glm::uvec2 getDepthResolution() const;
    glm::uvec2 getColorResolution() const;

  protected:
    void bindToFramebuffer(GLuint array_handle, GLuint layer);

    void processBackground();

    void readLoop();
    void readFromFiles();
    bool init();

    unsigned m_width;
    unsigned m_widthc;
    unsigned m_height;
    unsigned m_heightc;

    unsigned m_numLayers;
    std::unique_ptr<mvt::TextureArray> m_colorArray;
    std::unique_ptr<mvt::TextureArray> m_depthArray;
    globjects::ref_ptr<globjects::Texture> m_textures_quality;
    globjects::ref_ptr<globjects::Texture> m_textures_normal;
    globjects::ref_ptr<globjects::Texture> m_textures_bg;
    globjects::ref_ptr<globjects::Texture> m_textures_bg_back;
    globjects::ref_ptr<globjects::Texture> m_textures_silhouette;
    globjects::ref_ptr<globjects::Framebuffer> m_fbo;
    std::unique_ptr<mvt::TextureArray>  m_colorArray_back;
    std::unique_ptr<mvt::TextureArray>  m_depthArray_back;

    globjects::ref_ptr<globjects::Program> m_program_filter;
    globjects::ref_ptr<globjects::Program> m_program_normal;
    globjects::ref_ptr<globjects::Program> m_program_quality;
    globjects::ref_ptr<globjects::Program> m_program_bg;

    unsigned m_colorsize; // per frame
    unsigned m_depthsize; // per frame
    double_pbo m_pbo_colors;
    double_pbo m_pbo_depths;

    std::mutex m_mutex;
    std::unique_ptr<std::thread> m_readThread;
    bool m_running;
    bool m_filter_textures;
    std::string m_serverport;
    std::size_t m_num_frame;
    double m_curr_frametime;

    unsigned m_start_texture_unit;
    CalibrationFiles const* m_calib_files;
    CalibVolumes const* m_calib_vols;
  public:
    bool depth_compression_lex;
    float depth_compression_ratio;
  };

}

#endif // #ifndef KINECT_NETKINECTARRAY_H