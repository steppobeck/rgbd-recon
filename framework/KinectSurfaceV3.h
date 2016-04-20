#ifndef KINECT_KINECTSURFACEV3_H
#define KINECT_KINECTSURFACEV3_H

#include <Matrix.h>

#include <string>

namespace gloost{
  class Shader;
  class UniformSet;
  class Obj;
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
  class ARTListener;
  class CalibVolume;
  class EvaluationVolumes;
  class VolumeSliceRenderer;

  class KinectSurfaceV3{

  public:
    KinectSurfaceV3(const char* config);
    ~KinectSurfaceV3();

    void draw(bool update, float scale);

    void reloadShader();

    void switchCalibVolume();

    NetKinectArray* getNetKinectArray();


    bool isPhoto();

    void drawMesh(bool update, float scale);

  protected:

    virtual void init(const char* config);

    void getWidthHeight(unsigned& width, unsigned& height);

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
    VolumeSliceRenderer* m_vsr;

    gloost::Shader* m_shader;
    gloost::UniformSet* m_uniforms;
    gloost::Obj* m_obj;
    CalibVolume* m_cv;
    EvaluationVolumes* m_ev;

    void trackloop();
    boost::thread* m_trackThread;
    boost::mutex* m_mutex;
    bool m_running;
    gloost::Matrix m_trackposeART;
    gloost::Matrix m_trackposeKinect;

    

  public:
    int lookup;
    unsigned viztype;
    unsigned viztype_num;
    bool black;
  };


}


#endif // #ifndef KINECT_KINECTSURFACE3_H

