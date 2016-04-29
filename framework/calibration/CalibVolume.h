#ifndef KINECT_CALIBVOLUME_H
#define KINECT_CALIBVOLUME_H

#include <DataTypes.h>

#include <string>
#include <vector>

namespace kinect{
  class CalibVolume{

  public:
    CalibVolume(std::vector<std::string> const& calib_volume_files);
    virtual ~CalibVolume();

    bool reload();
    
    void bindToTextureUnits(unsigned start_texture_unit);
    unsigned getStartTextureUnit() const;

  public:
    std::vector<std::string> m_cv_xyz_filenames;
    std::vector<std::string> m_cv_uv_filenames;
    std::vector<unsigned> m_cv_xyz_ids;
    std::vector<unsigned> m_cv_uv_ids;
    std::vector<unsigned> m_cv_widths;
    std::vector<unsigned> m_cv_heights;
    std::vector<unsigned> m_cv_depths;
    std::vector<float> m_cv_min_ds;
    std::vector<float> m_cv_max_ds;
    std::vector<xyz*> m_cv_xyzs;
    std::vector<uv*> m_cv_uvs;

  protected:
    unsigned m_start_texture_unit;

  public:
    static std::string serverendpoint;
  };


}

#endif // #ifndef KINECT_CALIBVOLUME_H

