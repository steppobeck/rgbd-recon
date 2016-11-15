#ifndef MVT_VIEWARRAY_H
#define MVT_VIEWARRAY_H

#include <globjects/base/ref_ptr.h>
#include "TextureArray.h"

#include <Viewport.h>

namespace globjects {
  class Framebuffer;
}

namespace kinect {
  class ViewArray {

  public:
    ViewArray(unsigned width, unsigned height, unsigned numLayers);

    void enable(unsigned layer = 0, bool clearcolor = true);
    void disable();

    void bindToTextureUnits(unsigned start_texture_unit);
    void bindToTextureUnitDepth(unsigned start_texture_unit);
    void bindToTextureUnitRGBA(unsigned start_texture_unit);

    unsigned getWidth();
    unsigned getHeight();

    unsigned getNumLayers();

  private:
    void getWidthHeight(unsigned& x, unsigned& y, unsigned& width, unsigned& height);

    unsigned m_width;
    unsigned m_height;
    unsigned m_numLayers;

    TextureArray m_colorArray;
    TextureArray m_depthArray;
    globjects::ref_ptr<globjects::Framebuffer> m_fbo;

    int m_current_fbo;
    gloost::Viewport m_viewport_current;
  };

}

#endif // #ifndef MVT_VIEWARRAY_H