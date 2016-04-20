



#ifndef H_PMD_PROXY_MESH_GRID
#define H_PMD_PROXY_MESH_GRID



/// gloost system includes
#include <gloostConfig.h>
#include <gloostMath.h>


/// cpp includes
#include <string>
#include <GL/glew.h>


#include <vector>


namespace mvt
{


  //  Generates and renders a resH x resV mesh - normalized

class ProxyMeshGrid
{
 public:

  // class constructor
  ProxyMeshGrid(unsigned int resH, unsigned int resV);

  // class destructor
  ~ProxyMeshGrid();


  // inits the ProxyMeshGrid
  void init();

  // inits the ProxyMeshGrid
  void inits();

  // draws the ProxyMeshGrid
  void draw(float lod = 1.0 /*1...0, 1 means highest resolution*/);


  // returns horizontal resolution of the grid
  unsigned int getResH () const;
  // returns vertical resolution of the grid
  unsigned int getResV () const;


 protected:

  unsigned int _resH;
  unsigned int _resV;

  GLuint              _displayListHandle;
  std::vector<GLuint> _displayListHandles;
  GLuint       _vboHandle;
  unsigned _numvertices;

};


} // namespace mvt


#endif // H_PMD_PROXY_MESH_GRID


