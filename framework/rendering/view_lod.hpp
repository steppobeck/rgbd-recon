#ifndef VIEW_LOD_HPP
#define VIEW_LOD_HPP

#include "Viewport.h"

#include <globjects/base/ref_ptr.h>
#include <glm/gtc/type_precision.hpp>

#include <vector>

namespace globjects {
  class Framebuffer;
  class Texture;
}

namespace kinect {
  class ViewLod {

  public:
    ViewLod(unsigned width, unsigned height, unsigned num_lod = 1);

    void enable(unsigned lod = 0, bool clearcolor = true);
    void disable();

    void bindToTextureUnits(unsigned start_texture_unit);
    void bindToTextureUnitDepth(unsigned start_texture_unit);
    void bindToTextureUnitRGBA(unsigned start_texture_unit);

    unsigned getWidth(unsigned i) const;
    unsigned getHeight(unsigned i) const;

    void setResolution(unsigned width, unsigned height);
    glm::uvec2 resolution(unsigned i) const;

    unsigned getNumLods() const;

  private:
    void getWidthHeight(unsigned& x, unsigned& y, unsigned& width, unsigned& height);

    unsigned m_width;
    unsigned m_height;
    std::vector<glm::uvec2> m_resolutions;

    globjects::ref_ptr<globjects::Framebuffer> m_fbo;
    globjects::ref_ptr<globjects::Texture> m_tex_color;
    globjects::ref_ptr<globjects::Texture> m_tex_depth;

    int m_current_fbo;
    gloost::Viewport m_viewport_current;
  };

}

#endif