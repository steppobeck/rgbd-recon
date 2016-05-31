#ifndef KINECT_NETKINECTARRAY_H
#define KINECT_NETKINECTARRAY_H

#include "DataTypes.h"
#include "double_buffer.hpp"

#include <glm/gtc/type_precision.hpp>

#include <glbinding/gl/gl.h>
using namespace gl;

#include <globjects/Program.h>
#include <globjects/Texture.h>
#include <globjects/Framebuffer.h>
#include <globjects/Buffer.h>

#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <map>

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
     ,dirty{false}
    {}

    double_pbo(std::size_t s)
     :size{s}
     ,ptr{nullptr}
     ,front{new globjects::Buffer()}
     ,back{new globjects::Buffer()}
     ,dirty{false}
    {
      front->setData(size, nullptr, GL_DYNAMIC_DRAW);
      front->bind(GL_PIXEL_PACK_BUFFER);
      back->setData(size, nullptr, GL_DYNAMIC_DRAW);
      back->bind(GL_PIXEL_PACK_BUFFER);
      // unbind to prevent interference with downloads
      globjects::Buffer::unbind(GL_PIXEL_PACK_BUFFER);

      map();
    }

    globjects::Buffer const* get() {
      if(dirty) {
        swapBuffers();
      }
      return front.get();
    }    

    byte* pointer() {
      return ptr;
    }

    double_pbo& operator =(double_pbo&& pbo) {
      swap(pbo);
      return * this;
    }

    std::size_t size;
    byte* ptr;
    globjects::ref_ptr<globjects::Buffer> front;
    globjects::ref_ptr<globjects::Buffer> back;
    bool dirty;

    void swapBuffers(){
      unmap();
      
      std::swap(front, back);

      map();

      dirty = false;
    }

    void swap(double_pbo& b) {
      std::swap(size, b.size);
      std::swap(ptr, b.ptr);
      std::swap(front, b.front);
      std::swap(back, b.back);
      std::swap(dirty, b.dirty);
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

    void filterTextures(bool filter);
    void useProcessedDepths(bool filter);

    void bindToTextureUnits() const;

    glm::uvec2 getDepthResolution() const;
    glm::uvec2 getColorResolution() const;

    int getTextureUnit(std::string const& name) const; 

  protected:
    void bindToFramebuffer(GLuint array_handle, GLuint layer);

    void processBackground();
    void processDepth();

    void readLoop();
    void readFromFiles();
    bool init();

    glm::uvec2 m_resolution_color;
    glm::uvec2 m_resolution_depth;

    unsigned m_numLayers;
    std::unique_ptr<mvt::TextureArray> m_colorArray;
    std::unique_ptr<mvt::TextureArray> m_depthArray_raw;
    globjects::ref_ptr<globjects::Texture> m_textures_depth;
    globjects::ref_ptr<globjects::Texture> m_textures_depth_b;
    double_buffer<globjects::ref_ptr<globjects::Texture>> m_textures_depth2;
    globjects::ref_ptr<globjects::Texture> m_textures_quality;
    globjects::ref_ptr<globjects::Texture> m_textures_normal;
    globjects::ref_ptr<globjects::Texture> m_textures_color;
    double_buffer<globjects::ref_ptr<globjects::Texture>> m_textures_bg;
    globjects::ref_ptr<globjects::Texture> m_textures_silhouette;
    globjects::ref_ptr<globjects::Framebuffer> m_fbo;
    std::unique_ptr<mvt::TextureArray>  m_colorArray_back;

    std::map<std::string, globjects::ref_ptr<globjects::Program>> m_programs;
    std::map<std::string, unsigned> m_texture_unit_offsets;

    unsigned m_colorsize; // per frame
    unsigned m_depthsize; // per frame
    double_pbo m_pbo_colors;
    double_pbo m_pbo_depths;

    std::mutex m_mutex_pbo;
    std::unique_ptr<std::thread> m_readThread;
    bool m_running;
    bool m_filter_textures;
    std::string m_serverport;
    std::size_t m_num_frame;
    double m_curr_frametime;
    bool m_use_processed_depth;
    unsigned m_start_texture_unit;
    CalibrationFiles const* m_calib_files;
    CalibVolumes const* m_calib_vols;
  };

}

#endif // #ifndef KINECT_NETKINECTARRAY_H