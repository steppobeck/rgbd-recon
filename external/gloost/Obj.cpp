
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
#include <Obj.h>
#include <Point3.h>
#include <Vector3.h>
#include <Texcoord.h>
#include <Face.h>
#include <Ray.h>
#include <gloostMath.h>



/// cpp includes
#include <GL/glew.h>
#include <GL/glut.h>
#include <string>
#include <iostream>
#include <vector>


namespace gloost
{

///////////////////////////////////////////////////////////////////////////////


  /// class constructor

Obj::Obj():
  MultiGlContext(GLOOST_SYSTEM_NUM_RENDER_CONTEXTS),
  _objFileName("obj_with_no_name.obj"),
  _materialFile(),
  _vertices(),
  _normals(),
  _texcoords(),
  _faces(),
  _facesSortedByMaterial(),
  _perVertexAmbientValues(),
  _dirty(true),
  _boundingBox(new BoundingBox()),
  _enablePretransform(false),
  _pretransformMatrix(),
  _enableCompileWithMaterial(true),
  _enableCompileWithColorMaterial(false),
  _enableUsingPerVertexAmbientFile(true)
{

}

///////////////////////////////////////////////////////////////////////////////


  /// class constructor

Obj::Obj(const std::string& objPath):
  MultiGlContext(GLOOST_SYSTEM_NUM_RENDER_CONTEXTS),
  _objFileName(objPath),
  _materialFile(),
  _vertices(),
  _normals(),
  _texcoords(),
  _faces(),
  _facesSortedByMaterial(),
  _perVertexAmbientValues(),
  _dirty(true),
  _boundingBox(new BoundingBox()),
  _enablePretransform(false),
  _pretransformMatrix(),
  _enableCompileWithMaterial(true),
  _enableCompileWithColorMaterial(false),
  _enableUsingPerVertexAmbientFile(true)
{
  readFile(objPath);
}

///////////////////////////////////////////////////////////////////////////////


  /// class constructor

Obj::Obj(const std::string& objPath, const Matrix& pretransformMatrix):
  MultiGlContext(GLOOST_SYSTEM_NUM_RENDER_CONTEXTS),
  _objFileName(objPath),
  _materialFile(),
  _vertices(),
  _normals(),
  _texcoords(),
  _faces(),
  _facesSortedByMaterial(),
  _perVertexAmbientValues(),
  _dirty(true),
  _boundingBox(new BoundingBox()),
  _enablePretransform(true),
  _pretransformMatrix(pretransformMatrix),
  _enableCompileWithMaterial(true),
  _enableCompileWithColorMaterial(false),
  _enableUsingPerVertexAmbientFile(true)
{


  readFile(objPath);
}


///////////////////////////////////////////////////////////////////////////////


  /// class constructor

Obj::~Obj()
{

	/// delete r
//	glDeleteLists(_raw_display_list, 1);
//	glDeleteLists(_tangent_display_list, 1);

	delete _boundingBox;
}


///////////////////////////////////////////////////////////////////////////////


  ///

/*virtual*/
bool
Obj::initInContext(unsigned int contextId)
{
  return true;
}


///////////////////////////////////////////////////////////////////////////////


  /// remove the geometry from a context

/*virtual*/
void
Obj::removeFromContext(unsigned int contextId)
{

}


///////////////////////////////////////////////////////////////////////////////

  /// enables material from *.mtl file within the display lists

void
Obj::enableComplileWithMaterial(bool value)
{
  _enableCompileWithMaterial = value;
}


///////////////////////////////////////////////////////////////////////////////


  /// enables diffuse color from *.mtl file within the display lists as glColor (don't forget glEnable(GL_COLOR_MATERIAL in you app))

void
Obj::enableComplileWithColorMaterial(bool value)
{
  _enableCompileWithColorMaterial = value;
}


///////////////////////////////////////////////////////////////////////////////


  /// enables the use of ambient values per vertex comming from a <obj_name>.amb file. You can force creation with the second argument

void
Obj::enableUsingPerVertexAmbientFile(bool value, bool forceCreation)
{
  _enableUsingPerVertexAmbientFile = value;


  if (value)
  {



  }


}


///////////////////////////////////////////////////////////////////////////////


  /// draws the object, vertices only

void
Obj::drawRaw(unsigned int contextId)
{

	if (!handleInContextExists(contextId, 0))
	{
	  buildListRaw(contextId);
	}

	glCallList(getResourceHandleForContext(contextId, 0));
}


///////////////////////////////////////////////////////////////////////////////


  /// draws the object with faces, normals and texcoords

void
Obj::drawFacesNormalsTexcoords(unsigned int contextId)
{
  if (!handleInContextExists(contextId, 1))
	{
	  buildListFacesNormalsTexcoords(contextId);
	}

  glCallList(getResourceHandleForContext(contextId, 1));
}


///////////////////////////////////////////////////////////////////////////////


  /// draws the object with with tangent information in the diffuse color of the vertices

void
Obj::drawTangent(unsigned int contextId)
{
  if (!handleInContextExists(contextId, 2))
	{
	  buildListTangent(contextId);
	}

  glCallList(getResourceHandleForContext(contextId, 2));
}


///////////////////////////////////////////////////////////////////////////////


  /// draws the normals of the faces of this geometry

void
Obj::drawFaceNormals(unsigned int contextId)
{
  if (!handleInContextExists(contextId, 3))
	{
	  buildListFaceNormals(contextId);
	}

  glCallList(getResourceHandleForContext(contextId, 3));
}


///////////////////////////////////////////////////////////////////////////////


  /// draws the normals of the vertices of this geometry

void
Obj::drawVertexNormals(unsigned int contextId)
{
  if (!handleInContextExists(contextId, 4))
	{
	  buildListVertexNormals(contextId);
	}

  glCallList(getResourceHandleForContext(contextId, 4));
}


///////////////////////////////////////////////////////////////////////////////


  /// build a display list only out of the vertices

void
Obj::buildListRaw(unsigned int contextId)
{

  unsigned int raw_display_list = glGenLists(1);

  glNewList(raw_display_list, GL_COMPILE);

  glBegin(GL_TRIANGLES);
  for(std::vector<Face>::iterator f = _faces.begin(); f != _faces.end(); ++f)
  {
    unsigned int* tindex = f->getVertexIndices();

    gloostVertex3v(_vertices[tindex[0] - 1].data());
    gloostVertex3v(_vertices[tindex[1] - 1].data());
    gloostVertex3v(_vertices[tindex[2] - 1].data());
  }
  glEnd();
  glEndList();

  setResourceHandleForContext(raw_display_list, contextId, 0);
}


///////////////////////////////////////////////////////////////////////////////


  /// build a displaylist with faces, normals and texcoords

void
Obj::buildListFacesNormalsTexcoords(unsigned int contextId)
{
//  glEnable( GL_AUTO_NORMAL );
//  glEnable( GL_NORMALIZE );

#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_MESSAGES
  std::cout << std::endl;
  std::cout << std::endl << "Message from Obj::buildListFacesNormalsTexcoords on SharedResource " << getSharedResourceId() << ":";
  std::cout << std::endl << "             Building resource 1 for context " << contextId << ".";
  std::cout << std::endl;
  std::flush(std::cout);
#endif



  unsigned int faceCounter = 0;


  unsigned int facesNormalsTexcoords_display_list = glGenLists(1);

  glNewList(facesNormalsTexcoords_display_list, GL_COMPILE);
  {

    std::map<std::string, std::vector<Face> >::iterator faceVectorIt    = _facesSortedByMaterial.begin();
    std::map<std::string, std::vector<Face> >::iterator faceVectorEndIt = _facesSortedByMaterial.end();


    for (; faceVectorIt!=faceVectorEndIt; ++faceVectorIt)
    {
      gloost::Material* currentMaterial = _materialFile.getMaterial(faceVectorIt->first);

      if (currentMaterial)
      {
        if (_enableCompileWithMaterial)
        {
          currentMaterial->setGlMaterial();
        }

        if (_enableCompileWithColorMaterial)
        {
          glColor4fv(currentMaterial->_diffuse);
        }
      }


      Texcoord t1, t2, t3;

      std::vector<Face>::iterator facesIt    = faceVectorIt->second.begin();
      std::vector<Face>::iterator facesEndIt = faceVectorIt->second.end();

      glBegin(GL_TRIANGLES);
      {

        for(; facesIt != facesEndIt; ++facesIt)
        {
          ++faceCounter;
          unsigned int* vindex   = facesIt->getVertexIndices();
          unsigned int* texindex = facesIt->getTexcoordIndices();
          unsigned int* nindex   = facesIt->getNormalIndices();

          if (_texcoords.size() >= texindex[0] - 1)
          {
            t1[0] = _texcoords[texindex[0] - 1].data()[0];
            t1[1] = _texcoords[texindex[0] - 1].data()[1];
          }

          if (_texcoords.size() >= texindex[1] - 1)
          {
            t2[0] = _texcoords[texindex[1] - 1].data()[0];
            t2[1] = _texcoords[texindex[1] - 1].data()[1];
          }

          if (_texcoords.size() >= texindex[2] - 1)
          {
            t3[0] = _texcoords[texindex[2] - 1].data()[0];
            t3[1] = _texcoords[texindex[2] - 1].data()[1];
          }


          /// 1
          gloostNormal3v(_normals[nindex[0] - 1].normalized().data());
          glTexCoord2f(t1[0],t1[1]);

//          glColor3f(_perVertexAmbientValues[vindex[0] - 1],
//                    _perVertexAmbientValues[vindex[0] - 1],
//                    _perVertexAmbientValues[vindex[0] - 1]);

          gloostVertex3v(_vertices[vindex[0] - 1].data());



          /// 2
          gloostNormal3v(_normals[nindex[1] - 1].normalized().data());
          glTexCoord2f(t2[0],t2[1]);

//          glColor3f(_perVertexAmbientValues[vindex[1] - 1],
//                    _perVertexAmbientValues[vindex[1] - 1],
//                    _perVertexAmbientValues[vindex[1] - 1]);

          gloostVertex3v(_vertices[vindex[1] - 1].data());

          /// 3
          gloostNormal3v(_normals[nindex[2] - 1].normalized().data());
          glTexCoord2f(t3[0],t3[1]);

//          glColor3f(_perVertexAmbientValues[vindex[2] - 1],
//                    _perVertexAmbientValues[vindex[2] - 1],
//                    _perVertexAmbientValues[vindex[2] - 1]);

          gloostVertex3v(_vertices[vindex[2] - 1].data());
        }
      }
      glEnd();
    }
  }
  glEndList();

//  glDisable( GL_NORMALIZE );
//  glDisable( GL_AUTO_NORMAL );

  setResourceHandleForContext(facesNormalsTexcoords_display_list, contextId, 1);



#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_MESSAGES
  std::cout << std::endl;
  std::cout << std::endl << "Message from Obj::buildListFacesNormalsTexcoords on SharedResource " << getSharedResourceId() << ":";
  std::cout << std::endl << "             Building resource 1 for context " << contextId << " complete";
  std::cout << std::endl << "             Faces: " << faceCounter;
  std::cout << std::endl;
  std::flush(std::cout);
#endif





}


///////////////////////////////////////////////////////////////////////////////


  /// build a displaylist with tangent information in the diffuse color of the vertices

void
Obj::buildListTangent(unsigned int contextId)
{

  unsigned int tangent_display_list = glGenLists(1);

  glNewList(tangent_display_list, GL_COMPILE);
  struct vec p1, p2, p3, tangent;
  struct tex t1, t2, t3;
  glBegin(GL_TRIANGLES);
  for(std::vector<Face>::iterator f = _faces.begin(); f != _faces.end(); ++f)
  {

    unsigned int* tindex   = f->getVertexIndices();
    unsigned int* texindex = f->getTexcoordIndices();
    unsigned int* nindex   = f->getNormalIndices();


    p1.x = _vertices[tindex[0] - 1].data()[0];
    p1.y = _vertices[tindex[0] - 1].data()[1];
    p1.z = _vertices[tindex[0] - 1].data()[2];

    if (_texcoords.size() >= texindex[0] - 1)
    {
      t1.u = _texcoords[texindex[0] - 1].data()[0];
      t1.v = _texcoords[texindex[0] - 1].data()[1];
    }

    p2.x = _vertices[tindex[1] - 1].data()[0];
    p2.y = _vertices[tindex[1] - 1].data()[1];
    p2.z = _vertices[tindex[1] - 1].data()[2];

    if (_texcoords.size() >= texindex[1] - 1)
    {
      t2.u = _texcoords[texindex[1] - 1].data()[0];
      t2.v = _texcoords[texindex[1] - 1].data()[1];
    }


    p3.x = _vertices[tindex[2] - 1].data()[0];
    p3.y = _vertices[tindex[2] - 1].data()[1];
    p3.z = _vertices[tindex[2] - 1].data()[2];

    if (_texcoords.size() >= texindex[2] - 1)
    {
      t3.u = _texcoords[texindex[2] - 1].data()[0];
      t3.v = _texcoords[texindex[2] - 1].data()[1];
    }

    calcTangent(p1,p2,p3,t1,t2,t3,&tangent);

    gloostNormal3v(_normals[nindex[0] - 1].data());
    glColor3f(tangent.x,tangent.y,tangent.z);
    glTexCoord2f(t1.u,t1.v);
    glVertex3f(p1.x,p1.y,p1.z);

    gloostNormal3v(_normals[nindex[1] - 1].data());
    glColor3f(tangent.x,tangent.y,tangent.z);
    glTexCoord2f(t2.u,t2.v);
    glVertex3f(p2.x,p2.y,p2.z);

    gloostNormal3v(_normals[nindex[2] - 1].data());
    glColor3f(tangent.x,tangent.y,tangent.z);
    glTexCoord2f(t3.u,t3.v);
    glVertex3f(p3.x,p3.y,p3.z);

  }
  glEnd();
  glEndList();



  setResourceHandleForContext(tangent_display_list, contextId, 2);

}



///////////////////////////////////////////////////////////////////////////////


  /// build a displaylist with tangent information in the diffuse color of the vertices
//
//void
//Obj::buildListTangent()
//{
//  if (_tangent_display_list)
//  {
//    glDeleteLists(_tangent_display_list, 1);
//  }
//
//  _tangent_display_list = glGenLists(1);
//  glNewList(_tangent_display_list, GL_COMPILE);
//
//  Point3   p1, p2, p3;
//  Vector3  tangent;
//  Texcoord t1, t2, t3;
//
//  glBegin(GL_TRIANGLES);
//  {
//    for(std::vector<Face>::iterator f = _faces.begin(); f != _faces.end(); ++f)
//    {
//
//      unsigned int* tindex   = f->getVertexIndices();
//      unsigned int* texindex = f->getTexcoordIndices();
//      unsigned int* nindex   = f->getNormalIndices();
//
//
//      p1[0] = _vertices[tindex[0] - 1].data()[0];
//      p1[1] = _vertices[tindex[0] - 1].data()[1];
//      p1[2] = _vertices[tindex[0] - 1].data()[2];
//
//      t1[0] = _texcoords[texindex[0] - 1].data()[0];
//      t1[1] = _texcoords[texindex[0] - 1].data()[1];
//
//
//      p2[0] = _vertices[tindex[1] - 1].data()[0];
//      p2[1] = _vertices[tindex[1] - 1].data()[1];
//      p2[2] = _vertices[tindex[1] - 1].data()[2];
//
//      t2[0] = _texcoords[texindex[1] - 1].data()[0];
//      t2[1] = _texcoords[texindex[1] - 1].data()[1];
//
//
//      p3[0] = _vertices[tindex[2] - 1].data()[0];
//      p3[1] = _vertices[tindex[2] - 1].data()[1];
//      p3[2] = _vertices[tindex[2] - 1].data()[2];
//
//      t3[0] = _texcoords[texindex[2] - 1].data()[0];
//      t3[1] = _texcoords[texindex[2] - 1].data()[1];
//
//
//      tangent = calcTangent(p1, p2, p3, t1, t2, t3);
//
////      std::cout << std::endl << tangent;
//
//      glNormal3dv(_normals[nindex[0] - 1].data());
//      glColor3f(tangent[0],tangent[1],tangent[2]);
//      glTexCoord2f(t1[0],t1[1]);
//      glVertex3f(p1[0],p1[1],p1[2]);
//
//      glNormal3dv(_normals[nindex[1] - 1].data());
//      glColor3f(tangent[0],tangent[1],tangent[2]);
//      glTexCoord2f(t2[0],t2[1]);
//      glVertex3f(p2[0],p2[1],p2[2]);
//
//      glNormal3dv(_normals[nindex[2] - 1].data());
//      glColor3f(tangent[0],tangent[1],tangent[2]);
//      glTexCoord2f(t3[0],t3[1]);
//      glVertex3f(p3[0],p3[1],p3[2]);
//
//    }
//    glEnd();
//  }
//  glEndList();
//}


///////////////////////////////////////////////////////////////////////////////


  /// build a displaylist with the normals of this obj

void
Obj::buildListFaceNormals(unsigned int contextId)
{
  unsigned int face_normals_display_list = glGenLists(1);

  glNewList(face_normals_display_list, GL_COMPILE);

  glBegin(GL_LINES);
  for(std::vector<Face>::iterator f = _faces.begin(); f != _faces.end(); ++f)
  {
    unsigned int* tindex = f->getVertexIndices();


    Vector3 middlePosition = _vertices[tindex[0] - 1] + _vertices[tindex[1] - 1] + _vertices[tindex[2] - 1];
    middlePosition = middlePosition / 3.0;

    Vector3 middleNormal   = _normals[tindex[0] - 1]  + _normals[tindex[1] - 1]  + _normals[tindex[2] - 1];
    middleNormal.normalize();
    middleNormal = middleNormal.scalar(0.01);
    Vector3 endPosition    = middlePosition + middleNormal;
    //std::cout << std::endl << " middleNormal.length(): " << middleNormal.length();

    gloostVertex3v(middlePosition.data());
    gloostVertex3v(endPosition.data());

  }
  glEnd();
  glEndList();


  setResourceHandleForContext(face_normals_display_list, contextId, 3);

}


///////////////////////////////////////////////////////////////////////////////


  /// build a displaylist with the normals of the vertices

void
Obj::buildListVertexNormals(unsigned int contextId)
{

  unsigned int vertex_normals_display_list = glGenLists(1);

  glNewList(vertex_normals_display_list, GL_COMPILE);

  glBegin(GL_LINES);
  for(unsigned int i=0; i!=_vertices.size(); ++i)
  {
    Vector3 position = _vertices[i];

    Vector3 normal   = _normals[i];
    normal.normalize();
    normal = normal.scalar(0.01);
    Vector3 endPosition    = position + normal;

    gloostVertex3v(position.data());
    gloostVertex3v(endPosition.data());

  }
  glEnd();
  glEndList();


  setResourceHandleForContext(vertex_normals_display_list, contextId, 4);
}


///////////////////////////////////////////////////////////////////////////////


  /// get the vector of vertices

std::vector<Point3>&
Obj::getVertices()
{
  return _vertices;
}

///////////////////////////////////////////////////////////////////////////////


  /// get the vector of normals

std::vector<Vector3>&
Obj::getNormals()
{
  return _normals;
}


///////////////////////////////////////////////////////////////////////////////


  /// get the vector texcoords

std::vector<Texcoord>&
Obj::getTexcoords()
{
  return _texcoords;
}


///////////////////////////////////////////////////////////////////////////////


  /// get the vector with a ambient value per vertex

std::vector<float>&
Obj::getPerVertexAmbientValues()
{
  return _perVertexAmbientValues;
}


///////////////////////////////////////////////////////////////////////////////


  /// get the vector of faces

std::vector<Face>&
Obj::getFaces()
{
  return _faces;
}


///////////////////////////////////////////////////////////////////////////////


  /// build a display list from the obj file

bool
Obj::readFile(const std::string& objPath)
{
  std::ifstream infile;
  infile.open(objPath.c_str());

  if (!infile)
  {

#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_ERRORS
    std::cout << std::endl;
    std::cout << std::endl << "ERROR in Obj::readFile on SharedResource " << getSharedResourceId() << ":";
    std::cout << std::endl << "         Cant open \"" << objPath << "\"!";
    std::cout << std::endl;
    std::flush(std::cout);
#endif

    return 0;
  }


#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_MESSAGES
  std::cout << std::endl;
  std::cout << std::endl << "Message from Obj::readFile on SharedResource " << getSharedResourceId() << ":";
  std::cout << std::endl << "             Loading \"" << objPath << "\".";
  std::cout << std::endl;
  std::flush(std::cout);
#endif


  /// parse the obj file structure
  std::string token;
  std::string currentMaterialName = "gloost_default_material";

  // create a default material
  Material* gloost_default_material = new Material();
  gloost_default_material->setAmbient (0.2, 0.2, 0.2, 1.0);
  gloost_default_material->setDiffuse (0.6, 0.6, 0.6, 1.0);
  gloost_default_material->setSpecular(1.0, 1.0, 1.0, 1.0);
  gloost_default_material->setNs(120.0);

  _materialFile.addMaterial(currentMaterialName, gloost_default_material);


  while(infile >> token)
  {

    if("v" == token)
    {
      Point3 v(infile);
      _vertices.push_back(v);
      _perVertexAmbientValues.push_back(-1);
    }
    else if(token == "vn")
    {
      Vector3 n(infile);
      n.normalize();
      _normals.push_back(n);
    }
    else if(token == "vt")
    {
      Texcoord vt(infile);
      _texcoords.push_back(vt);
    }
    else if("f" == token)
    {
      Face face(infile);
      _faces.push_back(face);
      _facesSortedByMaterial[currentMaterialName].push_back(face);
    }
    else if("usemtl" == token)
    {
      infile >> currentMaterialName;
    }
    else if("mtllib" == token)
    {
      std::string mtlFileName;

      infile >> mtlFileName;

      /// load naterial
      std::string materialFilePath = gloost::pathToBasePath(objPath) + mtlFileName;
      _materialFile.load(materialFilePath);
    }
  }

  infile.close();




  /// transform all vertices and normals here befor they are compiled to a displaylist
  if (_enablePretransform)
  {

    // vertices
    for (unsigned int i=0; i!=_vertices.size(); ++i)
    {
      _vertices[i] = _pretransformMatrix * _vertices[i];
    }

    // normals
    for (unsigned int i=0; i!=_normals.size(); ++i)
    {
      _normals[i] = _pretransformMatrix * _normals[i];
    }
  }


  /// recalc the bounding box
  recalcBoundingBox();



#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_MESSAGES
  std::cout << std::endl;
  std::cout << std::endl << "Message from Obj::readFile on SharedResource " << getSharedResourceId() << ":";
  std::cout << std::endl << "             \"" << objPath << "\" succesfully loaded";
  std::cout << std::endl << "               vertices: ....... " << _vertices.size();
  std::cout << std::endl << "               normals:  ....... " << _normals.size();
  std::cout << std::endl << "               texcoords: ...... " << _texcoords.size();
  std::cout << std::endl << "               faces: .......... " << _faces.size();
  std::cout << std::endl << "               Bounding Box Min: " << _boundingBox->getPMin();
  std::cout << std::endl << "               Bounding Box Max: " << _boundingBox->getPMax();

  if (_enablePretransform)
  {
    std::cout << std::endl << "               pretransform: ... YES";
  }
  else
  {
    std::cout << std::endl << "               pretransform: ... NO";
  }
  std::cout << std::endl;
  std::flush(std::cout);
#endif





  /// loading ambient value file if there is one
  std::string objBaseName           = gloost::pathToBasename(objPath) + ".vertex_ambient";
  std::string ambientValuesFilePath = gloost::pathToBasePath(objPath) + objBaseName;

  std::ifstream ambientFile;
  ambientFile.open(ambientValuesFilePath.c_str());


  if (ambientFile)
  {
#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_MESSAGES
    std::cout << std::endl;
    std::cout << std::endl << "Message from Obj::readFile on SharedResource " << getSharedResourceId() << ":";
    std::cout << std::endl << "             Loading per vertex ambient values from";
    std::cout << std::endl << "             \"" << ambientValuesFilePath << "\".";
    std::cout << std::endl;
    std::flush(std::cout);
#endif


    float        oneValue;
    unsigned int index = 0;
    while(ambientFile >> oneValue)
    {
      _perVertexAmbientValues[index] = gloost::abs(oneValue);
      ++index;
    }


    /// does the number of values and the number of vertices match ???
    if (index != _vertices.size())
    {
#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_ERRORS
      std::cout << std::endl;
      std::cout << std::endl << "ERROR in Obj::readFile on SharedResource " << getSharedResourceId() << ":";
      std::cout << std::endl << "             Number of ambient values (" << index <<  ") in";
      std::cout << std::endl << "             \"" << ambientValuesFilePath << "\".";
      std::cout << std::endl << "             with the number of vertices of the Obj!";
      std::cout << std::endl;
      std::flush(std::cout);
#endif
    }

  }


  ambientFile.close();





  return true;

}



///////////////////////////////////////////////////////////////////////////////


/// setDirty so the class will rebuild the display list

void
Obj::setDirty()
{
  _dirty = true;
}


///////////////////////////////////////////////////////////////////////////////


/// returns TRUE if the object is not uptodate

bool
Obj::isDirty()
{
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////


  /// returns BoundingBox of this geometry

BoundingVolume*
Obj::getBoundingVolume()
{
  return _boundingBox;
}


///////////////////////////////////////////////////////////////////////////////


  /// recalculate BoundingBox

void
Obj::recalcBoundingBox()
{
  Point3 min(10000000, 10000000, 10000000);
  Point3 max(-10000000, -10000000, -10000000);

  for(unsigned int i=0; i!=_vertices.size(); ++i)
  {
    if (_vertices[i][0] < min[0]) min[0] = _vertices[i][0];
    if (_vertices[i][1] < min[1]) min[1] = _vertices[i][1];
    if (_vertices[i][2] < min[2]) min[2] = _vertices[i][2];

    if (_vertices[i][0] > max[0]) max[0] = _vertices[i][0];
    if (_vertices[i][1] > max[1]) max[1] = _vertices[i][1];
    if (_vertices[i][2] > max[2]) max[2] = _vertices[i][2];
  }

  _boundingBox->setPMin(min);
  _boundingBox->setPMax(max);

}




///////////////////////////////////////////////////////////////////////////////


/*
    (v3 - v1).(p2 - p1) - (v2 - v1).(p3 - p1)
T = ---------------------------------------
    (u2 - u1).(v3 - v1) - (v2 - v1).(u3 - u1)
*/

/// calculate tangent information (for tangent space bump mapping for example)
//
//Vector3
//Obj::calcTangent(const Point3&   p1, const Point3&   p2, const Point3&   p3,
//                 const Texcoord& t1, const Texcoord& t2, const Texcoord& t3)
//{
//  const float dv3v1 = t3[1] - t1[1];
//  const float dv2v1 = t2[1] - t1[1];
//  const float du3u1 = t3[0] - t1[0];
//  const float du2u1 = t2[0] - t1[0];
//  const float den = du2u1*dv3v1 - dv2v1*du3u1;
//
//  Vector3 tangentVector;
//
//  tangentVector[0] = ((dv3v1*(p2[0]-p1[0]) - dv2v1*(p3[0]-p1[0]))/den)*0.5 + 0.5;
//  tangentVector[1] = ((dv3v1*(p2[1]-p1[1]) - dv2v1*(p3[1]-p1[1]))/den)*0.5 + 0.5;
//  tangentVector[2] = ((dv3v1*(p2[2]-p1[2]) - dv2v1*(p3[2]-p1[2]))/den)*0.5 + 0.5;
//
//  return tangentVector;
//}





///////////////////////////////////////////////////////////////////////////////


  //

/*
    (v3 - v1).(p2 - p1) - (v2 - v1).(p3 - p1)
T = ---------------------------------------
    (u2 - u1).(v3 - v1) - (v2 - v1).(u3 - u1)
*/

void
Obj::calcTangent(struct vec p1, struct vec p2, struct vec p3,
	               struct tex t1, struct tex t2, struct tex t3,
	               struct vec* tangent)
{
  const float dv3v1 = t3.v - t1.v;
  const float dv2v1 = t2.v - t1.v;
  const float du3u1 = t3.u - t1.u;
  const float du2u1 = t2.u - t1.u;
  const float den = du2u1*dv3v1 - dv2v1*du3u1;
  tangent->x = ((dv3v1*(p2.x-p1.x) - dv2v1*(p3.x-p1.x))/den)*0.5 + 0.5;
  tangent->y = ((dv3v1*(p2.y-p1.y) - dv2v1*(p3.y-p1.y))/den)*0.5 + 0.5;
  tangent->z = ((dv3v1*(p2.z-p1.z) - dv2v1*(p3.z-p1.z))/den)*0.5 + 0.5;

}


} // namespace gloost

