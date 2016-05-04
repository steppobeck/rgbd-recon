
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



#ifndef GLOOST_PLANE_H
#define GLOOST_PLANE_H



/// gloost system includes
#include <Point3.h>
#include <Vector3.h>



/// cpp includes
#include <string>



namespace gloost
{


  ///  keeps a plane in the HNF notation (which is nice)

class Plane
{
	public:
    /// class constructor
    Plane(const Point3& point, const Vector3& normal);

    /// class destructor
	  ~Plane();


	  /// return true if the Point3 is in front of the Plane
	  bool pointInFrontOf(const Point3& point) const;



	private:

	  Point3  _point;
	  Vector3 _normal;

};


} // namespace gloost


#endif // GLOOST_PLANE_H



