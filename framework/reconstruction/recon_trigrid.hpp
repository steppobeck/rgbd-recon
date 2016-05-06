#ifndef RECON_TRIGRID_HPP
#define RECON_TRIGRID_HPP

#include "reconstruction.hpp"
#include "ViewArray.h"

#include <globjects/Buffer.h>
#include <globjects/Program.h>
#include <globjects/VertexArray.h>

namespace kinect{

  class ReconTrigrid : public Reconstruction {

  public:
    ReconTrigrid(CalibrationFiles const& cfs, CalibVolume const* cv, gloost::BoundingBox const&  bbox);
    ~ReconTrigrid();
    
    void draw() override;

    void resize(std::size_t width, std::size_t height) override;

  private:
    std::unique_ptr<mvt::ViewArray>     m_va_pass_depth;
    std::unique_ptr<mvt::ViewArray>     m_va_pass_accum;

    globjects::VertexArray*              m_tri_grid;
    globjects::Buffer*                  m_tri_buffer;

    globjects::Program*                  m_program_accum;
    globjects::Program*                  m_program_normalize;
  };
}

#endif // #ifndef RECON_TRIGRID_HPP