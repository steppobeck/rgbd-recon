#ifndef RECON_INTEGRATION_HPP
#define RECON_INTEGRATION_HPP

#include "reconstruction.hpp"
#include "volume_sampler.hpp"

#include <globjects/base/ref_ptr.h>
namespace globjects {
  class Texture;
  class Program;
  class Framebuffer;
}

namespace kinect{

  class ReconIntegration : public Reconstruction {

  public:
    ReconIntegration(CalibrationFiles const& cfs, CalibVolumes const* cv, gloost::BoundingBox const&  bbo, float limit, float size);

    void draw() override;
    void drawF() override;
    void integrate();
    void fillColors();

    void setVoxelSize(float size);
    void setTsdfLimit(float limit);

    std::uint64_t integrationTime() const;

  private:
    globjects::ref_ptr<globjects::Program> m_program;
    globjects::ref_ptr<globjects::Program> m_program_integration;
    globjects::ref_ptr<globjects::Program> m_program_inpaint;

    glm::uvec3          m_res_volume;
    VolumeSampler       m_sampler;
    globjects::ref_ptr<globjects::Texture> m_volume_tsdf;

    glm::fmat4          m_mat_vol_to_world;

    float m_limit;
    float m_voxel_size;

    TimerGPU m_timer_integration;
  };
}

#endif // #ifndef RECON_INTEGRATION_HPP