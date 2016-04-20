#ifndef PLANE3_H
#define PLANE3_H


#include <Point3.h>
#include <Vector3.h>


namespace kinect{


  float angleDeg(const gloost::Vector3& a, const gloost::Vector3& b);

  class Plane3{

  public:
    Plane3(const float nx, const float ny, const float nz, const float d);
    ~Plane3();


    float distance(const gloost::Point3& p);

    void draw(gloost::Point3& near, const float size = 0.25);

    void drawNormal(float length);

    gloost::Point3 intersect(const gloost::Point3& o, const gloost::Vector3& d) const;


    gloost::Vector3 _n;
    float           _d;

    gloost::Point3  _o;
    gloost::Vector3 _a;
    gloost::Vector3 _b;


  };

  std::ostream& operator << (std::ostream& o, const Plane3& p);

  class CoordinateSystem{

  public:

    void findFrom(const Plane3& a, const  Plane3& b, const  Plane3& c);
    void findFrom2(const Plane3& a, const  Plane3& b, const  Plane3& c);

    void draw(bool color = true);

    void flipX();
    void flipY();
    void flipZ();


    gloost::Vector3 _ex;
    gloost::Vector3 _ey;
    gloost::Vector3 _ez;

    gloost::Point3  _o;

    float calcDegError();

  };


}



#endif // #ifndef  PLANE3_H
