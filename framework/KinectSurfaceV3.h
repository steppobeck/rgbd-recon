#ifndef KINECT_KINECTSURFACEV3_H
#define KINECT_KINECTSURFACEV3_H

#include <ProxyMeshGridV2.h>
#include <ViewArray.h>

#include <Matrix.h>
#include <Shader.h>
#include <UniformSet.h>

#include <memory>

namespace kinect{

  class NetKinectArray;
  class CalibVolume;

  class KinectSurfaceV3{

  public:
    KinectSurfaceV3(const char* config);

    void draw(bool update, float scale);

    void reloadShader();

    NetKinectArray* getNetKinectArray();

  protected:

    std::unique_ptr<NetKinectArray>m_nka;
    std::unique_ptr<gloost::Shader> m_shader_pass_depth;
    std::unique_ptr<gloost::Shader> m_shader_pass_accum;
    std::unique_ptr<gloost::Shader> m_shader_pass_normalize;
    std::unique_ptr<gloost::UniformSet> m_uniforms_pass_depth;
    std::unique_ptr<gloost::UniformSet> m_uniforms_pass_accum;
    std::unique_ptr<gloost::UniformSet> m_uniforms_pass_normalize;

    std::unique_ptr<mvt::ProxyMeshGridV2> m_proxyMesh;
    std::unique_ptr<mvt::ViewArray>     m_va_pass_depth;
    std::unique_ptr<mvt::ViewArray>     m_va_pass_accum;

    std::unique_ptr<CalibVolume> m_cv;
  };
}

#endif // #ifndef KINECT_KINECTSURFACE3_H