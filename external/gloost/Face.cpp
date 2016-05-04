
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
#include <Face.h>



/// cpp includes
#include <stdio.h>
#include <stdlib.h>
#include <iostream>


namespace gloost
{


Face::Face(unsigned int* vertices, unsigned int* texcoords, unsigned int* normals):
    m_vertices(),
    m_texcoords(),
    m_normals()
{
  m_vertices[0] = vertices[0];
  m_vertices[1] = vertices[1];
  m_vertices[2] = vertices[2];

  m_texcoords[0] = texcoords[0];
  m_texcoords[1] = texcoords[1];
  m_texcoords[2] = texcoords[2];

  m_normals[0] = normals[0];
  m_normals[1] = normals[1];
  m_normals[2] = normals[2];
}


////////////////////////////////////////////////////////////////////////////////


Face::Face(unsigned int vertex1, unsigned int vertex2, unsigned int vertex3,
           unsigned int texcoord1, unsigned int texcoord2, unsigned int texcoord3,
           unsigned int normal1, unsigned int normal2, unsigned int normal3):
    m_vertices(),
    m_texcoords(),
    m_normals()
{
  m_vertices[0] = vertex1;
  m_vertices[1] = vertex2;
  m_vertices[2] = vertex3;

  m_texcoords[0] = texcoord1;
  m_texcoords[1] = texcoord2;
  m_texcoords[2] = texcoord3;

  m_normals[0] = normal1;
  m_normals[1] = normal2;
  m_normals[2] = normal3;
}


////////////////////////////////////////////////////////////////////////////////

Face::Face(std::ifstream& in)
  : m_vertices(),
    m_texcoords(),
    m_normals()
{
  // 12106/72580/72580 8096/72581/72581 12105/72582/72582
  // int sscanf(const char *str, const char *format, ...);
  std::string token;
  in >> token;

  unsigned int num_slashes = 0;
  unsigned int first = 0;


  for(unsigned int i = 0; i < token.size(); ++i)
  {
    if('/' == token[i])
    {
      if(first == 0)
	first = i;
      if(first == (i - 1))
      {
	num_slashes = 5;
	break;
      }
      ++num_slashes;
    }
  }

  switch (num_slashes){
  case 0:
    {
      std::string format = "%d";
      if(!sscanf(token.c_str(),format.c_str(),&m_vertices[0]))
        {
          std::cerr << "ERROR in" << __FILE__ << __LINE__ << " ...exiting!" << std::endl;
          exit(-1);
        }

      in >> token;
      if(!sscanf(token.c_str(),format.c_str(),&m_vertices[1]))
        {
          std::cerr << "ERROR in" << __FILE__ << __LINE__ << " ...exiting!" << std::endl;
          exit(-1);
        }

      in >> token;
      if(!sscanf(token.c_str(),format.c_str(),&m_vertices[2]))
        {
          std::cerr << "ERROR in" << __FILE__ << __LINE__ << " ...exiting!" << std::endl;
          exit(-1);
        }
    }

    break;
  case 1:
    {
      std::string format = "%d/%d";
      if(!sscanf(token.c_str(),format.c_str(),&m_vertices[0],&m_normals[0]))
        {
          std::cerr << "ERROR in" << __FILE__ << __LINE__ << " ...exiting!" << std::endl;
          exit(-1);
        }

      in >> token;
      if(!sscanf(token.c_str(),format.c_str(),&m_vertices[1],&m_normals[1]))
        {
          std::cerr << "ERROR in" << __FILE__ << __LINE__ << " ...exiting!" << std::endl;
          exit(-1);
        }

      in >> token;
      if(!sscanf(token.c_str(),format.c_str(),&m_vertices[2],&m_normals[2]))
        {
          std::cerr << "ERROR in" << __FILE__ << __LINE__ << " ...exiting!" << std::endl;
          exit(-1);
        }

    }
    break;

  case 2:
    {
      std::string format = "%d/%d/%d";
      if(!sscanf(token.c_str(),format.c_str(),&m_vertices[0],&m_texcoords[0],&m_normals[0]))
        {
          std::cerr << "ERROR in" << __FILE__ << __LINE__ << " ...exiting!" << std::endl;
          exit(-1);
        }

      in >> token;
      if(!sscanf(token.c_str(),format.c_str(),&m_vertices[1],&m_texcoords[1],&m_normals[1]))
        {
          std::cerr << "ERROR in" << __FILE__ << __LINE__ << " ...exiting!" << std::endl;
          exit(-1);
        }

      in >> token;
      if(!sscanf(token.c_str(),format.c_str(),&m_vertices[2],&m_texcoords[2],&m_normals[2]))
        {
          std::cerr << "ERROR in" << __FILE__ << __LINE__ << " ...exiting!" << std::endl;
          exit(-1);
        }
    }
    break;
  case 5:
    {
      std::string format = "%d//%d";
      if(!sscanf(token.c_str(),format.c_str(),&m_vertices[0],&m_normals[0]))
        {
          std::cerr << "ERROR in" << __FILE__ << __LINE__ << " ...exiting!" << std::endl;
          exit(-1);
        }

      in >> token;
      if(!sscanf(token.c_str(),format.c_str(),&m_vertices[1],&m_normals[1]))
        {
          std::cerr << "ERROR in" << __FILE__ << __LINE__ << " ...exiting!" << std::endl;
          exit(-1);
        }

      in >> token;
      if(!sscanf(token.c_str(),format.c_str(),&m_vertices[2],&m_normals[2]))
        {
          std::cerr << "ERROR in" << __FILE__ << __LINE__ << " ...exiting!" << std::endl;
          exit(-1);
        }
    }
    default:
      break;

  }
}


///////////////////////////////////////////////////////////////////////////////


Face::~Face()
{}


///////////////////////////////////////////////////////////////////////////////


unsigned int*
Face::getVertexIndices()
{
  return m_vertices;
}


///////////////////////////////////////////////////////////////////////////////


unsigned int*
Face::getTexcoordIndices()
{
  return m_texcoords;
}


///////////////////////////////////////////////////////////////////////////////


unsigned int*
Face::getNormalIndices()
{
  return m_normals;
}


///////////////////////////////////////////////////////////////////////////////


void
Face::dump() const
{
  std::cout << "face: "
	    << m_vertices[0]
	    << "/"
	    << m_texcoords[0]
	    << "/"
	    << m_normals[0]
	    << " "
	    << m_vertices[1]
	    << "/"
	    << m_texcoords[1]
	    << "/"
	    << m_normals[1]
	    << " "
	    << m_vertices[2]
	    << "/"
	    << m_texcoords[2]
	    << "/"
	    << m_normals[2]
	    << std::endl;
}


} // namespace gloost

