#ifndef KINECT_NETKINECTARRAY_H
#define KINECT_NETKINECTARRAY_H

#include "DataTypes.h"
#include "double_buffer.hpp"
#include "double_pixel_buffer.hpp"

#include <glm/gtc/type_precision.hpp>

#include <globjects/base/ref_ptr.h>
namespace globjects {
  class Buffer;
  class Program;
  class Framebuffer;
  class Texture;
}

#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <map>

namespace std{
  class thread;
}

namespace kinect {

  class TextureArray;
  class KinectCalibrationFile;
  class CalibrationFiles;
  class CalibVolumes;

  class NetKinectArray {

  public:
    NetKinectArray(std::string const& serverport, CalibrationFiles const* calibs, CalibVolumes const* vols, bool readfromfile = false);
    NetKinectArray(std::vector<KinectCalibrationFile*>& calibs);

    ~NetKinectArray();

    void update();

    void processTextures();
    void setStartTextureUnit(unsigned m_start_texture_unit);

    unsigned getStartTextureUnit() const;

    std::vector<KinectCalibrationFile*> const& getCalibs() const;

    void writeCurrentTexture(std::string prefix);
    void writeBMP(std::string, std::vector<std::uint8_t> const&, unsigned int offset, unsigned int bytesPerPixel);

    void filterTextures(bool filter);
    void useProcessedDepths(bool filter);
    void refineBoundary(bool filter);

    void bindToTextureUnits() const;

    glm::uvec2 getDepthResolution() const;
    glm::uvec2 getColorResolution() const;

    int getTextureUnit(std::string const& name) const; 
    std::size_t getStageTime(std::string const& name) const; 

  private:
    void processBackground();
    void processDepth();

    void readLoop();
    void readFromFiles();
    bool init();

    glm::uvec2 m_resolution_color;
    glm::uvec2 m_resolution_depth;

    unsigned m_numLayers;
    std::unique_ptr<TextureArray> m_colorArray;
    std::unique_ptr<TextureArray> m_depthArray_raw;
    globjects::ref_ptr<globjects::Texture> m_textures_depth;
    globjects::ref_ptr<globjects::Texture> m_textures_depth_b;
    double_buffer<globjects::ref_ptr<globjects::Texture>> m_textures_depth2;
    globjects::ref_ptr<globjects::Texture> m_textures_quality;
    globjects::ref_ptr<globjects::Texture> m_textures_normal;
    globjects::ref_ptr<globjects::Texture> m_textures_color;
    double_buffer<globjects::ref_ptr<globjects::Texture>> m_textures_bg;
    globjects::ref_ptr<globjects::Texture> m_textures_silhouette;
    globjects::ref_ptr<globjects::Framebuffer> m_fbo;
    std::unique_ptr<TextureArray>  m_colorArray_back;

    std::map<std::string, globjects::ref_ptr<globjects::Program>> m_programs;
    std::map<std::string, unsigned> m_texture_unit_offsets;
    std::map<std::string, std::size_t> m_times_stages;

    unsigned m_colorsize; // per frame
    unsigned m_depthsize; // per frame
    double_pbo m_pbo_colors;
    double_pbo m_pbo_depths;

    std::mutex m_mutex_pbo;
    std::unique_ptr<std::thread> m_readThread;
    bool m_running;
    bool m_filter_textures;
    bool m_refine_bound;
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