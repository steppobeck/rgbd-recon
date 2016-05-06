#ifndef H_PMD_PROXY_MESHV2_GRID
#define H_PMD_PROXY_MESHV2_GRID

/// gloost system includes
#include <gloostConfig.h>
#include <gloostMath.h>

/// cpp includes
#include <glbinding/gl/gl.h>
using namespace gl;

namespace mvt
{

//  Generates and renders a resH x resV mesh - normalized
class ProxyMeshGridV2
{
 public:
  // class constructor
  ProxyMeshGridV2(unsigned resH, unsigned resV);

  // class destructor
  ~ProxyMeshGridV2();


  // inits the ProxyMeshGridV2
  void init();

  // draws the ProxyMeshGridV2
  void draw();

  // returns horizontal resolution of the grid
  unsigned getResH () const;
  // returns vertical resolution of the grid
  unsigned getResV () const;

 protected:
  unsigned _resH;
  unsigned _resV;

  GLuint _displayListHandle;
};

} // namespace mvt

#endif // H_PMD_PROXY_MESHV2_GRID