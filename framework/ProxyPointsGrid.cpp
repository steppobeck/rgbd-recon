



/// gloost system includes
#include <ProxyPointsGrid.h>



/// cpp includes
#include <string>
#include <iostream>



namespace kinect
{

/**
  \class ProxyPointsGrid

  \brief Generates and renders resH x resV points in a uniform raster grid

  \author Felix Weiszig
  \date   Mar 2011
  \remarks
*/

////////////////////////////////////////////////////////////////////////////////


/**
  \brief Class constructor
  \remarks ...
*/

ProxyPointsGrid::ProxyPointsGrid(unsigned int resH,
                                 unsigned int resV):
  _resH(resH),
  _resV(resV),
  _displayListHandle(0)
{

}


////////////////////////////////////////////////////////////////////////////////


/**
  \brief Class destructor
*/

ProxyPointsGrid::~ProxyPointsGrid()
{
	glDeleteLists(_displayListHandle, 1);
}


////////////////////////////////////////////////////////////////////////////////


/**
  \brief inits the ProxyPointsGrid
*/

void
ProxyPointsGrid::init()
{

	const float stepX = 1.0f/_resH;
  const float stepY = 1.0f/_resV;

  _displayListHandle = glGenLists(1);

  glNewList(_displayListHandle, GL_COMPILE);
  {
    glBegin(GL_POINTS);
    {
      for(unsigned int y=0; y != _resV; ++y)
      {
        for(unsigned int x=0; x != _resH; ++x)
        {
          glVertex2f((0.5f + x) * stepX,
                     (0.5f + y) * stepY);
        }
      }
    }
    glEnd();
  }
  glEndList();

}


////////////////////////////////////////////////////////////////////////////////


/**
  \brief draws the ProxyPointsGrid
*/

void
ProxyPointsGrid::draw()
{
	if (!_displayListHandle)
	{
	  init();
	}
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
  glCallList(_displayListHandle);
}


////////////////////////////////////////////////////////////////////////////////


/**
  \brief returns horizontal resolution of the grid
*/

unsigned int
ProxyPointsGrid::getResH() const
{
  return _resH;
}


////////////////////////////////////////////////////////////////////////////////


/**
  \brief returns vertical resolution of the grid
*/

unsigned int
ProxyPointsGrid::getResV() const
{
  return _resV;
}


////////////////////////////////////////////////////////////////////////////////





} // namespace kinect


