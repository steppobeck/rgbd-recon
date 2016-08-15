#include "CameraNavigator.h"

#include <glbinding/gl/gl.h>
using namespace gl;

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <string.h> // memcpy

namespace pmd{


  CameraNavigator::CameraNavigator(float z)
    : m_poi(0.0,1.0,0.0),
      m_x(1.0,0.0,0.0),
      m_y(0.0,1.0,0.0),
      m_z(0.0,0.0,6.0),
      m_zoom(z),
      m_zoom_reset(z){}

  void
  CameraNavigator::mouse(int button, int state, int mouse_h, int mouse_v){
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      m_arcball.set_cur(mouse_h, mouse_v);
      if (state == GLFW_PRESS) {
         m_arcball.begin_drag();
      } 
      else if (state == GLFW_RELEASE) {
         m_arcball.end_drag();
      }
    }
    else if(button == GLFW_MOUSE_BUTTON_RIGHT) {
      if (state == GLFW_PRESS) {
         curr_button = 1;
         start_pos = {mouse_h, mouse_v};
      } 
      else if (state == GLFW_RELEASE) {
         button_offsets[curr_button] = gloost::Vector2{0.0f, 0.0f};
         curr_button = -1;
      }
    }
    else if(button == GLFW_MOUSE_BUTTON_MIDDLE) {
      if (state == GLFW_PRESS) {
	       curr_button = 0;
         start_pos = {mouse_h, mouse_v};
      } 
      else if (state == GLFW_RELEASE) {
         button_offsets[curr_button] = gloost::Vector2{0.0f, 0.0f};
	       curr_button = -1;
      }
    }
  }

  void
  CameraNavigator::motion(int mouse_h, int mouse_v){
    m_arcball.set_cur(mouse_h, mouse_v);
    if (curr_button >= 0) {
      button_offsets[curr_button] = gloost::Vector2{mouse_h, mouse_v} - start_pos;
      start_pos = gloost::Vector2{mouse_h, mouse_v};
    }
  }

  gloost::Vector2 const& CameraNavigator::getOffset(unsigned index) const {
    return button_offsets[index];
  }

  void CameraNavigator::resetOffsets() {
    button_offsets[0] = gloost::Vector2{0.0f, 0.0f}; 
    button_offsets[1] = gloost::Vector2{0.0f, 0.0f}; 
  }
  
  void
  CameraNavigator::resize(unsigned w, unsigned h){
    m_arcball.set_win_size(w, h);
  }


  gloost::Matrix
  CameraNavigator::get( gloost::Point3& speed){

    gloost::Matrix rot;
    
    memcpy(rot.data(),m_arcball.get(), 16 * sizeof(float) );
    rot.invert();
    
    gloost::Vector3 x(rot * m_x);
    gloost::Vector3 y(rot * m_y);
    gloost::Vector3 z(rot * m_z);

    m_poi = m_poi - x * speed[0] - y * speed[1];
    m_zoom -= speed[2];
    m_zoom = std::max(0.01f,m_zoom);

    gloost::Point3 o( m_zoom * z + m_poi);

    glPushMatrix();
    glLoadIdentity();
    gloost::Matrix m;
    gluLookAt(o[0],o[1],o[2],
	      m_poi[0],m_poi[1],m_poi[2],
	      y[0],y[1],y[2]);
    gloostGetv (GL_MODELVIEW_MATRIX, m.data());
    m.invert();
    glPopMatrix();
    
    return m;    
  }

  void
  CameraNavigator::reset(){
    m_poi = gloost::Point3(0.0,0.0,0.0);
    m_x   = gloost::Vector3(1.0,0.0,0.0);
    m_y   = gloost::Vector3(0.0,1.0,0.0);
    m_z   = gloost::Vector3(0.0,0.0,6.0);
    m_zoom = m_zoom_reset;
    m_arcball = gl::ArcBall();
  }

  gloost::Matrix
  CameraNavigator::getRot(){
    gloost::Matrix m;
    memcpy(m.data(),m_arcball.get(), 16 * sizeof(float) );
    m.invert();
    return m;
  }
  
  gloost::Point3
  CameraNavigator::getOrigin(){
    return m_poi;
  }

}
