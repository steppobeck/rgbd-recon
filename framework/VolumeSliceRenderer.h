#ifndef KINECT_VOLUMESLICERENDERER_H
#define KINECT_VOLUMESLICERENDERER_H


namespace gloost{
  class Shader;
  class UniformSet;
}

namespace mvt{
  class ProxyMeshGridV2;
}

namespace kinect{

  class VolumeSliceRenderer{

  public:
    VolumeSliceRenderer(unsigned width, unsigned height, unsigned depth);
    ~VolumeSliceRenderer();

    void draw(gloost::Shader* s, gloost::UniformSet* u);

  private:
    unsigned m_width;
    unsigned m_height;
    unsigned m_depth;
    mvt::ProxyMeshGridV2* m_proxyMesh;

  };


}

#endif // #ifndef KINECT_VOLUMESLICERENDERER_H
