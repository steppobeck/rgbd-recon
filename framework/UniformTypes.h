
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



/*
  This is a nearly complete set of GLSL uniform types
  Nice overview can be found at: http://wiki.delphigl.com/index.php/glUniform
*/



#ifndef GLOOST_UNIFORMTYPES_H
#define GLOOST_UNIFORMTYPES_H



/// gloost system includes
#include <gloostHelper.h>
#include <Point3.h>
#include <Vector3.h>
#include <Matrix.h>



/// cpp includes
#include <iostream>



namespace gloost
{


  ///  glsl vec2

struct vec2
{
  vec2()
  {
    u = 0.0;
    v = 0.0;
  }


  vec2(float u_value, float v_value)
  {
    u = u_value;
    v = v_value;
  }


  vec2(std::ifstream& in)
  {
    in >> u;
    in >> v;
  }


  float u;
  float v;
};


//////////////////////////////////////////////////////////////////////////////////////////


extern std::ostream& operator<< (std::ostream&, const vec2&);



inline const std::string operator+ (const std::string& s, const gloost::vec2& v2)
{
  return s + gloost::toString(v2);
}

inline const std::string operator+ (const gloost::vec2& v2, const std::string& s)
{
  return  gloost::toString(v2) + s;
}

//////////////////////////////////////////////////////////////////////////////////////////


  ///  glsl vec3

struct vec3
{
  vec3()
  {
    x = 0.0;
    y = 0.0;
    z = 0.0;
  }

  vec3(float x_value, float y_value, float z_value)
  {
    x = x_value;
    y = y_value;
    z = z_value;
  }

  vec3(const Vector3& vector)
  {
    x = vector[0];
    y = vector[1];
    z = vector[2];
  }

  vec3(const Point3& vector)
  {
    x = vector[0];
    y = vector[1];
    z = vector[2];
  }


  vec3(std::ifstream& in)
  {
    in >> x;
    in >> y;
    in >> z;
  }


  float x;
  float y;
  float z;
};


//////////////////////////////////////////////////////////////////////////////////////////


extern std::ostream& operator<< (std::ostream&, const vec3&);


//////////////////////////////////////////////////////////////////////////////////////////


  ///  glsl vec4

struct vec4
{
  vec4()
  {
    r = 0.0;
    g = 0.0;
    b = 0.0;
    a = 0.0;
  }


  vec4(float r_value, float g_value, float b_value, float a_value)
  {
    r = r_value;
    g = g_value;
    b = b_value;
    a = a_value;
  }


  vec4(std::ifstream& in)
  {
    in >> r;
    in >> g;
    in >> b;
    in >> a;
  }


  float r;
  float g;
  float b;
  float a;
};


//////////////////////////////////////////////////////////////////////////////////////////


extern std::ostream& operator<< (std::ostream&, const vec4&);


//////////////////////////////////////////////////////////////////////////////////////////


  ///  glsl ivec2

struct ivec2
{
  ivec2()
  {
    u = 0;
    v = 0;
  }


  ivec2(int u_value, int v_value)
  {
    u = u_value;
    v = v_value;
  }


  int u;
  int v;
};


//////////////////////////////////////////////////////////////////////////////////////////


  ///  glsl ivec3

struct ivec3
{
  ivec3()
  {
    x = 0;
    y = 0;
    z = 0;
  }


  ivec3(int x_value, int y_value, int z_value)
  {
    x = x_value;
    y = y_value;
    z = z_value;
  }


  int x;
  int y;
  int z;

};


//////////////////////////////////////////////////////////////////////////////////////////


  ///  glsl vec4

struct ivec4
{
  ivec4()
  {
    r = 0;
    g = 0;
    b = 0;
    a = 0;
  }


  ivec4(int r_value, int g_value, int b_value, int a_value)
  {
    r = r_value;
    g = g_value;
    b = b_value;
    a = a_value;
  }


  int r;
  int g;
  int b;
  int a;
};
//////////////////////////////////////////////////////////////////////////////////////////


  ///  glsl bvec2

struct bvec2
{
  bvec2()
  {
    u = 0;
    v = 0;
  }


  bvec2(bool u_value, bool v_value)
  {
    u = u_value;
    v = v_value;
  }


  bool u;
  bool v;
};


//////////////////////////////////////////////////////////////////////////////////////////


  ///  glsl bvec3

struct bvec3
{
  bvec3()
  {
    x = 0;
    y = 0;
    z = 0;
  }


  bvec3(bool x_value, bool y_value, bool z_value)
  {
    x = x_value;
    y = y_value;
    z = z_value;
  }


  bool x;
  bool y;
  bool z;

};


//////////////////////////////////////////////////////////////////////////////////////////


  ///  glsl vec4

struct bvec4
{
  bvec4()
  {
    r = 0;
    g = 0;
    b = 0;
    a = 0;
  }


  bvec4(bool r_value, bool g_value, bool b_value, bool a_value)
  {
    r = r_value;
    g = g_value;
    b = b_value;
    a = a_value;
  }


  bool r;
  bool g;
  bool b;
  bool a;
};


//////////////////////////////////////////////////////////////////////////////////////////


  ///  glsl mat2

struct mat2
{
  mat2()
  {
    data[0] = 1.0;
    data[1] = 0.0;
    data[2] = 0.0;
    data[3] = 1.0;
  }


  mat2(float a1_value, float a2_value, float b1_value, float b2_value)
  {
    data[0] = a1_value;
    data[1] = a2_value;
    data[2] = b1_value;
    data[3] = b2_value;
  }


  float data[4];

};


//////////////////////////////////////////////////////////////////////////////////////////


  ///  glsl mat3

struct mat3
{
  mat3()
  {
    data[0] = 1.0;
    data[1] = 0.0;
    data[2] = 0.0;
    data[3] = 0.0;
    data[4] = 1.0;
    data[5] = 0.0;
    data[6] = 0.0;
    data[7] = 0.0;
    data[8] = 1.0;
  }


  mat3(float a1_value, float a2_value, float a3_value,
       float b1_value, float b2_value, float b3_value,
       float c1_value, float c2_value, float c3_value)
  {
    data[0] = a1_value;
    data[1] = a2_value;
    data[2] = a3_value;
    data[3] = b1_value;
    data[4] = b2_value;
    data[5] = b3_value;
    data[6] = c1_value;
    data[7] = c2_value;
    data[8] = c3_value;
  }


  float data[9];

};



//////////////////////////////////////////////////////////////////////////////////////////


extern std::ostream& operator<< (std::ostream&, const mat3&);


//////////////////////////////////////////////////////////////////////////////////////////


  ///  glsl mat4

struct mat4
{
  mat4()
  {
    data[0]  = 1.0;
    data[1]  = 0.0;
    data[2]  = 0.0;
    data[3]  = 0.0;
    data[4]  = 1.0;
    data[5]  = 0.0;
    data[6]  = 0.0;
    data[7]  = 0.0;
    data[8]  = 1.0;
    data[9]  = 1.0;
    data[10] = 0.0;
    data[11] = 0.0;
    data[12] = 0.0;
    data[13] = 1.0;
    data[14] = 0.0;
    data[15] = 0.0;
  }


  mat4(float a1_value, float a2_value, float a3_value, float a4_value,
       float b1_value, float b2_value, float b3_value, float b4_value,
       float c1_value, float c2_value, float c3_value, float c4_value,
       float d1_value, float d2_value, float d3_value, float d4_value)
  {
    data[0]  = a1_value;
    data[1]  = a2_value;
    data[2]  = a3_value;
    data[3]  = a4_value;
    data[4]  = b1_value;
    data[5]  = b2_value;
    data[6]  = b3_value;
    data[7]  = b4_value;
    data[8]  = c1_value;
    data[9]  = c2_value;
    data[10] = c3_value;
    data[11] = c4_value;
    data[12] = d1_value;
    data[13] = d2_value;
    data[14] = d3_value;
    data[15] = d4_value;
  }


  mat4(const gloost::Matrix& matrix)
  {
    data[0]  = matrix[0];
    data[1]  = matrix[1];
    data[2]  = matrix[2];
    data[3]  = matrix[3];
    data[4]  = matrix[4];
    data[5]  = matrix[5];
    data[6]  = matrix[6];
    data[7]  = matrix[7];
    data[8]  = matrix[8];
    data[9]  = matrix[9];
    data[10] = matrix[10];
    data[11] = matrix[11];
    data[12] = matrix[12];
    data[13] = matrix[13];
    data[14] = matrix[14];
    data[15] = matrix[15];
  }


  float data[16];

};


//////////////////////////////////////////////////////////////////////////////////////////


  ///  glsl sampler1D

struct sampler1D
{
  sampler1D()
  {
    handle = 0;
  }


  sampler1D(unsigned int handle_value)
  {
    handle = handle_value;
  }


  unsigned int handle;
};


//////////////////////////////////////////////////////////////////////////////////////////


  ///  glsl sampler2D

struct sampler2D
{
  sampler2D()
  {
    handle = 0;
  }


  sampler2D(unsigned int handle_value)
  {
    handle = handle_value;
  }


  unsigned int handle;
};


//////////////////////////////////////////////////////////////////////////////////////////


  ///  glsl sampler3D

struct sampler3D
{
  sampler3D()
  {
    handle = 0;
  }


  sampler3D(unsigned int handle_value)
  {
    handle = handle_value;
  }


  unsigned int handle;
};


//////////////////////////////////////////////////////////////////////////////////////////


  ///  glsl samplerCube

struct samplerCube
{
  samplerCube()
  {
    handle = 0;
  }


  samplerCube(unsigned int handle_value)
  {
    handle = handle_value;
  }


  unsigned int handle;
};


//////////////////////////////////////////////////////////////////////////////////////////


  ///  glsl sampler1DShadow

struct sampler1DShadow
{
  sampler1DShadow()
  {
    handle = 0;
  }


  sampler1DShadow(unsigned int handle_value)
  {
    handle = handle_value;
  }


  unsigned int handle;
};


//////////////////////////////////////////////////////////////////////////////////////////


  ///  glsl sampler2DShadow

struct sampler2DShadow
{
  sampler2DShadow()
  {
    handle = 0;
  }


  sampler2DShadow(unsigned int handle_value)
  {
    handle = handle_value;
  }


  unsigned int handle;
};


//////////////////////////////////////////////////////////////////////////////////////////




} // namespace gloost


#endif // GLOOST_UNIFORMTYPES_H

