
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



#ifndef GLOOST_BOUNDINGBOX_H
#define GLOOST_BOUNDINGBOX_H



/// gloost system includes
#include <BoundingVolume.h>
#include <Vector3.h>
#include <Matrix.h>
#include <Point3.h>
#include <Ray.h>



/// cpp includes



namespace gloost
{


/*
 _pMin
      o-----
     /     /|
    /     / |
    ------  |
    |    | /
    |    |/
    -----o
         _pMax
*/


  ///  bounding box

class BoundingBox : public BoundingVolume
{
	public:

    /// class constructor
    BoundingBox(const Point3& pMin, const Point3& pMax);
    /// class constructor
    BoundingBox();
    /// class destructor
    ~BoundingBox();



    /// assignment
    const BoundingBox& operator= (const BoundingBox&);



    ///
    void setPMin(const Point3& pMin);
    ///
    const Point3& getPMin() const;
    ///
    void setPMax(const Point3& pMax);
    ///
    const Point3& getPMax() const;


	  /// intersect with a ray
    bool intersect(Ray& r);


    /// return TRUE if BoundingBox intersects with frustum
    /*virtual*/ bool intersectWithFrustum(const Frustum& frustum);

    /// return true if point p is inside;
    /*virtual*/ bool inside(const Point3& p);


    /// draw bounding box
    /*virtual*/ void draw();



    /// tarnsform the bounding box with a matrix
    /*virtual*/ void transform(const Matrix& m);


  protected:

    /// min and max point;
    Point3 _pMin;
    Point3 _pMax;


};




/// operator for notation Matrix*BoundingBox
extern BoundingBox operator* (const Matrix&, const BoundingBox&);


/// ostream operator
extern std::ostream& operator<< (std::ostream&, const BoundingBox&);





} // namespace gloost


#endif // GLOOST_BOUNDINGBOX_H

