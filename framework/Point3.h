
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



#ifndef GLOOST_POINT3_H
#define GLOOST_POINT3_H



/// gloost includes
#include <gloostConfig.h>



/// cpp includes
#include <cmath>
#include <iostream>



namespace gloost
{

  class Vector3;



 /// 3D point

class Point3
{
 public:

    Point3();
    Point3(const Point3&);
    Point3(const Vector3&);
    Point3(const mathType, const mathType, const mathType);

    /// reads three space seperated floats into the point
    Point3(std::ifstream& in);



    /// indexing
    mathType& operator[](unsigned int);
    const mathType& operator[](unsigned int)const;



    /// Assign
    const Point3& operator= (const Point3&);

    /// operator for notation Point3+=Vector3
    void operator+=(const Vector3&);

    /// operator for notation Point3-=Vector3
    void operator-=(const Vector3&);




    /// length of the vector from the origin to the point
    mathType distanceToOrigin() const;

    /// quadratic length of the vector from the origin to the point
    mathType distanceToOrigin2() const;



    /// get pointer to the points data as array
    const mathType* data() const;
    mathType* data();


    /// origin
    static Point3 origin;

private:

    mathType _data[4];

};


/// operator for notation Point3*mathType
extern Point3 operator* (const Point3&, const mathType&);

/// operator for notation mathType*Point3
extern Point3 operator* (const mathType&, const Point3&);



/// Point3 + Vector3
extern Point3 operator+(const Point3&, const Vector3&);

/// Point3 - Vector3
extern Point3 operator-(const Point3&, const Vector3&);



/// equal
extern bool operator== (const Point3&, const Point3&);

/// not equal
extern bool operator!= (const Point3&, const Point3&);



/// ostream operator
extern std::ostream& operator<< (std::ostream&, const Point3&);


} // namespace gloost

#endif // GLOOST_POINT3_H
