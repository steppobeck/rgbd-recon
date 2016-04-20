#ifndef KINECT_KINECTSURFACE_H
#define KINECT_KINECTSURFACE_H


#include <string>

namespace gloost{
  class Shader;
  class UniformSet;
  class Obj;
}
namespace mvt
{
  class ProxyMeshGrid;
}

namespace kinect{

  class NetKinectArray;
  class ARTListener;

  class KinectSurface{

  public:
    KinectSurface(const char* config);
    ~KinectSurface();

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
    mvt::ProxyMeshGrid* m_proxyMesh;
    mvt::ProxyMeshGrid* m_proxyMesh_low;
    std::string m_hostname;

    gloost::Shader* m_shader;
    gloost::UniformSet* m_uniforms;
    gloost::Obj* m_obj;

  };


}


#endif // #ifndef KINECT_KINECTSURFACE_H

