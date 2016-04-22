



/// mvt system includes
#include <GlPrimitives.h>



/// cpp includes
#include <string>
#include <iostream>



namespace mvt
{

/**
  \class GlPrimitives

  \brief draws primitives like Quad and QuadCenter

  \author Felix Weiszig
  \date   Mar 2011
  \remarks
*/



/*static*/ GlPrimitives* GlPrimitives::_instance = 0;


////////////////////////////////////////////////////////////////////////////////

/**
  \brief Class constructor
  \remarks ...
*/

/*static*/
GlPrimitives*
GlPrimitives::get()
{
  if(!_instance)
  {
    _instance = new GlPrimitives();
  }

  return _instance;
}

////////////////////////////////////////////////////////////////////////////////


/**
  \brief Class constructor
  \remarks ...
*/

GlPrimitives::GlPrimitives():
  _quadDisplayListHandle(0),
  _quadCenterDisplayListHandle(0),
  _coordsDisplayListHandle(0),
  _coordsDisplayListHandle2(0)
{
  init();
}


////////////////////////////////////////////////////////////////////////////////


/**
  \brief Class destructor
*/

GlPrimitives::~GlPrimitives()
{
  glDeleteLists(_quadDisplayListHandle, 1);
  glDeleteLists(_quadCenterDisplayListHandle, 1);
  glDeleteLists(_coordsDisplayListHandle, 1);
  glDeleteLists(_coordsDisplayListHandle2, 1);
}


////////////////////////////////////////////////////////////////////////////////


/**
  \brief draws the draws a quad with texCoord from 0.0 .. 1.0
*/

void
GlPrimitives::drawQuad()
{
  glCallList(_quadDisplayListHandle);
}


////////////////////////////////////////////////////////////////////////////////


/**
  \brief draws the draws a quad with texCoord from -0.5 .. 0.5
*/

void
GlPrimitives::drawQuadCenter()
{
  glCallList(_quadCenterDisplayListHandle);
}


////////////////////////////////////////////////////////////////////////////////


/**
  
*/

void
GlPrimitives::drawCoords()
{
  glLineWidth(2.0);
  glCallList(_coordsDisplayListHandle);
}


void
GlPrimitives::drawCoords2()
{
  glLineWidth(2.0);
  glCallList(_coordsDisplayListHandle2);
}


////////////////////////////////////////////////////////////////////////////////


/**
  \brief inits the GlPrimitives
*/

void
GlPrimitives::init()
{

  // quad
  _quadDisplayListHandle = glGenLists(1);

  glNewList(_quadDisplayListHandle, GL_COMPILE);
  {
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);
      glVertex2f(  0.0f, 0.0f);

      glTexCoord2f(1.0f, 0.0f);
      glVertex2f(  1.0f, 0.0f);

      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(  1.0f, 1.0f);

      glTexCoord2f(0.0f, 1.0f);
      glVertex2f(  0.0f, 1.0f);
    glEnd();
  }
  glEndList();


  // quad center
  _quadCenterDisplayListHandle = glGenLists(1);

  glNewList(_quadCenterDisplayListHandle, GL_COMPILE);
  {
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f,  0.0f);
      glVertex2f( -0.5f, -0.5f);

      glTexCoord2f(1.0f,  0.0f);
      glVertex2f(  0.5f, -0.5f);

      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(  0.5f, 0.5f);

      glTexCoord2f(0.0f, 1.0f);
      glVertex2f( -0.5f, 0.5f);
    glEnd();
  }
  glEndList();


  
  // coords
  _coordsDisplayListHandle = glGenLists(1);

  glNewList(_coordsDisplayListHandle, GL_COMPILE);
  {
    glBegin(GL_LINES);
    glColor3f(1.0,0.0,1.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(1.0,0.0,0.0);
#if 0
    for(unsigned i = 1; i < 10; ++i){
      glVertex3f(i * 0.1, i % 10 == 0 ? -0.05 : -0.02,0.0);
      glVertex3f(i * 0.1, i % 10 == 0 ? 0.05 : 0.02,0.0);
    }
#endif

    glColor3f(1.0,0.0,1.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,1.0,0.0);
#if 0
    for(unsigned i = 1; i < 10; ++i){
      glVertex3f(i % 10 == 0 ? -0.05 : -0.02, i * 0.1, 0.0);
      glVertex3f(i % 10 == 0 ?  0.05 :  0.02, i * 0.1, 0.0);
    }
#endif

    glColor3f(1.0,0.0,1.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,0.0,1.0);
#if 0
    for(unsigned i = 1; i < 10; ++i){
      glVertex3f(0.0,i % 10 == 0 ? -0.05 : -0.02, i * 0.1);
      glVertex3f(0.0,i % 10 == 0 ? 0.05 : 0.02, i * 0.1);
    }
#endif
  glEnd();

  }
  glEndList();

  // mark half size
  float ms = 0.05;

  // coords 2
  _coordsDisplayListHandle2 = glGenLists(1);

  glNewList(_coordsDisplayListHandle2, GL_COMPILE);
  {
    glBegin(GL_LINES);
    glColor3f(1.0,0.0,0.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.7,0.0,0.0);
    for(unsigned i = 1; i < 6; ++i){
      glVertex3f(i * 0.075, i % 10 == 0 ? -0.05 : -0.02, 0.0);
      glVertex3f(i * 0.075, i % 10 == 0 ? 0.05 : 0.02, 0.0);
    }

    const float ourpos = 0.7;

    // x mark
    glVertex3f(ourpos, ms*1.0, ms*1.0);
    glVertex3f(ourpos, ms*1.0,-ms*1.0);
      
    glVertex3f(ourpos, ms*1.0,-ms*1.0);
    glVertex3f(ourpos,-ms*1.0,-ms*1.0);
      
    glVertex3f(ourpos,-ms*1.0,-ms*1.0);
    glVertex3f(ourpos,-ms*1.0,-ms*1.0);
      
    glVertex3f(ourpos,-ms*1.0,-ms*1.0);
    glVertex3f(ourpos,-ms*1.0, ms*1.0);
      
    glVertex3f(ourpos,-ms*1.0, ms*1.0);
    glVertex3f(ourpos, ms*1.0, ms*1.0);
    

    glColor3f(0.0,1.0,0.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,0.7,0.0);
    for(unsigned i = 1; i < 8; ++i){
      glVertex3f(i % 10 == 0 ? -0.05 : -0.02, i * 0.075, 0.0);
      glVertex3f(i % 10 == 0 ?  0.05 :  0.02, i * 0.075, 0.0);
    }

    // y mark
    glVertex3f( ms*1.0,ourpos, ms*1.0);
    glVertex3f( ms*1.0,ourpos,-ms*1.0);
      
    glVertex3f( ms*1.0,ourpos,-ms*1.0);
    glVertex3f(-ms*1.0,ourpos,-ms*1.0);
      
    glVertex3f(-ms*1.0,ourpos,-ms*1.0);
    glVertex3f(-ms*1.0,ourpos,-ms*1.0);
      
    glVertex3f(-ms*1.0,ourpos,-ms*1.0);
    glVertex3f(-ms*1.0,ourpos, ms*1.0);
      
    glVertex3f(-ms*1.0,ourpos, ms*1.0);
    glVertex3f( ms*1.0,ourpos, ms*1.0);


    glColor3f(0.0,0.0,1.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,0.0,0.7);
#if 0
    for(unsigned i = 1; i < 10; ++i){
      glVertex3f(0.0,i % 10 == 0 ? -0.05 : -0.02, i * 0.1);
      glVertex3f(0.0,i % 10 == 0 ? 0.05 : 0.02, i * 0.1);
    }
#endif    
    // y mark
    glVertex3f( ms*1.0, ms*1.0,ourpos);
    glVertex3f( ms*1.0,-ms*1.0,ourpos);
      
    glVertex3f( ms*1.0,-ms*1.0,ourpos);
    glVertex3f(-ms*1.0,-ms*1.0,ourpos);
      
    glVertex3f(-ms*1.0,-ms*1.0,ourpos);
    glVertex3f(-ms*1.0,-ms*1.0,ourpos);
      
    glVertex3f(-ms*1.0,-ms*1.0,ourpos);
    glVertex3f(-ms*1.0, ms*1.0,ourpos);
      
    glVertex3f(-ms*1.0, ms*1.0,ourpos);
    glVertex3f( ms*1.0, ms*1.0,ourpos);
    
  glEnd();

  }
  glEndList();

}


////////////////////////////////////////////////////////////////////////////////





} // namespace mvt


