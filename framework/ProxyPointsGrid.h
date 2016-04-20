



#ifndef H_KINECT_PROXY_POINTS_GRID
#define H_KINECT_PROXY_POINTS_GRID



/// gloost system includes
#include <gloostConfig.h>
#include <gloostMath.h>


/// cpp includes
#include <string>
#include <GL/glew.h>



namespace kinect
{


  //  Generates and renders resH x resV points in a uniform raster grid

class ProxyPointsGrid
{
	public:

    // class constructor
    ProxyPointsGrid(unsigned int resH,
                    unsigned int resV);

    // class destructor
	  ~ProxyPointsGrid();


    // inits the ProxyPointsGrid
	  void init();

    // draws the ProxyPointsGrid
	  void draw();


    // returns horizontal resolution of the grid
	  unsigned int getResH () const;
    // returns vertical resolution of the grid
	  unsigned int getResV () const;


	protected:

   unsigned int _resH;
   unsigned int _resV;

   GLuint       _displayListHandle;

};


} // namespace kinect


#endif // H_KINECT_PROXY_POINTS_GRID


