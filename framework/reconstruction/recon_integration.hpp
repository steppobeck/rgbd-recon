#ifndef RECON_INTEGRATION_HPP
#define RECON_INTEGRATION_HPP

#include "reconstruction.hpp"
#include "volume_sampler.hpp"

#include <globjects/Buffer.h>
#include <globjects/Program.h>
#include <globjects/VertexArray.h>
#include <globjects/Texture.h>

namespace kinect{

  class ReconIntegration : public Reconstruction {

  public:
    ReconIntegration(CalibrationFiles const& cfs, CalibVolumes const* cv, gloost::BoundingBox const&  bbox);
    ~ReconIntegration();

    void draw() override;
    void integrate();

  private:
    globjects::Program* m_program;
    globjects::Program* m_program_integration;
    glm::uvec3          m_res_volume;
    VolumeSampler       m_sampler;
    globjects::Texture* m_volume_tsdf;

    glm::fmat4          m_mat_vol_to_world;
  };
}

#endif // #ifndef RECON_INTEGRATION_HPP