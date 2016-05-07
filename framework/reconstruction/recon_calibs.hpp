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

  private:
    globjects::Program* m_program;
    VolumeSampler       m_sampler;
    unsigned            m_num_kinect;
    std::vector<globjects::Texture*> m_volumes_xyz;
    std::vector<globjects::Texture*> m_volumes_uv;
  };
}

#endif // #ifndef RECON_CALIBS_HPP