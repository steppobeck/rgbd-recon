
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



#ifndef H_GLOOST_OBJ
#define H_GLOOST_OBJ


#define FLOAT_SIZE_OF_TRIANGLES 9



/// gloost includes
#include <MultiGlContext.h>
#include <Vector3.h>
#include <Point3.h>
#include <Ray.h>
#include <Texcoord.h>
#include <Face.h>
#include <BoundingBox.h>
#include <ObjMatFile.h>


/// cpp includes
#include <string>
#include <vector>
#include <glbinding/gl/gl.h>
using namespace gl;



namespace gloost
{


/// 3d Vector for tangen space calculation

struct vec
{
  float x;
  float y;
  float z;
};


/// texture coordinate for tangen space calculation
struct tex
{
  float u;
  float v;
};

//
//
///// resource positions within the MultiGlContext
#define GLOOST_OBJ_DRAWMODE_RAW                     1
#define GLOOST_OBJ_DRAWMODE_FACES_NORMALS_TEXCOORDS 2
#define GLOOST_OBJ_DRAWMODE_TANGENT                 4
#define GLOOST_OBJ_DRAWMODE_FACE_NORMALS            8
#define GLOOST_OBJ_DRAWMODE_VERTEX_NORMALS          16





//////////////////////////////////////////////////////////////////////////////////////////


  ///  OBJ file loader, draws obj with texCoord or with texCoords as color

class Obj : public MultiGlContext
{
	public:

    /// class constructor
    Obj();

    /// class constructor
    Obj(const std::string& filename);

    /// class constructor
    Obj(const std::string& filename, const Matrix& pretransformMatrix);

    /// class destructor
	  ~Obj();



    /*virtual*/ bool initInContext(unsigned int contextId = 0);

	  /// remove the geometry from a context
	  /*virtual*/ void removeFromContext(unsigned int contextId = 0);



    /// enables material from *.mtl file within the display lists (default: true)
    void enableComplileWithMaterial(bool value = true);

    /// enables diffuse color from *.mtl file within the display lists as glColor (don't forget glEnable(GL_COLOR_MATERIAL in you app)) (default: false)
    void enableComplileWithColorMaterial(bool value = true);

    /// enables the use of ambient values per vertex comming from a <obj_name>.amb file. You can force creation with the second argument
    void enableUsingPerVertexAmbientFile(bool value = true, bool forceCreation = false);



	  /// draws the object, vertices only
	  void drawRaw(unsigned int contextId = 0);

	  /// draws the object with faces, normals and texcoords
	  void drawFacesNormalsTexcoords(unsigned int contextId = 0);

	  /// draws the object with with tangent information in the diffuse color of the vertices
	  void drawTangent(unsigned int contextId = 0);

	  /// draws the normals of the faces of this geometry
	  void drawFaceNormals(unsigned int contextId = 0);

	  /// draws the normals of the vertices of this geometry
	  void drawVertexNormals(unsigned int contextId = 0);



	  /// get the vector of vertices
	  std::vector<Point3>& getVertices();

	  /// get the vector of normals
	  std::vector<Vector3>& getNormals();

	  /// get the vector texcoords
	  std::vector<Texcoord>& getTexcoords();

	  /// get the vector of faces
	  std::vector<Face>& getFaces();

	  /// get the vector of faces sorted by Material
//	  std::map<std::string, std::vector<Face> >& getFacesSortedByMaterial();

	  /// get the vector with a ambient value per vertex (uses same indexing as the vertex vector)
	  std::vector<float>& getPerVertexAmbientValues();



    /// setDirty so the class will rebuild the display list
    void setDirty();

    /// returns TRUE if the object is not uptodate
    bool isDirty();


    /// returns BoundingVolume (which is a Box) of this geometry
    BoundingVolume* getBoundingVolume();

    /// recalculate BoundingBox
    void recalcBoundingBox();


	private:

    /// build a display list only out of the vertices
	  void buildListRaw(unsigned int contextId = 0);

    /// build a displaylist with faces, normals and texcoords
	  void buildListFacesNormalsTexcoords(unsigned int contextId = 0);

    /// build a displaylist with tangent information in the diffuse color of the vertices
	  void buildListTangent(unsigned int contextId = 0);

    /// build a displaylist with the normals of this obj
	  void buildListFaceNormals(unsigned int contextId = 0);

    /// build a displaylist with the normals of this obj
	  void buildListVertexNormals(unsigned int contextId = 0);




    /// calculates tangents for each vertex
    void calcTangent(struct vec p1, struct vec p2, struct vec p3,
	                   struct tex t1, struct tex t2, struct tex t3,
	                   struct vec* tangent);



    /// reads a *.obj file
    bool readFile(const std::string& objPath);



    std::string _objFileName;

    ObjMatFile _materialFile;


    /// vector of vertices
    std::vector<Point3>    _vertices;
    /// vector of normals
    std::vector<Vector3>   _normals;
    /// vector of texcoords
    std::vector<Texcoord>  _texcoords;
    /// vector of texcoords
    std::vector<Face>      _faces;
    /// vector of faces
    std::map<std::string, std::vector<Face> > _facesSortedByMaterial;
    /// vector of vertex ambient occlusion values (uses same indexing as the vertex vector)
    std::vector<float>                        _perVertexAmbientValues;

    bool _dirty;

    BoundingBox* _boundingBox;



    /// flag, indicating that the vertex and normal data will be transformed with a matrix after loading
    bool   _enablePretransform;

    /// matrix to transform the vertex and normal data after loading
    Matrix _pretransformMatrix;


    /// flag indicating that the material from the *.mtl file should be used when compiling the display list
    bool _enableCompileWithMaterial;
    /// flag indicating that the material from the *.mtl file should be used as glColor4... when compiling the display list
    bool _enableCompileWithColorMaterial;
    /// flag indicating that a per vertex ambien data file should be used when compiling the display list
    bool _enableUsingPerVertexAmbientFile;
};


} // namespace gloost


#endif // GLOOST_NEWOBJ_H

