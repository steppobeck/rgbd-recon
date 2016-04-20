#ifndef KINECT_KINECTSURFACEV2_H
#define KINECT_KINECTSURFACEV2_H


#include <string>

namespace gloost{
  class Shader;
  class UniformSet;
  class Obj;
}
namespace mvt
{
  class ProxyMeshGridV2;
}

namespace kinect{

  class NetKinectArray;
  class ARTListener;
  class CalibVolume;

  class KinectSurfaceV2{

  public:
    KinectSurfaceV2(const char* config);
    ~KinectSurfaceV2();

    void draw(bool update, float scale, bool warpvis = false);
    void reloadShader();
    NetKinectArray* getNetKinectArray();


    bool isPhoto();

    void drawMesh(bool update, float scale);

  protected:

    virtual void init(const char* config);


    std::string m_config;
    NetKinectArray* m_nka;
    gloost::Shader* m_shaderWarp;
    gloost::UniformSet* m_uniformsWarp;
    mvt::ProxyMeshGridV2* m_proxyMesh;
    mvt::ProxyMeshGridV2* m_proxyMesh_low;
    std::string m_hostname;

    gloost::Shader* m_shader;
    gloost::UniformSet* m_uniforms;
    gloost::Obj* m_obj;
    CalibVolume* m_cv;

  public:
    int lookup;

    bool do_capture;
    bool do_apply;
    bool do_clear;
    void toggleCalibMode();
    void saveCalibVolumes();
    void saveSamplePoints(const char* bfname, unsigned stride);
    void dumpCalibVolumesSamplingPoints();
  };


}


#endif // #ifndef KINECT_KINECTSURFACE_H

