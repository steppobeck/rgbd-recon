

#include "ProxyMeshGridV2.h"



/// cpp includes
#include <string>
#include <iostream>
#include <vector>


namespace{
  struct vertex{
    float data[3];
  };
}

namespace mvt
{

/**
  \class ProxyMeshGridV2

  \brief Generates and renders a resH x resV mesh - normalized

  \author Stephan Beck
  \date   April 2011
  \remarks
*/

////////////////////////////////////////////////////////////////////////////////
/**
  \brief Class constructor
  \remarks ...
*/

ProxyMeshGridV2::ProxyMeshGridV2(unsigned resH, unsigned resV):
  _resH(resH),
  _resV(resV),
  _displayListHandle(0)
  {}

////////////////////////////////////////////////////////////////////////////////
/**
   \brief Class destructor
*/

ProxyMeshGridV2::~ProxyMeshGridV2() {
  if(_displayListHandle)
    glDeleteLists(_displayListHandle, 1);
}

////////////////////////////////////////////////////////////////////////////////
/**
  \brief inits the ProxyMeshGridV2
*/

void
ProxyMeshGridV2::init() {
  const float stepX = 1.0f/_resH;
  const float stepY = 1.0f/_resV;

  _displayListHandle = glGenLists(1);
  glNewList(_displayListHandle, GL_COMPILE);

#if 1
  glBegin( GL_TRIANGLES );
  for(unsigned y = 0; y < _resV; ++y){
    for(unsigned x = 0; x < _resH; ++x){
      glVertex2f( (x+0.5) * stepX, (y + 0.5) * stepY );
      glVertex2f( (x+1.5) * stepX, (y + 0.5) * stepY );
      glVertex2f( (x+0.5) * stepX, (y + 1.5) * stepY );

      glVertex2f( (x+1.5) * stepX, (y + 0.5) * stepY );
      glVertex2f( (x+1.5) * stepX, (y + 1.5) * stepY );
      glVertex2f( (x+0.5) * stepX, (y + 1.5) * stepY );

    }
  }
  glEnd();
#endif

  glEndList();
}
////////////////////////////////////////////////////////////////////////////////
/**
   \brief draws the ProxyMeshGridV2
*/

void
ProxyMeshGridV2::draw() {
  if (!_displayListHandle)
  {
    init();
  }
  glCallList(_displayListHandle);
  return;
}

////////////////////////////////////////////////////////////////////////////////
/**
   \brief returns horizontal resolution of the grid
*/

unsigned ProxyMeshGridV2::getResH() const {
  return _resH;
}


////////////////////////////////////////////////////////////////////////////////
/**
   \brief returns vertical resolution of the grid
*/

unsigned ProxyMeshGridV2::getResV() const {
  return _resV;
}

////////////////////////////////////////////////////////////////////////////////

} // namespace mvt