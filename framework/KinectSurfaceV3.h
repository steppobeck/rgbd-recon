#ifndef KINECT_KINECTSURFACEV3_H
#define KINECT_KINECTSURFACEV3_H

#include <Matrix.h>

#include <string>

namespace gloost{
  class Shader;
  class UniformSet;
}
namespace mvt
{
  class ProxyMeshGridV2;
  class ViewArray;
}

namespace boost{
  class thread;
  class mutex;
}


namespace kinect{

  class NetKinectArray;
  class CalibVolume;

  class KinectSurfaceV3{

  public:
    KinectSurfaceV3(const char* config);
    ~KinectSurfaceV3();

    void draw(bool update, float scale);

    void reloadShader();

    NetKinectArray* getNetKinectArray();

  protected:

    virtual void init(const char* config);

    std::string m_config;
    std::string m_hostname;
    NetKinectArray* m_nka;
    gloost::Shader* m_shader_pass_depth;
    gloost::Shader* m_shader_pass_accum;
    gloost::Shader* m_shader_pass_normalize;
    gloost::UniformSet* m_uniforms_pass_depth;
    gloost::UniformSet* m_uniforms_pass_accum;
    gloost::UniformSet* m_uniforms_pass_normalize;

    mvt::ProxyMeshGridV2* m_proxyMesh;
    mvt::ViewArray*     m_va_pass_depth;
    mvt::ViewArray*     m_va_pass_accum;
    
    gloost::Shader*     m_shader_pass_volviz;
    gloost::UniformSet* m_uniforms_pass_volviz;
    mvt::ViewArray*     m_va_pass_volviz;

    CalibVolume* m_cv;
    boost::mutex* m_mutex;
    bool m_running;

  public:
    int lookup;
    unsigned viztype;
    unsigned viztype_num;
    bool black;
  };


}


#endif // #ifndef KINECT_KINECTSURFACE3_H

