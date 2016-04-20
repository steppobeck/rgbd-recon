
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



#ifndef GLOOST_BOUNDINGVOLUME_H
#define GLOOST_BOUNDINGVOLUME_H



/// gloost system includes
#include <Matrix.h>
#include <Frustum.h>



/// ids for different kinds of bounding volumes
#define GLOOST_BOUNDINGVOLUME_BOX       0
#define GLOOST_BOUNDINGVOLUME_SPHERE    1
#define GLOOST_BOUNDINGVOLUME_SLABS     2
#define GLOOST_BOUNDINGVOLUME_GEOMETRY  3



namespace gloost
{

  class Ray;


  ///  bounding volume

class BoundingVolume
{

 public:

  /// class constructor
   BoundingVolume(unsigned int type);
   
   /// class destructor
     virtual ~BoundingVolume();
     

	  /// intersect with a ray
    virtual bool intersect(Ray& /*r*/){return 0;};


    /// return TRUE if BoundingVolume intersects with frustum
    virtual bool intersectWithFrustum(const Frustum& /*frustum*/){return 1;};

    /// return true if point p is inside
    virtual bool inside(const Point3& p){return 1;};

	  /// draw bounding box
    virtual void draw(){};


    /// transform the volume with a matrix
    virtual void transform(const Matrix& /*m*/){};


    /// return type of this BoundingVolume
    unsigned int getType();


  protected:

    ///
    unsigned int _typeId;


};


} // namespace gloost


#endif // GLOOST_BOUNDINGVOLUME_H

