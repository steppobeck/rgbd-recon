
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



/*
  This class was taken from the openGG scenegraph project of Felix Weiﬂig.
  Read the LICENSE.TXT for copyright questions.
*/



// gloost includes
#include <Vector3.h>
#include <Point3.h>

// cpp includes
#include <cstring>

namespace gloost
{
Vector3::operator glm::fvec3() const {
  return glm::fvec3{_data[0], _data[1], _data[2]};
}
////////////////////////////////////////////////////////////////////////////////


Vector3::Vector3():
  _data()
{
    _data[0]=0.0;
    _data[1]=0.0;
    _data[2]=0.0;
    _data[3]=0.0;
}


////////////////////////////////////////////////////////////////////////////////


Vector3::Vector3 (const Vector3& a):
  _data ()
{
  memcpy(_data, a._data, sizeof(mathType)*4);
}


////////////////////////////////////////////////////////////////////////////////


Vector3::Vector3 (const Point3& p):
  _data ()
{
  memcpy(_data, p.data(), sizeof(mathType)*3);
  _data[3] = 0.0;
}


////////////////////////////////////////////////////////////////////////////////


Vector3::Vector3(const mathType  a,const mathType b,const mathType c)
    : _data()
{
    _data[0]=a;
    _data[1]=b;
    _data[2]=c;
    _data[3]=0.0;
}

////////////////////////////////////////////////////////////////////////////////


  /// for use as quaternions, do not use for 3d coords

Vector3::Vector3(const mathType  a,const mathType b,const mathType c, const mathType w)
    : _data()
{
    _data[0]=a;
    _data[1]=b;
    _data[2]=c;
    _data[3]=w;
}

////////////////////////////////////////////////////////////////////////////////


Vector3::Vector3(std::ifstream& in)
    : _data()
{
  in >> _data[0];
  in >> _data[1];
  in >> _data[2];
  _data[3]=0.0;
}


////////////////////////////////////////////////////////////////////////////////


mathType
Vector3::length() const

{
  return (sqrt ((_data[0]*_data[0])+
                (_data[1]*_data[1])+
                (_data[2]*_data[2])+
                (_data[3]*_data[3])));
}


////////////////////////////////////////////////////////////////////////////////


mathType
Vector3::length2() const
{
  return ((_data[0]*_data[0])+
          (_data[1]*_data[1])+
          (_data[2]*_data[2])+
          (_data[3]*_data[3]));
}


///////////////////////////////////////////////////////////////////////////////


  /// contrains the vector to a particluar length but preserves the direction

void
Vector3::constrain (const mathType length)
{
  const mathType l(length2());

  if(l > length*length)
  {
    const mathType factor(length/sqrt(l));

    _data[0]=_data[0]*factor;
    _data[1]=_data[1]*factor;
    _data[2]=_data[2]*factor;
  }
}


////////////////////////////////////////////////////////////////////////////////


 /// get pointer to an array

const mathType*
Vector3::data() const
{
  return _data;
}


////////////////////////////////////////////////////////////////////////////////


 /// get pointer to an array

mathType*
Vector3::data()
{
  return _data;
}


////////////////////////////////////////////////////////////////////////////////


mathType&
Vector3::operator[](unsigned int a)
{
    return (4 < a) ? _data[0] : _data[a];
}


////////////////////////////////////////////////////////////////////////////////


const mathType&
Vector3::operator[] (unsigned int a) const
{
  return (4 < a) ? _data[0] : _data[a];
}


////////////////////////////////////////////////////////////////////////////////


const Vector3&
Vector3::operator= (const Vector3& v)
{
  memcpy(_data, v._data, sizeof(mathType)*4);
  return (*this);
}


////////////////////////////////////////////////////////////////////////////////


  /// scalar multiplication

Vector3
Vector3::scalar (const mathType r)const
{
  return Vector3((r * _data[0]),(r * _data[1]),(r * _data[2]));

#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_DEVNOTES
  std::cout << std::endl;
  std::cout << std::endl << "DevNote from Vector3::scalar (const mathType r)const" ;
  std::cout << std::endl << "             This method is obsolete ...";
  std::cout << std::endl << "             Use the * operator instead.";
  std::flush(std::cout);
#endif

}


////////////////////////////////////////////////////////////////////////////////


  /// scale this vector

void
Vector3::scale (const mathType r)
{
  _data[0] *= r;
  _data[1] *= r;
  _data[2] *= r;

#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_DEVNOTES
  std::cout << std::endl;
  std::cout << std::endl << "Warning from Vector3::scale (const mathType r)" ;
  std::cout << std::endl << "             This method is obsolete ...";
  std::cout << std::endl << "             Use the * operator instead.";
  std::flush(std::cout);
#endif

}


////////////////////////////////////////////////////////////////////////////////


  /// normalize the vector

void
Vector3::normalize()
{
	const mathType l(length());

  if(l!=0)
  {
	  _data[0]=_data[0]/l;
	  _data[1]=_data[1]/l;
	  _data[2]=_data[2]/l;
  }
}


////////////////////////////////////////////////////////////////////////////////


  /// returns a normalized copy of this vector

Vector3
Vector3::normalized() const
{
	Vector3 nn(*this);
	nn.normalize();

  return nn;
}


////////////////////////////////////////////////////////////////////////////////


/// vector addition

/*extern*/
Vector3 operator+(const Vector3& a, const Vector3& b)
{
  return Vector3(a[0] + b[0],
                 a[1] + b[1],
                 a[2] + b[2]);
}


////////////////////////////////////////////////////////////////////////////////


/// vector addition

void
Vector3::operator+=(const Vector3& b)
{
  _data[0] += b[0];
  _data[1] += b[1];
  _data[2] += b[2];
}


////////////////////////////////////////////////////////////////////////////////


/// vector substraction

/*extern*/
Vector3 operator-(const Vector3& a, const Vector3& b)
{
  return Vector3(a[0] - b[0],
                 a[1] - b[1],
                 a[2] - b[2]);
}


////////////////////////////////////////////////////////////////////////////////


/// vector substraction

void
Vector3::operator-=(const Vector3& b)
{
  _data[0] -= b[0];
  _data[1] -= b[1];
  _data[2] -= b[2];
}


////////////////////////////////////////////////////////////////////////////////


/// /// Point3 - Point3

/*extern*/
Vector3 operator-(const Point3& a, const Point3& b)
{
  return Vector3(a[0] - b[0],
                 a[1] - b[1],
                 a[2] - b[2]);
}


////////////////////////////////////////////////////////////////////////////////


/// dot product of two vectors

/*extern*/
mathType operator*(const Vector3& a, const Vector3& b)
{
  return (a[0]*b[0])+(a[1]*b[1])+(a[2]*b[2]);
}


////////////////////////////////////////////////////////////////////////////////


/// cross product

/*extern*/
Vector3 cross(const Vector3& a, const Vector3& b)
{
  return Vector3(((a[1]*b[2]) - (a[2]*b[1])),
	               ((a[2]*b[0]) - (a[0]*b[2])),
                 ((a[0]*b[1]) - (a[1]*b[0]))) ;
}


////////////////////////////////////////////////////////////////////////////////


/// operator for notation Vector3*mathType

/*extern*/
Vector3 operator* (const Vector3& a, const mathType& s)
{
  return Vector3(a[0]*s, a[1]*s, a[2]*s);
}


////////////////////////////////////////////////////////////////////////////////


/// operator for notation Vector3*=mathType

void
Vector3::operator*= (const mathType& s)
{
  _data[0] *= s;
  _data[1] *= s;
  _data[2] *= s;
}


////////////////////////////////////////////////////////////////////////////////


/// operator for notation mathType*Vector3

/*extern*/
Vector3 operator* (const mathType& s, const Vector3& a)
{
  return Vector3(a[0]*s, a[1]*s, a[2]*s);
}


////////////////////////////////////////////////////////////////////////////////


/// operator for notation Vector3/mathType

/*extern*/
Vector3 operator/ (const Vector3& a, const mathType& s)
{
  mathType d = 1.0/s;
  return Vector3(a[0]*d, a[1]*d, a[2]*d);
}


////////////////////////////////////////////////////////////////////////////////


/// operator for notation Vector3/=mathType

void
Vector3::operator/= (const mathType& s)
{
  mathType d = 1.0/s;
  _data[0] *= d;
  _data[1] *= d;
  _data[2] *= d;
}


////////////////////////////////////////////////////////////////////////////////


/// equal

/*extern*/
bool operator== (const Vector3& a, const Vector3& b)
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
bool operator!= (const Vector3& a, const Vector3& b)
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
operator<< (std::ostream& os, const Vector3& a)
{
  os << "Vector3(" << std::fixed << a[0] << "," << a[1] << "," << a[2] << ")";
  return os;
}


////////////////////////////////////////////////////////////////////////////////


} // namespace gloost
