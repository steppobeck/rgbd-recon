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
  struct brick {
    brick(glm::fvec3 const& p, glm::fvec3 const& s)
     :pos{p}
     ,size{s}
    {}

    glm::fvec3 pos;
    glm::fvec3 size;
  };

  class ViewLod;

  class ReconIntegration : public Reconstruction {

  public:
    ReconIntegration(CalibrationFiles const& cfs, CalibVolumes const* cv, gloost::BoundingBox const&  bbo, float limit, float size);

    void draw() override;
    void drawF() override;
    void integrate();
    void setColorFilling(bool active);
    void setVoxelSize(float size);
    void setTsdfLimit(float limit);

    std::uint64_t integrationTime() const;
    std::uint64_t holefillTime() const;
    
    void resize(std::size_t width, std::size_t height) override;
    void drawBricks() const;

    std::unique_ptr<ViewLod>               m_view_inpaint;
  private:
    void divideBox();
    void fillColors();
    std::unique_ptr<ViewLod>               m_view_inpaint2;
    globjects::ref_ptr<globjects::Program> m_program;
    globjects::ref_ptr<globjects::Program> m_program_integration;
    globjects::ref_ptr<globjects::Program> m_program_inpaint;
    globjects::ref_ptr<globjects::Program> m_program_colorfill;
    globjects::ref_ptr<globjects::Program> m_program_transfer;
    globjects::ref_ptr<globjects::Program> m_program_solid;
    glm::uvec3          m_res_volume;
    VolumeSampler       m_sampler;
    globjects::ref_ptr<globjects::Texture> m_volume_tsdf;

    glm::fmat4          m_mat_vol_to_world;
    std::vector<brick> m_bricks;
    float m_limit;
    float m_voxel_size;
    float m_brick_size;
    bool m_fill_holes;
    TimerGPU m_timer_integration;
    TimerGPU m_timer_holefill;
  };
}

#endif // #ifndef RECON_INTEGRATION_HPP