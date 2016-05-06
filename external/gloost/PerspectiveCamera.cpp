
/*
                       ___                            __
                      /\_ \                          /\ \__
                   __ \//\ \     ___     ___     ____\ \  _\
                 /'_ `\ \ \ \   / __`\  / __`\  /  __\\ \ \/
                /\ \ \ \ \_\ \_/\ \ \ \/\ \ \ \/\__   \\ \ \_
                \ \____ \/\____\ \____/\ \____/\/\____/ \ \__\
                 \/___/\ \/____/\/___/  \/___/  \/___/   \/__/
                   /\____/
                   \_/__/

                   OpenGL framework for fast demo programming

                             http://www.gloost.org

    This file is part of the gloost framework. You can use it in parts or as
       whole under the terms of the GPL (http://www.gnu.org/licenses/#GPL).

            gloost is being created by Felix Weiﬂig and Stephan Beck

     Felix Weiﬂig (thesleeper@gmx.net), Stephan Beck (stephan@pixelstars.de)
*/



/// gloost includes
#include <gloostConfig.h>
#include <PerspectiveCamera.h>



/// cpp includes
#include <GL/glew.h>
#include <GL/glut.h>



namespace gloost
{


///////////////////////////////////////////////////////////////////////////////


/// class constructor
PerspectiveCamera::PerspectiveCamera():
    Camera()
{

}


///////////////////////////////////////////////////////////////////////////////


  ///

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far)
  : Camera(fov, aspect, near, far)
{

}


///////////////////////////////////////////////////////////////////////////////


  ///

PerspectiveCamera::~PerspectiveCamera()
{

}


///////////////////////////////////////////////////////////////////////////////


  ///

void
PerspectiveCamera::setFov(float fov)
{
  _fov = fov;
  _projection_dirty = true;
}

///////////////////////////////////////////////////////////////////////////////


  ///
float
PerspectiveCamera::getFov() const
{
  return _fov;
}

///////////////////////////////////////////////////////////////////////////////


  ///

void
PerspectiveCamera::setAspect(float aspect)
{
  _aspect = aspect;
  _projection_dirty = true;
}

///////////////////////////////////////////////////////////////////////////////


  ///
float
PerspectiveCamera::getAspect() const
{
  return _aspect;
}


///////////////////////////////////////////////////////////////////////////////


  ///

void PerspectiveCamera::setNear(float near)
{
  _near = near;
  _projection_dirty = true;
}

///////////////////////////////////////////////////////////////////////////////


  ///
float
PerspectiveCamera::getNear() const
{
  return _near;
}

///////////////////////////////////////////////////////////////////////////////


  ///

void PerspectiveCamera::setFar(float far)
{
  _far = far;
  _projection_dirty = true;
}

///////////////////////////////////////////////////////////////////////////////


  ///
float
PerspectiveCamera::getFar() const
{
  return _far;
}


///////////////////////////////////////////////////////////////////////////////


  ///


/* virtual */
void PerspectiveCamera::set()
{

  if(_projection_dirty)
  {
    updateProjection();
    updateFrustum();
  }

  Camera::set();
}


///////////////////////////////////////////////////////////////////////////////


  /// set poisition, point to look at and down vector of the camera

void
PerspectiveCamera::lookAt(Point3 eye, Point3 coi, Vector3 up)
{

  _cameraPosition = eye;

  glMatrixMode(GL_MODELVIEW);

  glPushMatrix();
  {
    glLoadIdentity();
    gluLookAt((GLfloat) eye[0],(GLfloat) eye[1],(GLfloat) eye[2],
              (GLfloat) coi[0],(GLfloat) coi[1],(GLfloat) coi[2],
              (GLfloat) up[0], (GLfloat) up[1], (GLfloat) up[2]);
    gloostGetv (GL_MODELVIEW_MATRIX, _modelview.data());
  }
  glPopMatrix();


  updateFrustum();

}


///////////////////////////////////////////////////////////////////////////////


  /// returns position of the camera !!! Works only if position was set via lookAt(Point3 ...)

/* virtual */
const Point3&
PerspectiveCamera::getPosition()
{
  return _cameraPosition;
}


///////////////////////////////////////////////////////////////////////////////


  ///

/*virtual*/
Ray
PerspectiveCamera::getPickRay(unsigned int screenWidth,
                              unsigned int screenHeight,
                              unsigned int mouseH,
                              unsigned int mouseV)
{

  if(_projection_dirty)
  {
    updateProjection();
    updateFrustum();
  }


  Vector3 frustumH_vec         = _frustum.far_lower_right - _frustum.far_lower_left;
  Vector3 FrustumOnePixelWidth = frustumH_vec/screenWidth;

  Vector3 frustumV_vec          = _frustum.far_upper_left - _frustum.far_lower_left;
  Vector3 FrustumOnePixelHeight = frustumV_vec/screenHeight;


  Point3 pickPointOnFarPlane = _frustum.far_lower_left
                                + (FrustumOnePixelWidth * mouseH)
                                + (FrustumOnePixelHeight * mouseV);


  Matrix modelviewInv = _modelview;
  modelviewInv.invert();

  Point3 camPos = modelviewInv * Point3(0.0, 0.0, 0.0);
  Vector3 pickDir = (pickPointOnFarPlane - camPos).normalized();

  return Ray(camPos, pickDir);
}


///////////////////////////////////////////////////////////////////////////////


  /// recalc the projection matrix

/*virtual*/
void
PerspectiveCamera::updateProjection()
{
  glMatrixMode(GL_PROJECTION);

  glPushMatrix();
  {
    glLoadIdentity();
    gluPerspective (_fov, _aspect, _near, _far);
    gloostGetv (GL_PROJECTION_MATRIX, _projection.data());
  }
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);

  _projection_dirty = false;
}

///////////////////////////////////////////////////////////////////////////////


  ///

/*virtual*/
void
PerspectiveCamera::updateFrustum()
{
  const float scale = tan(M_PI * _fov/360.0f);
  const float near_top = _near * scale;
  const float near_left  = near_top * _aspect;
  const float far_top = _far * scale;
  const float far_left  = far_top * _aspect;
  Matrix _modelview_inv(_modelview);
  _modelview_inv.invert();
  _frustum.near_lower_left  = _modelview_inv * Point3(-near_left,-near_top,-_near);
  _frustum.near_lower_right = _modelview_inv * Point3(near_left,-near_top,-_near);
  _frustum.near_upper_right = _modelview_inv * Point3(near_left,near_top,-_near);
  _frustum.near_upper_left  = _modelview_inv * Point3(-near_left,near_top,-_near);

  _frustum.far_lower_left  = _modelview_inv * Point3(-far_left,-far_top,-_far);
  _frustum.far_lower_right = _modelview_inv * Point3(far_left,-far_top,-_far);
  _frustum.far_upper_right = _modelview_inv * Point3(far_left,far_top,-_far);
  _frustum.far_upper_left  = _modelview_inv * Point3(-far_left,far_top,-_far);

  _frustum.recalcPlanes();
}

///////////////////////////////////////////////////////////////////////////////

} // namespace gloost
