
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



/// gloost system includes
#include <Plane.h>



/// cpp includes
#include <string>
#include <iostream>



namespace gloost
{

///////////////////////////////////////////////////////////////////////////////


  /// class constructor

Plane::Plane(const Point3& point, const Vector3& normal):
	  _point(point),
	  _normal(normal)
{
	// insert your code here
}


///////////////////////////////////////////////////////////////////////////////


  /// class destructor

Plane::~Plane()
{
	// insert your code here
}


///////////////////////////////////////////////////////////////////////////////


  /// return true if the Point3 is in front of the Plane

bool
Plane::pointInFrontOf(const Point3& point) const
{
	return  ((point - _point)*_normal) > 0.0;
}




///////////////////////////////////////////////////////////////////////////////





} // namespace gloost


