#ifndef RECON_CALIBS_HPP
#define RECON_CALIBS_HPP

#include "reconstruction.hpp"
#include "volume_sampler.hpp"
#include <ViewArray.h>

#include <globjects/Buffer.h>
#include <globjects/Program.h>
#include <globjects/VertexArray.h>
#include <globjects/Texture.h>

namespace kinect{

  class ReconCalibs : public Reconstruction {

  public:
    ReconCalibs(CalibrationFiles const& cfs, CalibVolume const* cv, gloost::BoundingBox const&  bbox);
    ~ReconCalibs();

    void draw() override;
    void process();

    void setActiveKinect(unsigned num_kinect);

    std::vector<int> getXYZVolumeUnits() const;
    std::vector<int> getUVVolumeUnits() const;
  private:
    void bindToTextureUnits();
    // unsigned getStartTextureUnit() const;


    globjects::Program* m_program;
    globjects::Program* m_program_sample;
    VolumeSampler       m_sampler;
    unsigned            m_num_kinect;
    std::vector<globjects::Texture*> m_volumes_xyz;
    std::vector<globjects::Texture*> m_volumes_uv;

    unsigned m_start_texture_unit;
  };
}

#endif // #ifndef RECON_CALIBS_HPP