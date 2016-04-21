
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



#ifndef GLOOST_VECTOR3_H
#define GLOOST_VECTOR3_H



/// gloost includes
#include <gloostConfig.h>



/// cpp includes
#include <cmath>
#include <iostream>
#include <fstream>



namespace gloost
{

  class Point3;




 /// 3D vector

class Vector3
{

  public:

    /// create default (0.0, 0.0, 0.0) vector
    Vector3();

    /// copy constructor
    Vector3(const Vector3&);

    /// create a vector from a point (!!! fourth component will be set to 0.0)
    Vector3(const Point3&);

    /// initialize with values
    Vector3(const mathType, const mathType, const mathType);

    /// for use as quaternions, do not use for 3d coords
    Vector3(const mathType, const mathType, const mathType, const mathType);

    /// read three space seperated floats from a ifstream into the vector
    Vector3(std::ifstream& in);



    /// indexing
    mathType& operator[](unsigned int);
    const mathType& operator[](unsigned int) const;

    /// assignment
    const Vector3& operator= (const Vector3&);



    /// operator for notation Vector3+=Vector3
    void operator+=(const Vector3&);

    /// operator for notation Vector3-=Vector3
    void operator-=(const Vector3&);

    /// operator for notation Vector3*=mathType
    void operator*= (const mathType&);

    /// operator for notation Vector3/=mathType
    void operator/= (const mathType&);




    /// scalar multiplication (obsolete)
    Vector3 scalar(const mathType ) const;

    /// scale this vector (obsolete)
    void scale(const mathType );




    /// normalize the vector
    void normalize();

    /// returns a normalized copy of this vector
    Vector3 normalized() const;

    /// length of the vector
    mathType length() const;

    /// quadratic length of the vector
    mathType length2() const;

    /// contrains the vector to a particluar length but preserves the direction
    void constrain (const mathType length);


    /// get pointer to the vectors data as array
    const mathType* data() const;
    mathType* data();

private:

    mathType _data[4];

};




/// vector addition
extern Vector3 operator+(const Vector3&, const Vector3&);

/// vector substraction
extern Vector3 operator-(const Vector3&, const Vector3&);

/// Point3 - Point3
extern Vector3 operator-(const Point3&, const Point3&);

/// dot product of two vectors
extern mathType operator*(const Vector3&, const Vector3&);



/// cross product
extern Vector3 cross(const Vector3&, const Vector3&);



/// operator for notation Vector3*mathType
extern Vector3 operator* (const Vector3&, const mathType&);

/// operator for notation mathType*Vector3
extern Vector3 operator* (const mathType&, const Vector3&);


/// operator for notation Vector3/mathType
extern Vector3 operator/ (const Vector3&, const mathType&);


/// equal
extern bool operator== (const Vector3&, const Vector3&) ;

/// not equal
extern bool operator!= (const Vector3&, const Vector3&);




/// ostream operator
extern std::ostream& operator<< (std::ostream&, const Vector3&);


} // namespace gloost

#endif // GLOOST_VECTOR3_H
