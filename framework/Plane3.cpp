
#include "Plane3.h"

#include <glbinding/gl/gl.h>
using namespace gl;

#include <Matrix.h>
#include <gloostMath.h>
#include <iostream>

#include <cmath>


namespace kinect{

  float
  angleDeg(const gloost::Vector3& a, const gloost::Vector3& b){
    float rad = std::acos(a * b);
    return 180.0 * rad / M_PI;
  }


  Plane3::Plane3(const float nx, const float ny, const float nz, const float d)
    : _n(nx,ny,nz),
      _d(d),
      _o(),
      _a(1.0,1.0,1.0),
      _b()
  {
    //std::cerr << "normal: " << _n << std::endl;
    _n.normalize();
    //std::cerr << "normal: " << _n << std::endl;


    // compute parametric equation

    // origin
    _o = gloost::Point3(0.0,0.0,0.0) + (-_d) * _n;

    //std::cerr << "origin: " << _o << std::endl;

    // _a

    unsigned maxcoord = 0;
    if(std::abs(_n[1]) > std::abs(_n[0]))
      maxcoord = 1;
    if(maxcoord == 1){
      if(std::abs(_n[2]) > std::abs(_n[1]))
	maxcoord = 2;
    }
    else{
      if(std::abs(_n[2]) > std::abs(_n[0]))
	maxcoord = 2;
    }
    //std::cerr << "maxcoord: " << maxcoord << std::endl;

    switch (maxcoord){

    case 0:
      _a[0] = (-_n[1] -_n[2])/_n[0];
      break;
    case 1:
      _a[1] = (-_n[0] -_n[2])/_n[1];
      break;
    case 2:
      _a[2] = (-_n[0] -_n[1])/_n[2];
      break;
    }
    _a.normalize();
    _b = gloost::cross(_n,_a);
    _b.normalize();

    //std::cerr << "_o: " << _o << std::endl;
    //std::cerr << "_a: " << _a << std::endl;
    //std::cerr << "_b: " << _b << std::endl;
    //std::cerr << "_a * _n = " << _a * _n << std::endl;
    //std::cerr << "_b * _n = " << _b * _n << std::endl;

    // test if a point generated is on the plane
    // gloost::Point3 p = _o + ((50 * gloost::crand()) * _a) + ((50 * gloost::crand()) * _b);
    //std::cerr << "distance from " << p << " to plane is " << (gloost::Vector3(p) * _n) + _d  << std::endl;
  }
      
  Plane3::~Plane3()
  {}

  float
  Plane3::distance(const gloost::Point3& p){
    return _n[0] * p[0] + _n[1] * p[1] + _n[2]* p[2] + _d;
  }

  void
  Plane3::draw(gloost::Point3& near, const float size){

    //std::cerr << intersect(near, _n) << std::endl;

    gloost::Point3 o = intersect(near, _n);

    const float size_half = 0.5 * size;
    gloost::Point3 a(o - size_half * _a - size_half * _b );
    gloost::Point3 b(o + size_half * _a - size_half * _b );
    gloost::Point3 c(o + size_half * _a + size_half * _b );
    gloost::Point3 d(o - size_half * _a + size_half * _b );


    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glBegin(GL_QUADS);
    glVertex3fv(a.data());
    glVertex3fv(b.data());
    glVertex3fv(c.data());
    glVertex3fv(d.data());
    glEnd();

    glPopAttrib();


  }


  void
  Plane3::drawNormal(float length){

    glPushAttrib(GL_ALL_ATTRIB_BITS);


    gloost::Point3 origin(0.0,0.0,0.0);
    gloost::Point3 end = origin + length * _n;
    if(_n[2] < 0.0)
      end = -1.0 * end;
    
    glBegin(GL_LINES);
    glVertex3fv(origin.data());
    glVertex3fv(end.data());
    glEnd();

    glPopAttrib();

  }


  gloost::Point3
  Plane3::intersect(const gloost::Point3& o, const gloost::Vector3& d) const{

    using namespace gloost;
    // Returns in (fX, fY) the location on the plane (P1,P2,P3) of the intersection with the ray (R1, R2)
    // First compute the axes
    Vector3 V1 = _a; //P2 - P1;
    Vector3 V2 = _b; //P3 - P1;
    Vector3 V3 = cross(V1,V2);//CrossProduct ( V1, V2);

    Point3 R1 = o;
    Point3 P1 = _o;
    Point3 R2 = o + 1.0f * d;   

    // Project ray points R1 and R2 onto the axes of the plane. (This is equivalent to a rotation.)
    // vRotRay1 = CVector3 ( Dot (V1, R1-P1 ), Dot (V2, R1-P1 ), Dot (V3, R1-P1 ) );
    Vector3 vRotRay1(  V1 * (R1-P1), V2 * (R1-P1), V3 * (R1-P1) );
    // vRotRay2 = CVector3 ( Dot (V1, R2-P1 ), Dot (V2, R2-P1 ), Dot (V3, R2-P1 ) ); 
    Vector3 vRotRay2(  V1 * (R2-P1), V2 * (R2-P1), V3 * (R2-P1) );
 
    // Return now if ray will never intersect plane (they're parallel)
    if (vRotRay1[2] == vRotRay2[2]) return gloost::Point3(0.0f,0.0f,0.0f);
   
    // Find 2D plane coordinates (fX, fY) that the ray interesects with
    float fPercent = vRotRay1[2] / (vRotRay2[2] - vRotRay1[2]);
    
    return R1 + (R1 - R2) * fPercent;


  }


  std::ostream& operator << (std::ostream& o, const Plane3& p){
    o << "Plane3: " << p._o << " -> " << p._a << " -> " << p._b;
    return o;
  }


  void
  CoordinateSystem::findFrom(const Plane3& a, const  Plane3& b, const  Plane3& c){


    using namespace gloost;

    // intersect a and b -> ab1 and ab2
    Point3 ab1 = b.intersect(a._o, a._a);
    Point3 ab2 = b.intersect(a._o, a._b);

    _ex = ab2 - ab1;
    _ex.normalize();
    if(_ex[2] < 0)
      _ex = - 1.0 * _ex;

    _o = c.intersect(ab1, _ex);

    // intersect b and c -> bc1 and bc2
    Point3 bc1 = c.intersect(b._o, b._a);
    Point3 bc2 = c.intersect(b._o, b._b);

    _ey = bc2 - bc1;
    _ey.normalize();
    if(_ey[2] < 0)
      _ey = - 1.0 * _ey;

    // intersect c and a -> ca1 and ca2
    Point3 ca1 = a.intersect(c._o, c._a);
    Point3 ca2 = a.intersect(c._o, c._b);

    _ez = ca2 - ca1;
    _ez.normalize();
    if(_ez[2] < 0)
      _ez = - 1.0 * _ez;

  }

  void
  CoordinateSystem::findFrom2(const Plane3& a, const  Plane3& b, const  Plane3& c){


    using namespace gloost;

    // intersect a and b -> ab1 and ab2
    Point3 ab1 = b.intersect(a._o, a._a);
    Point3 ab2 = b.intersect(a._o, a._b);

    _ex = ab2 - ab1;
    _ex.normalize();


    _o = c.intersect(ab1, _ex);

    // intersect b and c -> bc1 and bc2
    Point3 bc1 = c.intersect(b._o, b._a);
    Point3 bc2 = c.intersect(b._o, b._b);

    _ey = bc2 - bc1;
    _ey.normalize();

    _ez = cross(_ex, _ey);
    _ez.normalize();

    _ey = cross(_ez, _ex);
    _ey.normalize();

  }

  void
  CoordinateSystem::flipX(){
    _ex = -1.0 * _ex;
  }

  void
  CoordinateSystem::flipY(){
    _ey = -1.0 * _ey;
  }

  void
  CoordinateSystem::flipZ(){
    _ez = -1.0 * _ez;
  }

  void
  CoordinateSystem::draw(bool color){

    const float length = color ? 1.3 : 1.0;

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    if(color)
      glColor3f(0.5,0.5,0.5);
    glPointSize(2.5);
    glBegin(GL_POINTS);
    glVertex3fv(_o.data());
    glEnd();

    if(color)
      glColor3f(1.0,0.0,0.0);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex3fv(_o.data());
    gloost::Point3 ex(_o + length * _ex);
    glVertex3fv(ex.data());
    glEnd();

    if(color)
      glColor3f(0.0,1.0,0.0);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex3fv(_o.data());
    gloost::Point3 ey(_o + length * _ey);
    glVertex3fv(ey.data());
    glEnd();

    if(color)
      glColor3f(0.0,0.0,1.0);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex3fv(_o.data());
    gloost::Point3 ez(_o + length * _ez);
    glVertex3fv(ez.data());
    glEnd();


    if(color)
      glColor3f(1.0,1.0,1.0);
    glPointSize(6.0);
    glBegin(GL_POINTS);
    glVertex3fv(_o.data());
    glEnd();



    glPopAttrib();
  }

  float
  CoordinateSystem::calcDegError(){
    return std::abs(90.0 - angleDeg(_ex, _ey)) +
      std::abs(90.0 - angleDeg(_ex, _ez)) +
      std::abs(90.0 - angleDeg(_ey, _ez));
      
  }

}
