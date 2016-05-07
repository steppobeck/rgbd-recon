#ifndef KINECT_CALIBVOLUME_H
#define KINECT_CALIBVOLUME_H

#include <DataTypes.h>

#include <string>
#include <vector>

#include <globjects/Buffer.h>
#include <globjects/Texture.h>

namespace kinect{
  class CalibVolume{

  public:
    CalibVolume(std::vector<std::string> const& calib_volume_files);
    virtual ~CalibVolume();

    void reload();
    
    void bindToTextureUnits(unsigned start_texture_unit);
    unsigned getStartTextureUnit() const;

    std::vector<int> getXYZVolumeUnits() const;
    std::vector<int> getUVVolumeUnits() const;

  public:
    std::vector<std::string> m_cv_xyz_filenames;
    std::vector<std::string> m_cv_uv_filenames;

    std::vector<globjects::Texture*> m_volumes_xyz;
    std::vector<globjects::Texture*> m_volumes_uv;

    std::vector<unsigned> m_cv_widths;
    std::vector<unsigned> m_cv_heights;
    std::vector<unsigned> m_cv_depths;
    std::vector<float> m_cv_min_ds;
    std::vector<float> m_cv_max_ds;

    globjects::Buffer*    m_buffer_minmax_d;

  protected:
    int m_start_texture_unit;

    void addVolume(std::string const& filename_xyz, std::string filename_uv);
    void uploadMinMaXDepths() const;

  public:
    static std::string serverendpoint;
  };


}

#endif // #ifndef KINECT_CALIBVOLUME_H

