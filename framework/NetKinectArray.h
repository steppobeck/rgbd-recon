#ifndef KINECT_NETKINECTARRAY_H
#define KINECT_NETKINECTARRAY_H

#include <glbinding/gl/gl.h>
using namespace gl;

#include <Matrix.h>

#include <string>
#include <vector>
#include <atomic>
#include "DataTypes.h"

#include <globjects/Program.h>
#include <globjects/Texture.h>
#include <globjects/Buffer.h>

namespace boost{
  class thread;
  class mutex;
}

namespace mvt{
  class TextureArray;
}

namespace kinect{

  struct double_pbo{
    double_pbo()
     :size{0}
     ,front{nullptr}
     ,backb{nullptr}
     ,needSwap{false}
    {}

    double_pbo(std::size_t s)
     :size{s}
     ,front{new globjects::Buffer()}
     ,backb{new globjects::Buffer()}
     ,needSwap{false}
    {
      front->setData(size, nullptr, GL_DYNAMIC_DRAW);
      front->bind(GL_PIXEL_PACK_BUFFER);
      backb->setData(size, nullptr, GL_DYNAMIC_DRAW);
      backb->bind(GL_PIXEL_PACK_BUFFER);

      map();
    }

    void map() {
      back = (byte*)backb->mapRange(0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    }

    ~double_pbo() {
      front->destroy();
      backb->destroy();
    }

    double_pbo& operator =(double_pbo&& pbo) {
      swap(pbo);
      return * this;
    }

    unsigned frontID;
    unsigned backID;
    byte* back;
    unsigned size;
    globjects::Buffer* front;
    globjects::Buffer* backb;
    std::atomic<bool> needSwap;

    void swapBuffers(){
      // backb->unmap();
      std::swap(frontID, backID);
      std::swap(front, backb);

      // map();

      // needSwap = false;
    }

    void swap(double_pbo& b) {
      std::swap(frontID, b.frontID);
      std::swap(backID, b.backID);
      std::swap(back, b.back);
      std::swap(size, b.size);
      std::swap(front, b.front);
      std::swap(backb, b.backb);
      // std::swap(needSwap, b.needSwap);
      bool a_swap = needSwap;
      if(b.needSwap) {
        needSwap = true;
      }
      else {
        needSwap = false;
      }
      b.needSwap = a_swap;
    }
  };

inline void swap(double_pbo& a, double_pbo& b) {
  a.swap(b);
}

  class KinectCalibrationFile;
  class CalibrationFiles;
  class CalibVolume;

  class NetKinectArray{

  public:
    NetKinectArray(std::string const& serverport, CalibrationFiles const* calibs, CalibVolume const* vols, bool readfromfile = false);

    NetKinectArray(std::vector<KinectCalibrationFile*>& calibs);

    virtual ~NetKinectArray();

    virtual void update();

    void bilateralFilter();

    void setStartTextureUnit(unsigned m_start_texture_unit);

    unsigned getStartTextureUnit() const;
    std::vector<int> getColorTextureUnits() const;
    std::vector<int> getDepthTextureUnits() const;

    std::vector<KinectCalibrationFile*> const& getCalibs() const;

    void writeCurrentTexture(std::string prefix);
    void writeBMP(std::string, std::vector<std::uint8_t> const&, unsigned int offset, unsigned int bytesPerPixel);
    
    mvt::TextureArray* getDepthArrayBack();
    mvt::TextureArray* getDepthArray();

    void bindToTextureUnits() const;
  protected:
    void bindToFramebuffer(GLuint array_handle, GLuint layer);

    void bindBackToTextureUnits() const;

    void readLoop();
    void readFromFiles();
    bool init();
    unsigned m_width;
    unsigned m_widthc;
    unsigned m_height;
    unsigned m_heightc;

    unsigned m_numLayers;
    mvt::TextureArray* m_colorArray;
    mvt::TextureArray* m_depthArray;
    globjects::Texture* m_textures_quality;
    mvt::TextureArray*  m_colorArray_back;
    mvt::TextureArray*  m_depthArray_back;

    globjects::Program*                  m_program_filter;

    unsigned m_fboID;

    unsigned m_colorsize; // per frame
    unsigned m_depthsize; // per frame
    double_pbo m_colorsCPU3;
    double_pbo m_depthsCPU3;

    boost::mutex* m_mutex;
    boost::thread* m_readThread;
    bool m_running;
    std::string m_serverport;

    unsigned m_start_texture_unit;
    CalibrationFiles const* m_calib_files;
    CalibVolume const* m_calib_vols;
  public:
    bool depth_compression_lex;
    float depth_compression_ratio;
  };

}

#endif // #ifndef KINECT_NETKINECTARRAY_H