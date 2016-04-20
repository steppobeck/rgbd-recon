
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



#ifndef GLOOST_FRUSTUM_H
#define GLOOST_FRUSTUM_H



/// gloost system includes
#include <Point3.h>
#include <Matrix.h>
#include <Plane.h>



/// cpp includes
#include <string>
#include <vector>



namespace gloost
{


  ///  Camera Frustum with inside() and lineIntersect()

class Frustum
{
	public:
    /// class constructor
    Frustum();

    Frustum(const Frustum& frustum);

    /// class destructor
	  ~Frustum();



    /// return TRUE if the point is inside the frustum
    bool inside(const Point3& point) const;

    /// return TRUE if a line intersects with the frustum
    bool lineIntersect(const Point3& point1, const Point3& point2) const;


    /// recalculate the bounding planes of this frustum such that the normal points away from the frustum
    void recalcPlanes();


	  /// assignment
    const Frustum& operator= (const Frustum&);



    Point3 near_lower_left;
    Point3 near_lower_right;
    Point3 near_upper_right;
    Point3 near_upper_left;
    Point3 far_lower_left;
    Point3 far_lower_right;
    Point3 far_upper_right;
    Point3 far_upper_left;


	private:

	  /// vector of planes with order: front, left, right, top, bottom, beck
	  std::vector<Plane> _planes;

};

/// operator for notation Matrix*Frustum
extern Frustum operator* (const Matrix&, const Frustum&);


} // namespace gloost


#endif // GLOOST_FRUSTUM_H


