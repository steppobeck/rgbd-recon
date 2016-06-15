#ifndef RECON_INTEGRATION_HPP
#define RECON_INTEGRATION_HPP

#include "reconstruction.hpp"
#include "volume_sampler.hpp"
#include "view_lod.hpp"

#include <globjects/base/ref_ptr.h>
namespace globjects {
  class Texture;
  class Program;
  class Framebuffer;
}

#include <memory>

namespace kinect{
  class ViewLod;

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
    
    void resize(std::size_t width, std::size_t height) override;

  private:
    globjects::ref_ptr<globjects::Program> m_program;
    globjects::ref_ptr<globjects::Program> m_program_integration;
    globjects::ref_ptr<globjects::Program> m_program_inpaint;
    globjects::ref_ptr<globjects::Program> m_program_transfer;
    std::unique_ptr<ViewLod>               m_view_inpaint;
    std::unique_ptr<ViewLod>               m_view_inpaint2;
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