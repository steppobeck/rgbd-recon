
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
#include <Camera.h>



/// cpp includes
#include <GL/glew.h>
#include <GL/glut.h>



namespace gloost
{


///////////////////////////////////////////////////////////////////////////////


  /// class constructor

Camera::Camera():
    _cameraPosition(Point3(0.0,0.0,0.0)),
    _fov(45),
    _aspect(1.6),
    _near(0.1),
    _far(100.0),
    _projection_dirty(true),
    _projection(),
    _modelview(),
    _frustum()
{

}


///////////////////////////////////////////////////////////////////////////////


  /// class constructor

Camera::Camera(const Matrix& projectionMatrix, const Matrix& modelViewMatrix):
    _cameraPosition(Point3(0.0,0.0,0.0)),
    _fov(45.0),
    _aspect(0.0),
    _near(0.0),
    _far(0.0),
    _projection_dirty(true),
    _projection(projectionMatrix),
    _modelview(modelViewMatrix),
    _frustum()
{

}


///////////////////////////////////////////////////////////////////////////////


  ///

Camera::Camera(float fov, float aspect, float near, float far):
    _cameraPosition(Point3(0.0,0.0,0.0)),
    _fov(fov),
    _aspect(aspect),
    _near(near),
    _far(far),
    _projection_dirty(true),
    _projection(),
    _modelview(),
    _frustum()
{

}


///////////////////////////////////////////////////////////////////////////////


  ///

/* virtual */
Camera::~Camera()
{

}


///////////////////////////////////////////////////////////////////////////////


  ///

void Camera::setProjectionMatrix(const Matrix& projectionMatrix)
{
  _projection = projectionMatrix;
}


///////////////////////////////////////////////////////////////////////////////


  ///

void Camera::setModelViewMatrix(const Matrix& modelViewMatrix)
{
  _modelview = modelViewMatrix;
}


///////////////////////////////////////////////////////////////////////////////


  ///

/* virtual */
void Camera::set()
{
  glMatrixMode(GL_PROJECTION);
  gloostLoadMatrix(_projection.data());

  glMatrixMode(GL_MODELVIEW);
  gloostLoadMatrix(_modelview.data());
}

///////////////////////////////////////////////////////////////////////////////


  ///

const Frustum&
Camera::getFrustum() const{
  return _frustum;
}


///////////////////////////////////////////////////////////////////////////////


  ///

const Matrix&
Camera::getProjectionMatrix() const
{
  return _projection;
}


///////////////////////////////////////////////////////////////////////////////


  ///

const Matrix&
Camera::getModelViewMatrix() const
{
  return _modelview;
}


///////////////////////////////////////////////////////////////////////////////


/// returns position of the camera !!! Works only if position was set via lookAt(Point3 ...)

/*virtual*/
const Point3&
Camera::getPosition()
{
  return _cameraPosition;
}

///////////////////////////////////////////////////////////////////////////////

Matrix
Camera::getImageToEye(){
  Matrix viewport_translate;
  viewport_translate.setIdentity();
  viewport_translate.setTranslate(1.0f,1.0f,1.0f);
  
  Matrix viewport_scale;
  viewport_scale.setIdentity();
  viewport_scale.setScale(0.5f, 0.5f, 0.5f);

  Matrix image2eye = viewport_scale * viewport_translate * _projection;
  image2eye.invert();

  return image2eye;
}


float
Camera::getNear(){
  return _near;
}

float
Camera::getFar(){
  return _far;
}


} // namespace gloost
