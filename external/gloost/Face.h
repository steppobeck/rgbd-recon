
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



#ifndef GLOOST_FACE_H
#define GLOOST_FACE_H



/// cpp includes
#include <fstream>
#include <string>


namespace gloost
{

  /// Triangle face

class Face
{

 public:

  /// constructor
  Face(unsigned int* vertices, unsigned int* texcoords, unsigned int* normals);

  Face(unsigned int vertex1, unsigned int vertex2, unsigned int vertex3,
       unsigned int texcoord1, unsigned int texcoord2, unsigned int texcoord3,
       unsigned int normal1, unsigned int normal2, unsigned int normal3);

  /// constructor reads obj file face declaration like "f 4/4/4 3/1/1 1/3/3"
  Face(std::ifstream& in);
  ~Face();

  unsigned int* getVertexIndices();

  unsigned int* getTexcoordIndices();

  unsigned int* getNormalIndices();


  void dump() const;
 private:
  unsigned int m_vertices[3];
  unsigned int m_texcoords[3];
  unsigned int m_normals[3];
};


} // namespace gloost

#endif // #ifndef GLOOST_FACE_H
