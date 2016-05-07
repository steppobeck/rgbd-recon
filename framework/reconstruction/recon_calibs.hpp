#ifndef RECON_CALIBS_HPP
#define RECON_CALIBS_HPP

#include "reconstruction.hpp"
#include "volume_sampler.hpp"
#include <ViewArray.h>

#include <globjects/Buffer.h>
#include <globjects/Program.h>
#include <globjects/VertexArray.h>

namespace kinect{

  class ReconCalibs : public Reconstruction {

  public:
    ReconCalibs(CalibrationFiles const& cfs, CalibVolume const* cv, gloost::BoundingBox const&  bbox);
    ~ReconCalibs();

    void draw() override;

    void setActiveKinect(unsigned num_kinect);

  private:
    globjects::Program* m_program;
    VolumeSampler       m_sampler;
    unsigned            m_num_kinect;
  };
}

#endif // #ifndef RECON_CALIBS_HPP