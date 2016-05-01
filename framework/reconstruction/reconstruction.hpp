#ifndef RECONSTRUCTION_HPP
#define RECONSTRUCTION_HPP

namespace kinect{

  class CalibVolume;
  class CalibrationFiles;

  class Reconstruction{

  public:
    Reconstruction(CalibrationFiles const& cfs, CalibVolume const* cv);

    virtual void draw() = 0;

    virtual void reload() = 0;

  protected:
    CalibVolume const* m_cv;

    unsigned m_tex_width;
    unsigned m_tex_height;
    unsigned m_num_kinects;
    float m_min_length;
  };
}

#endif // #ifndef RECONSTRUCTION_HPP