#ifndef PMD_CAMERANAVIGATOR_H
#define PMD_CAMERANAVIGATOR_H

#include <stdlib.h>
#include <math.h>

#include <arcball.hpp>

#include <Point3.h>
#include <Vector3.h>
#include <Vector2.h>
#include <Matrix.h>

namespace pmd{

class CameraNavigator{

public:
  CameraNavigator(float z = 0.5f);
  void setZoom(float z);
  void mouse(int button, int state, int mouse_h, int mouse_v);
  void motion(int mouse_h, int mouse_v);
  void resize(unsigned w, unsigned h);

  // virtual void special(int key){};
  // virtual void keyboard(unsigned char key){};

  virtual gloost::Matrix get( gloost::Point3& speed);

  void reset();

  virtual gloost::Matrix getRot();
  virtual gloost::Point3 getOrigin();

  gloost::Vector2 const& getOffset(unsigned index) const;
  void resetOffsets();

protected:
  gl::ArcBall m_arcball;
  gloost::Point3 m_poi;
  gloost::Vector3 m_x;
  gloost::Vector3 m_y;
  gloost::Vector3 m_z;
  gloost::Vector2 button_offsets[2] = {gloost::Vector2{0.0f, 0.0f}, gloost::Vector2{0.0f, 0.0f}}; 
  gloost::Vector2 start_pos; 
  int curr_button = -1;
  float m_zoom;
  float m_zoom_reset;
};

}

#endif // #ifndef  PMD_CAMERANAVIGATOR_H