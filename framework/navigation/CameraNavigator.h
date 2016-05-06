#ifndef PMD_CAMERANAVIGATOR_H
#define PMD_CAMERANAVIGATOR_H

// #include <glbinding/gl/gl.h>
// using namespace gl;

#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>

#include <arcball.hpp>

#include <Point3.h>
#include <Vector3.h>
#include <Matrix.h>

namespace pmd{

class CameraNavigator{

public:
  CameraNavigator(float z = 0.5f);
  virtual ~CameraNavigator();

  void mouse(int button, int state, int mouse_h, int mouse_v);
  void motion(int mouse_h, int mouse_v);
  void resize(GLsizei w, GLsizei h);

  // virtual void special(int key){};
  // virtual void keyboard(unsigned char key){};

  virtual gloost::Matrix get( gloost::Point3& speed);

  void reset();

  virtual gloost::Matrix getRot();
  virtual gloost::Point3 getOrigin();

protected:
  gl::ArcBall m_arcball;
  gloost::Point3 m_poi;
  gloost::Vector3 m_x;
  gloost::Vector3 m_y;
  gloost::Vector3 m_z;
  float m_zoom;
  float m_zoom_reset;
};

}

#endif // #ifndef  PMD_CAMERANAVIGATOR_H