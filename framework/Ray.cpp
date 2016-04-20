
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
#include <Point3.h>
#include <Vector3.h>
#include <Ray.h>



/// cpp includes
#include <math.h>
#include <limits.h>



namespace gloost
{

///////////////////////////////////////////////////////////////////////////////


 /// class constructor

Ray::Ray():
  _origin(0,0,0),
  _direction(0,0,-1),
  _t(T_MAX)
{
	// insert your code here
}


///////////////////////////////////////////////////////////////////////////////


 /// class constructor

Ray::Ray(const Point3& origin, const Vector3& direction):
  _origin(origin),
  _direction(direction),
  _t(T_MAX)
{
	// insert your code here
}


///////////////////////////////////////////////////////////////////////////////


 /// class constructor

Ray::Ray(const Point3& origin, const Vector3& direction, mathType t):
  _origin(origin),
  _direction(direction),
  _t(t)
{
	// insert your code here
}


///////////////////////////////////////////////////////////////////////////////


 /// class destructor

Ray::~Ray()
{
	// insert your code here
}


////////////////////////////////////////////////////////////////////////////////


 /// copy

const Ray&
Ray::operator= (const Ray& a)
{
  if (this != &a)
  {
    _origin    = a._origin;
    _direction = a._direction;
    _t         = a._t;
  }
  return *this;
}


///////////////////////////////////////////////////////////////////////////////


 ///

///
void
Ray::setOrigin(const Point3& origin)
{
	_origin = origin;
}


///////////////////////////////////////////////////////////////////////////////


 ///

const Point3&
Ray::getOrigin() const
{
	return _origin;
}


///////////////////////////////////////////////////////////////////////////////


 ///

void
Ray::setDirection(const Vector3& direction)
{
  _direction = direction;
}


///////////////////////////////////////////////////////////////////////////////


 ///

const Vector3&
Ray::getDirection() const
{
  return _direction;
}


///////////////////////////////////////////////////////////////////////////////


 ///

void
Ray::setT(mathType t)
{
	_t = t;
}


///////////////////////////////////////////////////////////////////////////////


 ///

void
Ray::setTMax()
{
	_t = T_MAX;
}


///////////////////////////////////////////////////////////////////////////////


 ///

const mathType&
Ray::getT() const
{
	return _t;
}



///////////////////////////////////////////////////////////////////////////////


 ///

mathType&
Ray::getT()
{
	return _t;
}


///////////////////////////////////////////////////////////////////////////////


  /// get the point origin*(direction*t)

Point3
Ray::getDestination() const
{
  return _origin + _t*_direction;
}


///////////////////////////////////////////////////////////////////////////////


  /// normalize length of direction

void
Ray::normalize()
{
	_direction.normalize();
}


///////////////////////////////////////////////////////////////////////////////


  /// returns a normalized version of the ray

Ray
Ray::normalized()
{
	Ray normRay = (*this);
	normRay.normalize();
	return normRay;
}


////////////////////////////////////////////////////////////////////////////////


  ///

/* extern */
std::ostream&
operator<< (std::ostream& os, const Ray& r)
{
  os << std::endl << "ray{";
  os << std::endl <<  "  origin: " << r.getOrigin();
  os << std::endl <<  "  direct: " << r.getDirection();
  os << std::endl <<  "  t:      " << r.getT();
  os << std::endl <<  "}" << std::endl;
  return os;
}


////////////////////////////////////////////////////////////////////////////////



/*static*/ mathType Ray::T_MAX = GLOOST_MATH_TYPE_MAX_VALUE;



} // namespace gloost
