#ifndef VIEW_HPP
#define VIEW_HPP

#include "Viewport.h"

#include <globjects/base/ref_ptr.h>
#include <glm/gtc/type_precision.hpp>

#include <vector>

namespace globjects {
  class Framebuffer;
  class Texture;
}

namespace kinect {
  class View {

  public:
    View(unsigned width, unsigned height, bool depth = true);
    View(unsigned width, unsigned height, std::vector<GLenum> const& layers, bool depth = true);

    void enable(bool clear_color = true, bool clear_depth =  true);
    void disable();

    void bindToTextureUnits(unsigned start_texture_unit);
    void bindToTextureUnitDepth(unsigned start_texture_unit);
    void bindToTextureUnitsRGBA(unsigned start_texture_unit);

    unsigned getWidth(unsigned i) const;
    unsigned getHeight(unsigned i) const;

    void setResolution(unsigned width, unsigned height);
    void setClearColor(glm::fvec4 const& color);
    glm::uvec2 const& resolution() const;

    globjects::Texture const* getColorTex(unsigned i) const;

  private:
    void getWidthHeight(unsigned& x, unsigned& y, unsigned& width, unsigned& height);

    glm::uvec2 m_resolution;
    glm::fvec4 m_color_clear;
    globjects::ref_ptr<globjects::Framebuffer> m_fbo;
    std::vector<globjects::ref_ptr<globjects::Texture>> m_texs_color;
    globjects::ref_ptr<globjects::Texture> m_tex_depth;
    std::vector<GLenum> m_layers;

    int m_current_fbo;
    gloost::Viewport m_viewport_current;
  };

}

#endif