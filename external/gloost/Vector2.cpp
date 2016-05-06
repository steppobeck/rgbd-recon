
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



// gloost system includes
#include <Vector2.h>
#include <gloostMath.h>



// cpp includes
#include <cstring>



namespace gloost
{

Vector2::operator glm::fvec2() const {
  return glm::fvec2{_data[0], _data[1]};
}

////////////////////////////////////////////////////////////////////////////////


  /// ...

Vector2::Vector2():
  _data()
{
    _data[0]=0.0;
    _data[1]=0.0;
}


////////////////////////////////////////////////////////////////////////////////


  /// ...

Vector2::Vector2 (const Vector2& p)
  : _data ()
{
  memcpy(_data, p._data, sizeof(float)*2);
}


////////////////////////////////////////////////////////////////////////////////


  /// ...

Vector2::Vector2(const float a, const float b)
    : _data()
{
    _data[0]=a;
    _data[1]=b;
}


////////////////////////////////////////////////////////////////////////////////


  /// ...

float
Vector2::length() const

{
  return (sqrt((_data[0]*_data[0])+
               (_data[1]*_data[1])));
}


////////////////////////////////////////////////////////////////////////////////


  /// ...

float
Vector2::length2() const

{
  return _data[0]*_data[0] + _data[1]*_data[1];
}


///////////////////////////////////////////////////////////////////////////////


  /// contrains the vector to a particluar length but preserves the direction

void
Vector2::constrain (const double length)
{
  const double l(length2());

  if(l > length*length)
  {
    const double factor(length/sqrt(l));

    _data[0]=_data[0]*factor;
    _data[1]=_data[1]*factor;
  }
}


////////////////////////////////////////////////////////////////////////////////


  /// ...

Vector2
Vector2::angle2Vector(float angle) const

{
  float coss = cos(angle*PI/180.00);
  float sinn = sin(angle*PI/180.00);

  return Vector2(_data[0]*coss-_data[1]*sinn,
                     _data[0]*sinn+_data[1]*coss);
}


////////////////////////////////////////////////////////////////////////////////


  /// ...

void
Vector2::rotate(float angle)

{
  float coss = cos(angle*PI/180.00);
  float sinn = sin(angle*PI/180.00);

  float x = _data[0];
  float y = _data[1];

  _data[0] = x*coss-y*sinn;
  _data[1] = x*sinn+y*coss;
}


////////////////////////////////////////////////////////////////////////////////


 /// return the angle of the vectors direction

float
Vector2::vector2angle() const
{
  float deltaH = _data[0];
  float deltaV = _data[1];

  float the_angle;

  if (deltaH != 0)
  {
    float slope = deltaV/deltaH;
    the_angle   = atan(slope);

    if (deltaH  < 0)
    {
      the_angle = the_angle + PI;
    }

  }
  else if (deltaV > 0)
  {
    the_angle = PI / 2.0;
  }
  else if (deltaV < 0)
  {
    the_angle = (3 * PI) / 2.0;
  }
  else
  {
    the_angle = 0;
  }

  // Convert rad to degre
  the_angle = (the_angle * 180) / PI + 90.0;

  return the_angle;
}


////////////////////////////////////////////////////////////////////////////////


  /// ...

float&
Vector2::operator[](unsigned int a)
{
  return (1 < a) ? _data[0] : _data[a];
}


////////////////////////////////////////////////////////////////////////////////


  /// ...

const float&
Vector2::operator[] (unsigned int a) const
{
  return (1 < a) ? _data[0] : _data[a];
}


////////////////////////////////////////////////////////////////////////////////


  /// ...

const Vector2&
Vector2::operator= (const Vector2& a)
{
  memcpy(_data, a._data, sizeof(float)*2);
  return (*this);
}


////////////////////////////////////////////////////////////////////////////////


  /// ...

Vector2
Vector2::operator+(const Vector2& a)const
{
  return Vector2((_data[0]+a[0]), (_data[1]+a[1]));
}


////////////////////////////////////////////////////////////////////////////////


  /// ...

Vector2
Vector2::operator-(const Vector2& a)const
{
  return Vector2((_data[0]-a[0]), (_data[1]-a[1])) ;
}


////////////////////////////////////////////////////////////////////////////////


  /// Skalar multiplikation

float
Vector2::operator*(const Vector2& a)const
{
  return (_data[0]*a[0])+(_data[1]*a[1]);
}


////////////////////////////////////////////////////////////////////////////////


  /// multiplication with a scalar

Vector2
Vector2::scalar (const float r) const
{
  return Vector2((r * _data[0]),(r * _data[1]));
}


////////////////////////////////////////////////////////////////////////////////


  /// scale the vector

void
Vector2::scale (const float r)
{
  _data[0] *= r;
  _data[1] *= r;
}


////////////////////////////////////////////////////////////////////////////////


  /// normalize the vector

void
Vector2::normalize()
{
	float l = length2();

  if(l)
  {
	  _data[0]=_data[0]/l;
	  _data[1]=_data[1]/l;
  }

}


////////////////////////////////////////////////////////////////////////////////


  /// get a normalized copy of the vector

Vector2
Vector2::getNormalized()
{
	float l = length2();

  if(l)
  {
    return Vector2(_data[0]/l, _data[1]/l);
  }
  else
  {
    return Vector2(0, 0);
  }

}


////////////////////////////////////////////////////////////////////////////////


 /// get Vector2er to an array

const float*
Vector2::data() const
{
  return _data;
}


////////////////////////////////////////////////////////////////////////////////


 /// get Vector2er to an array

float*
Vector2::data()
{
  return _data;
}


////////////////////////////////////////////////////////////////////////////////


  /// operator for notation Vector2*float

/*extern*/
Vector2
operator* (const Vector2& v, const float& f)
{
  return v.scalar(f);
}


////////////////////////////////////////////////////////////////////////////////


  /// operator for notation float*Vector2

/*extern*/
Vector2
operator* (const float& f, const Vector2& v)
{
  return v.scalar(f);
}


////////////////////////////////////////////////////////////////////////////////


  /// ...

/* extern */
std::ostream&
operator<< (std::ostream& os, const Vector2& a)
{
  os << "Vector2(" << a[0] << "," << a[1] << ")";
  return os;
}

////////////////////////////////////////////////////////////////////////////////


} // SDLE

