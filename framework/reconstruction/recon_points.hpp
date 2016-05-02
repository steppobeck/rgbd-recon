#ifndef RECON_POINTS_HPP
#define RECON_POINTS_HPP

#include "reconstruction.hpp"
#include <ProxyMeshGridV2.h>
#include <ViewArray.h>

#include <Shader.h>
#include <UniformSet.h>

#include <memory>

namespace kinect{

  class ReconPoints : public Reconstruction {

  public:
    ReconPoints(CalibrationFiles const& cfs, CalibVolume const* cv);

    void draw() override;

    void reload() override;

  private:
    std::unique_ptr<gloost::Shader> m_shader;

    std::unique_ptr<gloost::UniformSet> m_uniforms;

    // std::unique_ptr<mvt::ProxyMeshGridV2> m_proxyMesh;

    std::unique_ptr<mvt::ViewArray>     m_va;
  };
}

#endif // #ifndef RECON_POINTS_HPP