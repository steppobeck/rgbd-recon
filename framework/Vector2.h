
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



#ifndef GLOOST_VECTOR2_H
#define GLOOST_VECTOR2_H


/// cpp includes
#include <cmath>
#include <iostream>


namespace gloost
{


 /// 2D vector

class Vector2
{
 public:

    Vector2();
    Vector2(const Vector2&);
    Vector2(const float, const float);

    /// indexing
    float& operator[](unsigned int);
    const float& operator[](unsigned int)const;

    /// Copy
    const Vector2& operator= (const Vector2&);

    /// vector addition
    Vector2 operator+(const Vector2&) const;

    /// vector substraction
    Vector2 operator-(const Vector2&) const;

    /// scalar product
    float operator*(const Vector2&) const;

    /// multiplication with a scalar
    Vector2 scalar(const float) const;

    /// scale the vector
    void scale(const float);


    /// normalize the vector
    void normalize();

    /// get a normalized copy of the vector
    Vector2 getNormalized();


    /// Length of the vector
    float length() const;

    /// Length2 of the vector
    float length2() const;


    /// contrains the vector to a particluar length but preserves the direction
    void constrain (const double length);


    /// return the point rotated ccw in degree
    Vector2 angle2Vector(float angle) const;

    /// rotate this vector
    void rotate(float angle);

    /// return the angle of the vectors direction
    float vector2angle () const;


    /// get pointer to an array
    const float* data() const;
    float* data();

private:

    float _data[2];

};


/// operator for notation Vector2*float
extern Vector2 operator* (const Vector2&, const float&);

/// operator for notation float*Vector2
extern Vector2 operator* (const float&, const Vector2&);


extern std::ostream& operator<< (std::ostream&, const Vector2&);


} // namespace SDLE

#endif
