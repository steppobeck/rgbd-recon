#include "FourTiledWindow.h"

#include <glbinding/gl/gl.h>
using namespace gl;
#include <GL/glut.h>

namespace mvt{

  FourTiledWindow::FourTiledWindow(unsigned width, unsigned height)
    : m_width(width),
      m_height(height),
      m_mouseX(0),
      m_mouseY(0),
      m_button(0),
      m_pressed(0),
      m_activeWindow(0),
      m_buttonSpeeds()
  {
    endFrame();
  }

  void
  FourTiledWindow::resize(unsigned width, unsigned height){
    m_width = width;
    m_height = height;
  }


  void FourTiledWindow::setViewport(unsigned windowNum){
    switch (windowNum){
    case 0: // TOPLEFT
      glViewport(0.0,m_height * 0.5, m_width * 0.5, m_height * 0.5);
      break;

    case 1: // TOPRIGHT
      glViewport(m_width * 0.5,m_height * 0.5, m_width * 0.5, m_height * 0.5);
      break;

    case 2: // BOTTOMLEFT
      glViewport(0.0,0.0, m_width * 0.5, m_height * 0.5);
      break;

    case 3: // BOTTOMRIGHT
      glViewport(m_width * 0.5,0.0, m_width * 0.5, m_height * 0.5);
      break;

    default:
      glViewport(0.0,0.0,m_width, m_height);
      break;
    }
  }

  float
  FourTiledWindow::getAspect(unsigned windowNum){
    switch (windowNum){
    case 0: // TOPLEFT
    case 1: 
    case 2: 
    case 3: 
      return (1.0 * m_width)/m_height;
      break;
    default:
      break;
    }
    return 1.0;
  }
  // here goes glutMouseFunc, glutMotionFunc, glutPassiveMotionFunc
  void
  FourTiledWindow::mouse(int button, int state, int x, int y){
    m_activeWindow = findActive(x, y);
    m_button = (button < 3 && button > -1) ? button : 0;
    m_pressed = state;
#if 0
    std::cerr << "button " << button << std::endl;
    std::cerr << "state " << state << std::endl;
#endif
  }

  void
  FourTiledWindow::motion(int x, int y){
    if(m_button < 3)
    m_buttonSpeeds[m_button] = gloost::vec2(x - m_mouseX, y - m_mouseY);

    m_activeWindow = findActive(x, y);
  }
  

  void
  FourTiledWindow::passive(int x, int y){
    m_activeWindow = findActive(x, y);
  }


  void
  FourTiledWindow::specialKey(int key, int x, int y){

    unsigned middleMouse = 1;

    switch(key){

    case GLUT_KEY_LEFT:
      m_buttonSpeeds[middleMouse] = gloost::vec2(-1.0, 0.0);
      break;

    case GLUT_KEY_RIGHT:
      m_buttonSpeeds[middleMouse] = gloost::vec2(1.0, 0.0);
      break;

    case GLUT_KEY_DOWN:
      m_buttonSpeeds[middleMouse] = gloost::vec2(0.0, -1.0);
      break;

    case GLUT_KEY_UP:
      m_buttonSpeeds[middleMouse] = gloost::vec2(0.0, 1.0);
      break;

    default:
      break;
    }

    m_activeWindow = findActive(x, y);
  }

  unsigned
  FourTiledWindow::getActiveWindow(){
    return m_activeWindow;
  }


  gloost::vec2
  FourTiledWindow::getRelativeLocation(unsigned windowNum){

    switch (windowNum){
    case 0:
      return gloost::vec2(m_mouseX,m_mouseY);
      break;

    case 1:
      return gloost::vec2(m_mouseX - 0.5 * m_width,m_mouseY);
      break;

    case 2:
      return gloost::vec2(m_mouseX,m_mouseY - 0.5 * m_height);
      break;

    case 3:
      return gloost::vec2(m_mouseX - 0.5 * m_width, m_mouseY - 0.5 * m_height);
      break;


    default:
      break;
    }
    return gloost::vec2(m_mouseX,m_mouseY);

  }

  void
  FourTiledWindow::endFrame(){
    m_buttonSpeeds[0] = gloost::vec2(0.0,0.0);
    m_buttonSpeeds[1] = gloost::vec2(0.0,0.0);
    m_buttonSpeeds[2] = gloost::vec2(0.0,0.0);
  }


  gloost::vec2
  FourTiledWindow::getButtonSpeed(unsigned buttonNum){
    if(buttonNum < 3){
      return m_buttonSpeeds[buttonNum];
    }
    return gloost::vec2(0.0,0.0);
  }

  void
  FourTiledWindow::draw(){

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glViewport(0.0,0.0,m_width, m_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0,1.0,0.0,1.0,1.0,-1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glDisable(GL_DEPTH_TEST);
    glColor4f(1.0,1.0,1.0,1.0);
    glBegin(GL_LINES);
    glVertex2f(0.5,0.0);
    glVertex2f(0.5,1.0);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(0.0,0.5);
    glVertex2f(1.0,0.5);
    glEnd();

    glPopAttrib();

  }

  void
  FourTiledWindow::drawCursor(){

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glViewport(0.0,0.0,m_width, m_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0,m_width,0.0,m_height,1.0,-1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glDisable(GL_DEPTH_TEST);
    glColor4f(1.0,1.0,1.0,1.0);
    glTranslatef(m_mouseX,m_height - m_mouseY,0.0);
    const float half_sizeX(10.0);
    const float half_sizeY(10.0);
    glBegin(GL_LINES);
    glVertex2f(-half_sizeX,0.0);
    glVertex2f(half_sizeX,0.0);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(0.0,-half_sizeY);
    glVertex2f(0.0,half_sizeY);
    glEnd();

    glPopAttrib();

  }

  unsigned
  FourTiledWindow::getWidth(){
    return 0.5 * m_width;
  }
   
  unsigned
  FourTiledWindow::getHeight(){
    return 0.5 * m_height;
  }



  unsigned
  FourTiledWindow::findActive(int x, int y){

    m_mouseX = x;
    m_mouseY = y;

    if(x < 0.5 * m_width){
      if(y < 0.5 * m_height)
	return 0;
      else
	return 2;
    }
    else{
      if(y < 0.5 * m_height)
	return 1;
      else
	return 3;
    }
  }




}
