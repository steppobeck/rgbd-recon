#ifndef RECON_TRIGRID_HPP
#define RECON_TRIGRID_HPP

#include "reconstruction.hpp"
#include <ProxyMeshGridV2.h>
#include <ViewArray.h>

#include <Shader.h>
#include <UniformSet.h>

#include <memory>

namespace kinect{

  class ReconTrigrid : public Reconstruction {

  public:
    ReconTrigrid(CalibrationFiles const& cfs, CalibVolume const* cv, gloost::BoundingBox const&  bbox);

    void draw() override;

    void reload() override;

    void resize(std::size_t width, std::size_t height) override;

  private:
    std::unique_ptr<gloost::Shader> m_shader_pass_accum;
    std::unique_ptr<gloost::Shader> m_shader_pass_normalize;
    std::unique_ptr<gloost::UniformSet> m_uniforms_pass_accum;
    std::unique_ptr<gloost::UniformSet> m_uniforms_pass_normalize;

    std::unique_ptr<mvt::ProxyMeshGridV2> m_proxyMesh;
    std::unique_ptr<mvt::ViewArray>     m_va_pass_depth;
    std::unique_ptr<mvt::ViewArray>     m_va_pass_accum;
  };
}

#endif // #ifndef RECON_TRIGRID_HPP