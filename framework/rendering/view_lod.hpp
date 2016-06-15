#ifndef VIEW_LOD_HPP
#define VIEW_LOD_HPP

#include <globjects/base/ref_ptr.h>

#include <Viewport.h>

namespace globjects {
  class Framebuffer;
  class Texture;
}

namespace kinect {
  class ViewLod {

  public:
    ViewLod(unsigned width, unsigned height, unsigned num_lod = 0);

    void enable(unsigned lod = 0, bool clearcolor = true);
    void disable();

    void bindToTextureUnits(unsigned start_texture_unit);
    void bindToTextureUnitDepth(unsigned start_texture_unit);
    void bindToTextureUnitRGBA(unsigned start_texture_unit);

    unsigned getWidth();
    unsigned getHeight();

    unsigned getNumLods();

  private:
    void getWidthHeight(unsigned& x, unsigned& y, unsigned& width, unsigned& height);

    unsigned m_width;
    unsigned m_height;
    unsigned m_num_lods;

    globjects::ref_ptr<globjects::Framebuffer> m_fbo;
    globjects::ref_ptr<globjects::Texture> m_tex_color;
    globjects::ref_ptr<globjects::Texture> m_tex_depth;

    int m_current_fbo;
    gloost::Viewport m_viewport_current;
  };

}

#endif