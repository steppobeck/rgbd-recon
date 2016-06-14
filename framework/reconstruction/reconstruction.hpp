#ifndef RECONSTRUCTION_HPP
#define RECONSTRUCTION_HPP

#include "gloost/BoundingBox.h"
#include "timer_gpu.hpp"

namespace kinect{

  class CalibVolumes;
  class CalibrationFiles;

  class Reconstruction{

  public:
    Reconstruction(CalibrationFiles const& cfs, CalibVolumes const* cv, gloost::BoundingBox const&  bbox);

    virtual void draw() = 0;
    virtual void drawF();

    virtual void reload();
    // mustnt be implemented by children without fbos
    virtual void resize(std::size_t width, std::size_t height);

    std::uint64_t drawTime() const;

  protected:
    CalibVolumes const* m_cv;
    CalibrationFiles const* m_cf;

    unsigned m_tex_width;
    unsigned m_tex_height;
    unsigned m_num_kinects;
    float m_min_length;
    gloost::BoundingBox m_bbox;

    TimerGPU m_timer_draw;

    static glm::uvec2 getViewportRes();
    static glm::uvec4 getViewport();
  };
}

#endif // #ifndef RECONSTRUCTION_HPP