
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



#ifndef GLOOST_MATH_H
#define GLOOST_MATH_H



/// cpp includes
#include <sstream>
#include <cmath>
#include <stdlib.h>



// gloost includes
#include  <UniformTypes.h>
#include  <Vector3.h>
#include  <Vector2.h>



/// init rand
#ifndef RAND_MAX
  #define RAND_MAX 2147483647
  //int rand();
#endif



namespace gloost
{


/// define PI
static const float PI = 3.1415926535;


///////////////////////////////////////////////////////////////////////////////


  /// returns a "random" value between 0.0 ... 1.0

inline float
frand ()
{
  return (rand()/((float)RAND_MAX));
}


///////////////////////////////////////////////////////////////////////////////


  /// returns a "random" value between -1.0 ... 1.0

inline float
crand ()
{
  return (rand()/((float)RAND_MAX))*2.0f - 1.0f;
}


///////////////////////////////////////////////////////////////////////////////


  /// returns a "random" value between fMin ... fMax

template <class T>
inline T
getRandomMinMax( T fMin, T fMax )
{
  float fRandNum = (float)rand () / RAND_MAX;
  return fMin + (fMax - fMin) * fRandNum;
}


///////////////////////////////////////////////////////////////////////////////


  ///

inline bool chance(float probability = 0.5)
{
  if (frand() < probability)
  {
    return true;
  }

  return false;
}



///////////////////////////////////////////////////////////////////////////////


  /// returns minimum of two values

template <class T>
inline T
min ( T a, T b)
{
  if ( a > b)
  {
    return b;
  }

  return a;
}


///////////////////////////////////////////////////////////////////////////////


  /// returns maximum of two values

template <class T>
inline T
max ( T a, T b)
{
  if ( a > b)
  {
    return a;
  }

  return b;
}


///////////////////////////////////////////////////////////////////////////////


  /// returns maximum of two values

template <class T>
inline T
abs ( T a )
{
  if ( a < 0)
  {
    return -a;
  }

  return a;
}


///////////////////////////////////////////////////////////////////////////////

/* (this function is templated, so if you want float precision give floats!!!) */

  /// converts degree to rad

template <class T>
inline T
deg2rad (const T& a)
{
  return (T)(a * PI / 180.0);
}


///////////////////////////////////////////////////////////////////////////////

/* (this function is templated, so if you want float precision give floats!!!) */

  /// converts degree to rad

template <class T>
inline T
rad2deg (T a)
{
  return (T)(a * 180.0 / PI);
}


///////////////////////////////////////////////////////////////////////////////

/* (this function is templated, so if you want float precision give floats!!!) */

  /// converts rad to rad (I wonder if sombody will use it??? ;-) )

template <class T>
inline T
rad2rad (T a)
{
  std::cout<< std::endl;
  std::cout << std::endl << "Laughing from gloost::rad2rad" ;
  std::cout<< std::endl << "              Why would you do that ???";
  std::flush(std::cout);

  return a;
}


///////////////////////////////////////////////////////////////////////////////


  /// coverts a angle in a vector

//template <class T>
//inline vec2
//angle2Vector(T angle)
//{
//  return vec2(cos(angle*PI/180.00), sin(angle*PI/180.00));
//}


///////////////////////////////////////////////////////////////////////////////


  /// coverts a angle in a vector

template <class T>
inline vec2
angle2Vector(T angle)
{
  return vec2(cos(angle*0.0174532925199), sin(angle*0.0174532925199));
}


///////////////////////////////////////////////////////////////////////////////


  /// coverts a angle in a vector

template <class T>
inline gloost::Vector3
angle2Vector3xy(T angle)
{
  return gloost::Vector3(cos(angle*0.0174532925199), sin(angle*0.0174532925199), 0.0);
}


///////////////////////////////////////////////////////////////////////////////


  /// coverts a angle in a vector

template <class T>
inline gloost::Vector3
angle2Vector3xz(T angle)
{
  return gloost::Vector3(cos(angle*0.0174532925199), 0.0, sin(angle*0.0174532925199));
}


///////////////////////////////////////////////////////////////////////////////


  /// coverts a 2D vector in to an angle

inline float
vector2angleRad(float deltaH, float deltaV)
{
  float the_angle = 0.0;

  if (deltaH != 0)
  {
    float slope = deltaV / deltaH;
    the_angle = atan (slope);
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
    the_angle = 0.0;
  }

  return the_angle;
}


///////////////////////////////////////////////////////////////////////////////


  /// coverts a 2D vector in to an angle

inline float
vector2angle(float deltaH, float deltaV)
{
  float the_angle = 0.0;

  if (deltaH != 0)
  {
    float slope = deltaV / deltaH;
    the_angle = atan (slope);
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
    the_angle = 0.0;
  }

  the_angle = (the_angle * 180) / PI;

  return the_angle;
}


///////////////////////////////////////////////////////////////////////////////


  /// coverts a 2D vector in to an angle

inline float
vector2angle(vec2 vector)
{
  return vector2angle(vector.u, vector.v);
}


///////////////////////////////////////////////////////////////////////////////


 /// the modulo or "%" operator for all scalar types

template <class T>
T fmod(T a, T b)
{
  int result = static_cast<int>( a / b );
  return a - static_cast<double>( result ) * b;
}


///////////////////////////////////////////////////////////////////////////////


  /// returns TRUE if two lines are intersect each other

inline bool
lineIntersect2D(const Vector2& line1P1, const Vector2& line1P2,
                const Vector2& line2P1, const Vector2& line2P2 )
{
  // direction vectors of both lines
  gloost::Vector2 d1 = line1P2 - line1P1;
  gloost::Vector2 d2 = line2P2 - line2P1;


  float div = d1[0] * d2[1] - d1[1] * d2[0];

  float t = -(line1P1[0] * d2[1] - line1P1[1] * d2[0] - line2P1[0] * d2[1] + line2P1[1] * d2[0]) / div;
  float s = -(line1P1[0] * d1[1] - line1P1[1] * d1[0] + d1[0] * line2P1[1] - d1[1] * line2P1[0]) / div;

  // or u1.multiply(s) u0.plus(u1);
  d1 = d1 * t;

  // der Punkt wo sie sich schneiden
  gloost::Vector2 p = line1P1 + d1;

  if ((t > 0 && s > 0) && (t < 1 && s < 1))
  {
     //std::cout << std::endl << "intersect at: " << "[x: " << p[0] << " y: " << p[1] << "]";
     return 1;
  }

  return 0;
}




}  // namespace gloost


#endif // #ifndef GLOOST_MATH_H


