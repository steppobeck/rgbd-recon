#ifndef RECONSTRUCTION_HPP
#define RECONSTRUCTION_HPP

#include "gloost/BoundingBox.h"

namespace kinect{

  class CalibVolume;
  class CalibrationFiles;

  class Reconstruction{

  public:
    Reconstruction(CalibrationFiles const& cfs, CalibVolume const* cv, gloost::BoundingBox const&  bbox);

    virtual void draw() = 0;

    virtual void reload() = 0;
    // mustnt be implemented by children without fbos
    virtual void resize(std::size_t width, std::size_t height);

  protected:
    CalibVolume const* m_cv;

    unsigned m_tex_width;
    unsigned m_tex_height;
    unsigned m_num_kinects;
    float m_min_length;
    gloost::BoundingBox m_bbox;
  };
}

#endif // #ifndef RECONSTRUCTION_HPP