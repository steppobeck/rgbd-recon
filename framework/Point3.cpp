
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



// gloost includes
#include <Point3.h>
#include <Vector3.h>



// cpp includes
#include <cstring>



namespace gloost
{


/*static*/ Point3 Point3::origin = Point3(0.0,0.0,0.0);




////////////////////////////////////////////////////////////////////////////////


 /// ...

Point3::Point3():
  _data()
{
    _data[0]=0.0;
    _data[1]=0.0;
    _data[2]=0.0;
    _data[3]=1.0;
}


////////////////////////////////////////////////////////////////////////////////


 /// ...

Point3::Point3 (const Vector3& v):
  _data ()
{
  memcpy(_data, v.data(), sizeof(mathType)*3);
  _data[3] = 1.0;
}


////////////////////////////////////////////////////////////////////////////////


 /// ...

Point3::Point3 (const Point3& p)
  : _data ()
{
  memcpy(_data, p._data, sizeof(mathType)*4);
}


////////////////////////////////////////////////////////////////////////////////


 /// ...

Point3::Point3(const mathType a, const mathType b, const mathType c)
    : _data()
{
    _data[0] = a;
    _data[1] = b;
    _data[2] = c;
    _data[3] = 1.0;
}


////////////////////////////////////////////////////////////////////////////////


 /// ...

Point3::Point3(std::ifstream& in)
    : _data()
{
  in >> _data[0];
  in >> _data[1];
  in >> _data[2];
  _data[3] = 1.0;
}


///////////////////////////////////////////////////////////////////////////////


  /// ...

mathType&
Point3::operator[](unsigned int a)
{
  return (3 < a) ? _data[0] : _data[a];
}


////////////////////////////////////////////////////////////////////////////////


  /// ...

const mathType&
Point3::operator[] (unsigned int a) const
{
  return (3 < a) ? _data[0] : _data[a];
}


////////////////////////////////////////////////////////////////////////////////


  /// ...

const Point3&
Point3::operator= (const Point3& a)
{
  memcpy(_data, a._data, sizeof(mathType)*4);
  return *this;
}


////////////////////////////////////////////////////////////////////////////////


  /// length of the vector from the origin to the point

mathType
Point3::distanceToOrigin() const
{
  return (sqrt ((_data[0]*_data[0])+
                (_data[1]*_data[1])+
                (_data[2]*_data[2])+
                (_data[3]*_data[3])));
}


////////////////////////////////////////////////////////////////////////////////


  /// quadratic length of the vector from the origin to the point

mathType
Point3::distanceToOrigin2() const
{
  return ((_data[0]*_data[0])+
          (_data[1]*_data[1])+
          (_data[2]*_data[2])+
          (_data[3]*_data[3]));
}


////////////////////////////////////////////////////////////////////////////////


 /// get pointer to an array

const mathType*
Point3::data() const
{
  return _data;
}


////////////////////////////////////////////////////////////////////////////////


 /// get pointer to an array

mathType*
Point3::data()
{
  return _data;
}


////////////////////////////////////////////////////////////////////////////////


  /// operator for notation Point3*mathType

/*extern*/
Point3
operator* (const Point3& p, const mathType& s)
{
  return Point3(p[0]*s, p[1]*s, p[2]*s);
}

////////////////////////////////////////////////////////////////////////////////


  /// operator for notation mathType*Point3

/*extern*/
Point3
operator* (const mathType& s, const Point3& p)
{
  return Point3(p[0]*s, p[1]*s, p[2]*s);
}


////////////////////////////////////////////////////////////////////////////////


/// Point3 + Vector3

/*extern*/
Point3 operator+(const Point3& a, const Vector3& b)
{
  return Point3(a[0] + b[0],
                a[1] + b[1],
                a[2] + b[2]);
}


////////////////////////////////////////////////////////////////////////////////


  /// operator for notation Point3+=Vector3

void
Point3::operator+=(const Vector3& b)
{
  _data[0] += b[0];
  _data[1] += b[1];
  _data[2] += b[2];
}


////////////////////////////////////////////////////////////////////////////////


 /// Point3 - Vector3

/*extern*/
Point3 operator-(const Point3& a, const Vector3& b)
{
  return Point3(a[0] - b[0],
                a[1] - b[1],
                a[2] - b[2]);
}


////////////////////////////////////////////////////////////////////////////////


  /// operator for notation Point3-=Vector3

void
Point3::operator-=(const Vector3& b)
{
  _data[0] -= b[0];
  _data[1] -= b[1];
  _data[2] -= b[2];
}


////////////////////////////////////////////////////////////////////////////////


  /// equal

/*extern*/
bool operator== (const Point3& a, const Point3& b)
{
  if (   std::abs(a[0] - b[0]) < GLOOST_MATH_TYPE_EPSYLON
      && std::abs(a[1] - b[1]) < GLOOST_MATH_TYPE_EPSYLON
      && std::abs(a[2] - b[2]) < GLOOST_MATH_TYPE_EPSYLON  )
  {
    return true;
  }

  return false;
}


////////////////////////////////////////////////////////////////////////////////


  /// not equal

/*extern*/
bool operator!= (const Point3& a, const Point3& b)
{
  if (   std::abs(a[0] - b[0]) < GLOOST_MATH_TYPE_EPSYLON
      && std::abs(a[1] - b[1]) < GLOOST_MATH_TYPE_EPSYLON
      && std::abs(a[2] - b[2]) < GLOOST_MATH_TYPE_EPSYLON  )
  {
    return false;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


  ///

/* extern */
std::ostream&
operator<< (std::ostream& os, const Point3& a)
{
  os << "Point3( " << std::fixed << a[0] << "," << a[1] << "," << a[2] << " )";
  return os;
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
} // namespace gloost
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

