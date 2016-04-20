
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



#ifndef GLOOST_RAY_H
#define GLOOST_RAY_H



/// gloost system includes
#include <gloostConfig.h>
#include <Point3.h>
#include <Vector3.h>



namespace gloost
{




  ///  ray with point, vector and length

class Ray
{
	public:


    /// class constructor
    Ray();
    /// class constructor
    Ray(const Point3& origin, const Vector3& direction);
    /// class constructor
    Ray(const Point3& origin, const Vector3& direction, mathType t);


    /// class destructor
    ~Ray();


    /// Copy
    const Ray& operator= (const Ray&);


    /// set origin
    void setOrigin(const Point3& origin);
    /// get origin
    const Point3& getOrigin() const;


    /// set direction
    void setDirection(const Vector3& direction);
    /// get direction
    const Vector3& getDirection() const;


    /// set t
    void setT(mathType t);
    /// Set t to maximal value
    void setTMax();
    /// get t
    const mathType& getT() const;

    mathType& getT();


    /// get the point origin + direction * t
    Point3 getDestination() const;


    /// normalize length of direction
    void normalize();

    /// returns a normalized version of the ray
    Ray normalized();



	private:


    ///
    Point3    _origin;
    Vector3   _direction;
    mathType  _t;



    static mathType T_MAX;


  protected:

    ///


};


extern std::ostream& operator<< (std::ostream&, const Ray&);


} // namespace gloost


#endif // GLOOST_RAY_H

