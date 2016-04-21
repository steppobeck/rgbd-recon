
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
#include <Frustum.h>



/// cpp includes
#include <string>
#include <iostream>
#include <cstring>



namespace gloost
{

///////////////////////////////////////////////////////////////////////////////


  /// class constructor

Frustum::Frustum():
    near_lower_left(),
    near_lower_right(),
    near_upper_right(),
    near_upper_left(),
    far_lower_left(),
    far_lower_right(),
    far_upper_right(),
    far_upper_left(),
    _planes()
{

	/// fill with stuff
	for(unsigned int i=0; i != 6; ++i)
	{
	  _planes.push_back(Plane(Point3(), Vector3()));
	}



}


///////////////////////////////////////////////////////////////////////////////


  /// class constructor

Frustum::Frustum(const Frustum& frustum)
{
  near_lower_left  = frustum.near_lower_left;
  near_lower_right = frustum.near_lower_right;
  near_upper_right = frustum.near_upper_right;
  near_upper_left  = frustum.near_upper_left;
  far_lower_left   = frustum.far_lower_left;
  far_lower_right  = frustum.far_lower_right;
  far_upper_right  = frustum.far_upper_right;
  far_upper_left   = frustum.far_upper_left;

  _planes = frustum._planes;
}


///////////////////////////////////////////////////////////////////////////////


  /// class destructor

Frustum::~Frustum()
{
	// insert your code here
}


////////////////////////////////////////////////////////////////////////////////


/// return TRUE if the point is inside the frustum

bool
Frustum::inside(const Point3& point) const
{

	for(unsigned int i=0; i != 6; ++i)
	{
	  if (_planes[i].pointInFrontOf(point))
	    return false;
	}

	return true;
}



////////////////////////////////////////////////////////////////////////////////


/// return TRUE if a line intersects with the frustum

bool
Frustum::lineIntersect(const Point3& point1, const Point3& point2) const
{

  std::cout << std::endl;
  std::cout << std::endl << "Warning from Frustum::lineIntersect(const Point3& point1, const Point3& point2) const:" ;
  std::cout << std::endl << "             Sorry, but this method is not implemented...";
  std::flush(std::cout);

  /// avoid compiler warings
  gloost::Point3 a = point1;
  gloost::Point3 b = point2;


	return true;
}


////////////////////////////////////////////////////////////////////////////////


  /// recalculate the bounding planes of this frustum such that the normal points away from the frustum

void
Frustum::recalcPlanes()
{
  /// front
  _planes[0] = Plane(near_lower_left,
                     cross ( (near_lower_right-near_lower_left), (near_upper_left-near_lower_left) ) );

  /// left
  _planes[1] = Plane(far_lower_left,
                     cross (near_lower_left-far_lower_left, (far_upper_left-far_lower_left) ));

  /// right
  _planes[2] = Plane(near_lower_right,
                     cross ((far_lower_right-near_lower_right), (near_upper_right-near_lower_right) ));

  /// top
  _planes[3] = Plane(near_upper_right,
                     cross( (far_upper_right-near_upper_right), (near_upper_left-near_upper_right) ));

  /// bottom
  _planes[4] = Plane(near_lower_left,
                     cross( (far_lower_left-near_lower_left), (near_lower_right-near_lower_left) ));

  /// far
  _planes[5] = Plane(far_lower_right,
                     cross( (far_lower_left-far_lower_right), (far_upper_right-far_lower_right) ));
}


////////////////////////////////////////////////////////////////////////////////


const Frustum&
Frustum::operator= (const Frustum& frustum)
{
  memcpy(this, &frustum, sizeof(Frustum));
  return (*this);
}

////////////////////////////////////////////////////////////////////////////////


  /// operator for notation scalar*Vector3

/*extern*/
Frustum
operator* (const Matrix& matrix, const Frustum& frustum)
{

  Frustum newFrustum;

  newFrustum.near_lower_left  = matrix * frustum.near_lower_left;
  newFrustum.near_lower_right = matrix * frustum.near_lower_right;
  newFrustum.near_upper_right = matrix * frustum.near_upper_right;
  newFrustum.near_upper_left  = matrix * frustum.near_upper_left;

  newFrustum.far_lower_left   = matrix * frustum.far_lower_left;
  newFrustum.far_lower_right  = matrix * frustum.far_lower_right;
  newFrustum.far_upper_right  = matrix * frustum.far_upper_right;
  newFrustum.far_upper_left   = matrix * frustum.far_upper_left;

  return newFrustum;
}


///////////////////////////////////////////////////////////////////////////////


  /// ...







///////////////////////////////////////////////////////////////////////////////





} // namespace gloost



