#ifndef RECON_POINTS_HPP
#define RECON_POINTS_HPP

#include "reconstruction.hpp"
#include <ProxyMeshGridV2.h>
#include <ViewArray.h>

#include <Shader.h>
#include <UniformSet.h>

namespace kinect{

  class ReconPoints : public Reconstruction {

  public:
    ReconPoints(CalibrationFiles const& cfs, CalibVolume const* cv, gloost::BoundingBox const&  bbox);

    void draw() override;

    void reload() override;

  private:
    gloost::Shader m_shader;

    gloost::UniformSet m_uniforms;

    // mvt::ViewArray     m_va;
  };
}

#endif // #ifndef RECON_POINTS_HPP