#ifndef KINECT_KINECTSURFACE3_H
#define KINECT_KINECTSURFACE3_H


#include <string>

namespace gloost{
  class Shader;
  class UniformSet;
  class Obj;
}
namespace mvt
{
  class ProxyMeshGrid;
  class ViewArray;
}

namespace kinect{

  class NetKinectArray;
  class ARTListener;

  class KinectSurface3{

  public:
    KinectSurface3(const char* config);
    ~KinectSurface3();

    void draw(bool update, float scale);
    void reloadShader();
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

    mvt::ProxyMeshGrid* m_proxyMesh;
    mvt::ViewArray*     m_va_pass_depth;
    mvt::ViewArray*     m_va_pass_accum;

    gloost::Shader* m_shader;
    gloost::UniformSet* m_uniforms;
    gloost::Obj* m_obj;

  };


}


#endif // #ifndef KINECT_KINECTSURFACE3_H

