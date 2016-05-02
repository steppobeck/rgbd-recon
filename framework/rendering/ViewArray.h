#ifndef MVT_VIEWARRAY_H
#define MVT_VIEWARRAY_H



#include <GL/glew.h>
#include <GL/gl.h>

#include <Viewport.h>

namespace mvt{


  class TextureArray;

  class ViewArray{

  public:
    ViewArray(unsigned width, unsigned height, unsigned numLayers);
    ~ViewArray();

    void init();
    void enable(unsigned layer, bool use_vp = true, unsigned* ox = 0, unsigned* oy = 0, bool clearcolor = true);
    void disable(bool use_vp = true);

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
    GLuint m_glHandle;

    TextureArray* m_colorArray;
    TextureArray* m_depthArray;

    GLint m_current_fbo;
    gloost::Viewport m_viewport;
    gloost::Viewport m_viewport_current;
  };


}


#endif // #ifndef MVT_VIEWARRAY_H
