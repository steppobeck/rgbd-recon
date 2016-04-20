
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
#include <BoundingBox.h>
#include <Vector3.h>
#include <Point3.h>
#include <Ray.h>
#include <gloostRenderGoodies.h>



/// cpp includes



namespace gloost
{

///////////////////////////////////////////////////////////////////////////////


 /// class constructor

BoundingBox::BoundingBox(const Point3& pMin, const Point3& pMax):
  BoundingVolume(GLOOST_BOUNDINGVOLUME_BOX),
  _pMin(pMin),
  _pMax(pMax)
{
	// insert your code here
}

///////////////////////////////////////////////////////////////////////////////


 /// class constructor

BoundingBox::BoundingBox():
  BoundingVolume(GLOOST_BOUNDINGVOLUME_BOX),
  _pMin(0,0,0),
  _pMax(0,0,0)
{
	// insert your code here
}


///////////////////////////////////////////////////////////////////////////////


 /// class destructor

BoundingBox::~BoundingBox()
{
	// insert your code here
}


////////////////////////////////////////////////////////////////////////////////


 /// copy

const BoundingBox&
BoundingBox::operator= (const BoundingBox& b)
{
  if (this != &b)
  {
    _pMin = b._pMin;
    _pMax = b._pMax;
  }
  return *this;
}


///////////////////////////////////////////////////////////////////////////////


 ///

void
BoundingBox::setPMin(const Point3& pMin)
{
	_pMin = pMin;
}


///////////////////////////////////////////////////////////////////////////////


 ///

const Point3&
BoundingBox::getPMin() const
{
	return _pMin;
}


///////////////////////////////////////////////////////////////////////////////


 ///

void
BoundingBox::setPMax(const Point3& pMax)
{
	_pMax = pMax;
}


///////////////////////////////////////////////////////////////////////////////


 ///

const Point3&
BoundingBox::getPMax() const
{
	return _pMax;
}


///////////////////////////////////////////////////////////////////////////////


 /// intersect with a ray

bool
BoundingBox::intersect(Ray& r)
{

	// source: http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm


	/*
	For each pair of planes P associated with X, Y, and Z do:
  (example using X planes)
  if direction Xd = 0 then the ray is parallel to the X planes.
  So, if origin Xo is not between the slabs ( Xo < Xl or Xo > Xh) then return false
  else, if the ray is not parallel to the plane then
  begin
  compute the intersection distance of the planes
	T1 = (Xl - Xo) / Xd
  T2 = (Xh - Xo) / Xd
  If T1 > T2 swap (T1, T2) // since T1 intersection with near plane

  If T1 > Tnear set Tnear =T1 // want largest Tnear
  If T2 < Tfar set Tfar="T2" // want smallest Tfar

  If Tnear > Tfar box is missed so return false
  If Tfar < 0 box is behind ray return false end

  end of for loop
  If Box survived all above tests, return true with
  intersection point Tnear and exit point Tfar.
	*/

	double tNear = -1000000;
	double tFar  = 1000000;

	// parallel to plane 0..1
	for (int i=0; i!=3; ++i)
	{
    //std::cout << std::endl << i;
    if (r.getDirection()[i] == 0)
    {
      if (!(_pMin[i] < r.getOrigin()[i] && _pMax[i] > r.getOrigin()[i]))
      {
        //std::cout << std::endl << "Parallel but not within: " << 0;
        return 0;
      }
    }


    double t1 = (_pMin[i] - r.getOrigin()[i]) / r.getDirection()[i];
    double t2 = (_pMax[i] - r.getOrigin()[i]) / r.getDirection()[i];

    // swap
    if (t1 > t2)
    {
      double temp = t1;
      t1 = t2;
      t2 = temp;
    }

    if (t1 > tNear)
    {
      tNear = t1;
    }

    if (t2 < tFar)
    {
      tFar = t2;
    }

    if (tNear > tFar)
    {
      return 0;
    }

    if (tFar < 0)
    {
      return 0;
    }
	}
  r.setT(tNear);

  return 1;
}




////////////////////////////////////////////////////////////////////////////////


    /// return TRUE if BoundingBox intersects with frustum

/*virtual*/ bool
BoundingBox::intersectWithFrustum(const Frustum& frustum)
{

/*
      _pMin
          g-----h
         /     /|
        /     / |
        c----d  f
  e --> |    | /
        |    |/
        a----b
             _pMax
*/

  gloost::Point3 a(_pMin[0], _pMax[1], _pMax[2]);
  gloost::Point3 b(_pMax);
  gloost::Point3 c(_pMin[0], _pMin[1], _pMax[2]);
  gloost::Point3 d(_pMax[0], _pMin[1], _pMax[2]);
  gloost::Point3 e(_pMin[0], _pMax[1], _pMin[2]);
  gloost::Point3 f(_pMax[0], _pMax[1], _pMin[2]);
  gloost::Point3 g(_pMin);
  gloost::Point3 h(_pMax[0], _pMin[1], _pMin[2]);

  if (frustum.inside(a)) return true;
  if (frustum.inside(b)) return true;
  if (frustum.inside(c)) return true;
  if (frustum.inside(d)) return true;
  if (frustum.inside(e)) return true;
  if (frustum.inside(f)) return true;
  if (frustum.inside(g)) return true;
  if (frustum.inside(h)) return true;

  return false;
}


////////////////////////////////////////////////////////////////////////////////

/// return true if point p is inside;
/*virtual*/ bool
BoundingBox::inside(const Point3& p){
  return (_pMin[0] < p[0]) &&
    (_pMin[1] < p[1]) &&
    (_pMin[2] < p[2]) &&
    (_pMax[0] > p[0]) &&
    (_pMax[1] > p[1]) &&
    (_pMax[2] > p[2]);
}


////////////////////////////////////////////////////////////////////////////////
 /// draw bounding box

/*virtual*/ void
BoundingBox::draw()
{

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  {
    glEnable(GL_COLOR_MATERIAL);
    glColor4f(1.0, 0.3, 0.3, 0.75);

    glDisable(GL_LIGHTING);

    glLineWidth(2.0);


    gloost::drawWiredBox(_pMin[0], _pMin[1], _pMin[2],
                         _pMax[0],  _pMax[1],  _pMax[2]);
  }
  glPopAttrib();

}



////////////////////////////////////////////////////////////////////////////////


  /// tarnsform the bounding box with a matrix

/*virtual*/
void
BoundingBox::transform(const Matrix& m)
{
    (*this) = m * (*this);
}



////////////////////////////////////////////////////////////////////////////////


  /// operator for notation Matrix*BoundingBox

/*extern*/
BoundingBox
operator* (const Matrix& matrix, const BoundingBox& boundingBox)
{
  return BoundingBox(matrix*boundingBox.getPMin(),
                     matrix*boundingBox.getPMax());
}


////////////////////////////////////////////////////////////////////////////////


  ///

/* extern */
std::ostream&
operator<< (std::ostream& os, const BoundingBox& bb)
{
  os << "BoundingBox( " << std::fixed
                        << bb.getPMin() << std::endl
                        << bb.getPMax() << std::endl << ")";
  return os;
}


///////////////////////////////////////////////////////////////////////////////


 ///










} // namespace GG

