#ifndef RECON_TRIGRID_HPP
#define RECON_TRIGRID_HPP

#include <ProxyMeshGridV2.h>
#include <ViewArray.h>

#include <Matrix.h>
#include <Shader.h>
#include <UniformSet.h>

#include <memory>

namespace kinect{

  class NetKinectArray;
  class CalibVolume;
  class CalibrationFiles;

  class ReconTrigrid{

  public:
    ReconTrigrid(CalibrationFiles const& cfs, CalibVolume const* cv);

    void draw(float scale);

    void reloadShader();

  private:
    CalibVolume const* m_cv;
    std::unique_ptr<gloost::Shader> m_shader_pass_depth;
    std::unique_ptr<gloost::Shader> m_shader_pass_accum;
    std::unique_ptr<gloost::Shader> m_shader_pass_normalize;
    std::unique_ptr<gloost::UniformSet> m_uniforms_pass_depth;
    std::unique_ptr<gloost::UniformSet> m_uniforms_pass_accum;
    std::unique_ptr<gloost::UniformSet> m_uniforms_pass_normalize;

    std::unique_ptr<mvt::ProxyMeshGridV2> m_proxyMesh;
    std::unique_ptr<mvt::ViewArray>     m_va_pass_depth;
    std::unique_ptr<mvt::ViewArray>     m_va_pass_accum;

    unsigned m_tex_width;
    unsigned m_tex_height;
    unsigned m_num_kinects;
    float m_min_length;
  };
}

#endif // #ifndef RECON_TRIGRID_HPP