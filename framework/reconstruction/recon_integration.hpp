#ifndef RECON_INTEGRATION_HPP
#define RECON_INTEGRATION_HPP

#include "reconstruction.hpp"
#include "volume_sampler.hpp"
#include "view_lod.hpp"
#include "view.hpp"

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
     ,indices{}
     ,baseVoxel{0}
    {}

    glm::fvec3 pos;
    glm::fvec3 size;
    std::vector<unsigned> indices;
    unsigned baseVoxel;
  };

  class ViewLod;

  class ReconIntegration : public Reconstruction {

  public:
    ReconIntegration(CalibrationFiles const& cfs, CalibVolumes const* cv, gloost::BoundingBox const&  bbo, float limit, float size);

    void draw() override;
    void drawF() override;
    void integrate();
    void setColorFilling(bool active);
    void setUseBricks(bool active);
    void setSpaceSkip(bool active);
    void setDrawBricks(bool active);
    void setVoxelSize(float size);
    void setTsdfLimit(float limit);
    void setBrickSize(float limit);

    unsigned numBricks() const;
    float occupiedRatio() const;
    float getBrickSize() const;

    void clearOccupiedBricks() const;
    void updateOccupiedBricks();
    void setMinVoxelsPerBrick(unsigned i);
    void resize(std::size_t width, std::size_t height) override;

    std::unique_ptr<ViewLod>               m_view_inpaint;
    void drawOccupiedBricks() const;

    // override from Reconstruction
    void setViewportOffset(float x, float y);
  private:
    void drawDepthLimits();
    void divideBox();
    void fillColors();

    std::unique_ptr<ViewLod>               m_view_inpaint2;
    std::unique_ptr<View>                  m_view_depth;
    globjects::ref_ptr<globjects::Buffer>  m_buffer_bricks;
    globjects::ref_ptr<globjects::Buffer>  m_buffer_occupied;

    globjects::ref_ptr<globjects::Program> m_program;
    globjects::ref_ptr<globjects::Program> m_program_integration;
    globjects::ref_ptr<globjects::Program> m_program_inpaint;
    globjects::ref_ptr<globjects::Program> m_program_colorfill;
    globjects::ref_ptr<globjects::Program> m_program_transfer;
    globjects::ref_ptr<globjects::Program> m_program_solid;
    globjects::ref_ptr<globjects::Program> m_program_bricks;

    glm::uvec3          m_res_volume;
    glm::uvec3          m_res_bricks;
    VolumeSampler       m_sampler;
    VolumeSampler       m_sampler_brick;
    globjects::ref_ptr<globjects::Texture> m_volume_tsdf;
    globjects::ref_ptr<globjects::Texture> m_tex_num_samples;

    glm::fmat4          m_mat_vol_to_world;
    std::vector<brick> m_bricks;
    std::vector<unsigned> m_active_bricks;
    std::vector<unsigned> m_bricks_occupied;
    float m_limit;
    float m_voxel_size;
    float m_brick_size;
    bool m_fill_holes;
    bool m_use_bricks;
    bool m_skip_space;
    bool m_draw_bricks;
    float m_ratio_occupied;
    unsigned m_min_voxels_per_brick;
  };
}

#endif // #ifndef RECON_INTEGRATION_HPP
