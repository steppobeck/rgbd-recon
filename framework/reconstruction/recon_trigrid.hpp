#ifndef RECON_TRIGRID_HPP
#define RECON_TRIGRID_HPP

#include "reconstruction.hpp"
#include <ViewArray.h>

#include <Shader.h>
#include <UniformSet.h>

#include <memory>

#include <globjects/Buffer.h>
#include <globjects/VertexArray.h>

namespace kinect{

  class ReconTrigrid : public Reconstruction {

  public:
    ReconTrigrid(CalibrationFiles const& cfs, CalibVolume const* cv, gloost::BoundingBox const&  bbox);
    ~ReconTrigrid();
    void draw() override;

    void reload() override;

    void resize(std::size_t width, std::size_t height) override;

  private:
    std::unique_ptr<gloost::Shader> m_shader_pass_accum;
    std::unique_ptr<gloost::Shader> m_shader_pass_normalize;
    std::unique_ptr<gloost::UniformSet> m_uniforms_pass_accum;
    std::unique_ptr<gloost::UniformSet> m_uniforms_pass_normalize;

    std::unique_ptr<mvt::ViewArray>     m_va_pass_depth;
    std::unique_ptr<mvt::ViewArray>     m_va_pass_accum;
    globjects::VertexArray*              m_tri_grid;
    globjects::Buffer*                  m_tri_buffer;
  };
}

#endif // #ifndef RECON_TRIGRID_HPP