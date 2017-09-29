
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



#ifndef GLOOST_SCREENCAMERA_H
#define GLOOST_SCREENCAMERA_H



/// gloost includes
#include <Camera.h>



namespace gloost
{


  ///  ScreenCamera is basically a wrapper for glFrustum.
  //   It allows to configure your projection setup in a geometrically correct way.
  //   The cyclops (eye) matrix is a matrix which translates the eye from the origin of your world
  //   The near and far cliping parameters define the near and far clipping planes respectively
  //   The screen matrix, screenwidth and screenheight allow you to define
  //       the position, orientation and dimensions of your physical display.
  class ScreenCamera : public Camera
  {

  public:


    /// class constructors

    ScreenCamera(const Matrix& cyclopsmatrix,
                 float near,
                 float far,
                 float screenwidth,
                 float screenheight);

    ScreenCamera(const Matrix& cyclopsmatrix,
                 float near,
                 float far,
                 const Matrix& screenmatrix,
                 float screenwidth,
                 float screenheight);

    /// class destructor
    ~ScreenCamera();

    void setCyclopsMatrix(const Matrix& cyclopsmatrix);

    /// set distance to the near plane
    void setNear(float near);

    /// set distance to the far plane
    void setFar(float far);

    void setScreenMatrix(const Matrix& screenmatrix);

    void setScreenWidth(float screenwidth);

    void setScreenHeight(float screenheight);

    /// set poisition, point to look at and down vector of the camera
    void lookAt(Point3 eye,
                Point3 coi,
                Vector3 up);

    /* virtual */ void set();

    float getLeft();
    float getRight();
    float getTop();
    float getBottom();


    /*virtual*/ const Point3& getPosition();

 private:

    /// recalc the projection matrix
  protected:
    virtual void update();
    virtual void updateFrustum();
    Matrix _cyclops;

    Matrix _eye;
    float _near;
    float _far;
    float _fov;
    Matrix _screen;
    float _screenw;
    float _screenh;


    bool _dirty;

    float _left;
    float _right;
    float _bottom;
    float _top;


};


} // namespace gloost


#endif // GLOOST_SCREENCAMERA_H
