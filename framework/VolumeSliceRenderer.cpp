#include "VolumeSliceRenderer.h"


#include <ProxyMeshGridV2.h>

#include <UniformSet.h>
#include <Shader.h>

namespace kinect{


  VolumeSliceRenderer::VolumeSliceRenderer(unsigned width, unsigned height, unsigned depth)
    : m_width(width),
      m_height(height),
      m_depth(depth),
      m_proxyMesh(new mvt::ProxyMeshGridV2(width,height))
  {
    
  }
  
  VolumeSliceRenderer::~VolumeSliceRenderer(){


    delete m_proxyMesh;
  }
  

  void
  VolumeSliceRenderer::draw(gloost::Shader* s, gloost::UniformSet* u){
    const float scale = 1.0;


    u->set_float("alpha_fac", (1.0f/m_depth));
    
    for(int d = m_depth; d >= 0; --d){
      u->set_float("d_coord", (1.0f/m_depth) * d);
      u->set_int("voldepth", d);
      u->applyToShader(s);
      m_proxyMesh->draw(scale);
    }

  }
  
}
