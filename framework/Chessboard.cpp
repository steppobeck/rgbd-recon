#include "Chessboard.h"








namespace mvt{

  Chessboard::Chessboard(float width, float height, unsigned resH, unsigned resV)
    : m_width(width),
      m_height(height),
      m_resH(resH),
      m_resV(resV),
      m_displayListHandle(0)
  {}


  // class destructor
  Chessboard::~Chessboard(){
    if(m_displayListHandle)
      glDeleteLists(m_displayListHandle, 1);
  }


    // inits the opengl stuff
  void
  Chessboard::init(){

    m_displayListHandle = glGenLists(1);

    glNewList(m_displayListHandle, GL_COMPILE);{

    glBegin(GL_QUADS);
    {
      for(unsigned y = 0; y < m_resV; ++y){
	for(unsigned x = 0; x < m_resH; ++x){
	  if(y % 2 == 0){
	    if(x % 2 == 0){
	      glColor4f(0.0,0.0,0.0,1.0);
	    }
	    else{
	      glColor4f(1.0,1.0,1.0,1.0);
	    }
	  }
	  else{
	    if(x % 2 == 1){
	      glColor4f(0.0,0.0,0.0,1.0);
	    }
	    else{
	      glColor4f(1.0,1.0,1.0,1.0);
	    }
	  }

	  glVertex2f((x * m_width) + 0.0,    (y * m_height) + 0.0);
	  glVertex2f((x * m_width) + m_width,(y * m_height) + 0.0);
	  glVertex2f((x * m_width) + m_width,(y * m_height) + m_height);
	  glVertex2f((x * m_width) + 0.0,    (y * m_height) + m_height);

	}
      }

    }
    glEnd();
    }
    glEndList();

  }

   

  // draws the chessbaord
  void
  Chessboard::draw(){
    if(0 == m_displayListHandle)
      init();

    glCallList(m_displayListHandle);

  }


} // namespace mvt

