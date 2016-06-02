
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



#ifndef GLOOST_RENDERGOODIES_H
#define GLOOST_RENDERGOODIES_H



/// gloost includes
#include <UniformTypes.h>
#include <gloostMath.h>



/// cpp includes
#include <glbinding/gl/gl.h>
using namespace gl;
#include <string>



namespace gloost
{



///////////////////////////////////////////////////////////////////////////////


  /// draws a quad from (0.0, 0.0, 0.0) to (1.0, 1.0, 0.0) with texture coords (look from positiv z)

inline void drawQuad()
{
  glBegin(GL_QUADS);
  {
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f  (0.0f, 1.0f, 0.0f);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f  (0.0f, 0.0f, 0.0f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f  (1.0f, 0.0f, 0.0f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f  (1.0f, 1.0f, 0.0f);
  }
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a quad from (0.0, 0.0, 0.0) to (1.0, 1.0, 0.0) with modified texture coords (look from positiv z)

inline void drawQuad(float minU, float maxU, float minV, float maxV)
{
  glBegin(GL_QUADS);
  {
    glTexCoord2f(minU, minV);
    glVertex3f  (0.0f, 1.0f, 0.0f);

    glTexCoord2f(minU, maxV);
    glVertex3f  (0.0f, 0.0f, 0.0f);

    glTexCoord2f(maxU, maxV);
    glVertex3f  (1.0f, 0.0f, 0.0f);

    glTexCoord2f(maxU, minV);
    glVertex3f  (1.0f, 1.0f, 0.0f);
  }
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a quad from (0.0, 0.0, 0.0) to (1.0, 1.0, 0.0) with modified texture coords (look from positiv z)

inline void drawQuad(vec4 texcoords)
{
  drawQuad(texcoords.r, texcoords.g, texcoords.b, texcoords.a);
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a quad from (-0.5, -0.5, 0.0) to (0.5, 0.5, 0.0) with texture coords (look from positiv z)

inline void drawQuadCenter()
{
  glBegin(GL_QUADS);
  {
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f  (-0.5f, 0.5f, 0.0f);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f  (-0.5f, -0.5f, 0.0f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f  (0.5f, -0.5f, 0.0f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f  (0.5f,  0.5f, 0.0f);
  }
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a quad from (-0.5, -0.5, 0.0) to (0.5, 0.5, 0.0) with modified texture coords (look from positiv z)

inline void drawQuadCenter(float minU, float maxU, float minV, float maxV)
{
  glBegin(GL_QUADS);
  {
    glTexCoord2f(minU, minV);
    glVertex3f  (-0.5f, 0.5f, 0.0f);

    glTexCoord2f(minU, maxV);
    glVertex3f  (-0.5f, -0.5f, 0.0f);

    glTexCoord2f(maxU, maxV);
    glVertex3f  (0.5f, -0.5f, 0.0f);

    glTexCoord2f(maxU, minV);
    glVertex3f  (0.5f,  0.5f, 0.0f);
  }
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a quad from (-0.5, -0.5, 0.0) to (0.5, 0.5, 0.0) with modified texture coords (look from positiv z)

inline void drawQuadCenter(const vec4& texcoords)
{
  glBegin(GL_QUADS);
  {
    drawQuadCenter(texcoords.r, texcoords.g, texcoords.b, texcoords.a);
  }
  glEnd();
}

///////////////////////////////////////////////////////////////////////////////


  /// Draws a simple box using the given corners

inline void drawBox(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax)
{

  // front
  glBegin(GL_QUADS);
    glVertex3f(xmin, ymin, zmax);
    glVertex3f(xmax, ymin, zmax);
    glVertex3f(xmax, ymax, zmax);
    glVertex3f(xmin, ymax, zmax);
  glEnd();


  // right
  glBegin(GL_QUADS);
    glVertex3f(xmax, ymin, zmax);
    glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmax, ymax, zmax);
  glEnd();


  // back
  glBegin(GL_QUADS);
    glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmin, ymin, zmin);
    glVertex3f(xmin, ymax, zmin);
    glVertex3f(xmax, ymax, zmin);
  glEnd();

  // left
  glBegin(GL_QUADS);
    glVertex3f(xmin, ymin, zmin);
    glVertex3f(xmin, ymin, zmax);
    glVertex3f(xmin, ymax, zmax);
    glVertex3f(xmin, ymax, zmin);
  glEnd();

  // top
  glBegin(GL_QUADS);
    glVertex3f(xmin, ymax, zmax);
    glVertex3f(xmax, ymax, zmax);
    glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmin, ymax, zmin);
  glEnd();

  // bottom
  glBegin(GL_QUADS);
    glVertex3f(xmin, ymin, zmin);
    glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmax, ymin, zmax);
    glVertex3f(xmin, ymin, zmax);
  glEnd();

}

///////////////////////////////////////////////////////////////////////////////


  /// Draws a simple box using the given corners

inline void drawBox()
{
  drawBox(-0.5, -0.5, -0.5,
           0.5,  0.5,  0.5);
}

///////////////////////////////////////////////////////////////////////////////


  /// Draws a simple wired box using the given corners

inline void drawWiredBox(float xmin = -0.5, float ymin= -0.5, float zmin= -0.5,
                         float xmax= 0.5, float ymax= 0.5, float zmax= 0.5)
{

  // front
  glBegin(GL_LINE_LOOP);
    glVertex3f(xmin, ymin, zmax);
    glVertex3f(xmax, ymin, zmax);
    glVertex3f(xmax, ymax, zmax);
    glVertex3f(xmin, ymax, zmax);
  glEnd();


  // right
  glBegin(GL_LINE_LOOP);
    glVertex3f(xmax, ymin, zmax);
    glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmax, ymax, zmax);
  glEnd();


  // back
  glBegin(GL_LINE_LOOP);
    glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmin, ymin, zmin);
    glVertex3f(xmin, ymax, zmin);
    glVertex3f(xmax, ymax, zmin);
  glEnd();

  // left
  glBegin(GL_LINE_LOOP);
    glVertex3f(xmin, ymin, zmin);
    glVertex3f(xmin, ymin, zmax);
    glVertex3f(xmin, ymax, zmax);
    glVertex3f(xmin, ymax, zmin);
  glEnd();

  // top
  glBegin(GL_LINE_LOOP);
    glVertex3f(xmin, ymax, zmax);
    glVertex3f(xmax, ymax, zmax);
    glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmin, ymax, zmin);
  glEnd();

  // bottom
  glBegin(GL_LINE_LOOP);
    glVertex3f(xmin, ymin, zmin);
    glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmax, ymin, zmax);
    glVertex3f(xmin, ymin, zmax);
  glEnd();

}

///////////////////////////////////////////////////////////////////////////////


  /// call this function befor you call one of the gloost::drawColorXXXX( ... ) functions

inline void startDrawShapes()
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  {
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glEnable(GL_LINE_SMOOTH);
  }
}

///////////////////////////////////////////////////////////////////////////////


  /// call this function befor you call one of the gloost::drawColorXXXX( ... ) functions

inline void endDrawShapes()
{
  glPopAttrib();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a raster with defined tiling from (-0.5, -0.5, 0.0) to (0.5, 0.5, 0.0)

inline void drawWireRaster(unsigned int tiling)
{
  glBegin(GL_LINES);
    for(unsigned int i=0; i != tiling+1; ++i)
    {
      float step = 1.0f/tiling;

      glVertex3f(-0.5+step*i, 0.5, 0.0);
      glVertex3f(-0.5+step*i, -0.5, 0.0);

      glVertex3f(0.5, -0.5+step*i, 0.0);
      glVertex3f(-0.5, -0.5+step*i, 0.0);
    }
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a raster with defined x- and y-tiling from (-0.5, -0.5, 0.0) to (0.5, 0.5, 0.0)

inline void drawWireRaster(unsigned int tilingH, unsigned int tilingV)
{
  glBegin(GL_LINES);
    for(unsigned int i=0; i != tilingH+1; ++i)
    {
      float step = 1.0f/tilingH;

      glVertex3f(-0.5+step*i, 0.5, 0.0);
      glVertex3f(-0.5+step*i, -0.5, 0.0);
    }

    for(unsigned int i=0; i != tilingV+1; ++i)
    {
      float step = 1.0f/tilingV;

      glVertex3f(0.5, -0.5+step*i, 0.0);
      glVertex3f(-0.5, -0.5+step*i, 0.0);
    }
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a half wire box from (-0.5, -0.5, 0.0) to (0.5, 0.5, 0.0)

inline void drawWireQuad()
{
  glBegin(GL_LINE_LOOP);
  {
    glVertex3f  (0.0f, 1.0f, 0.0f);
    glVertex3f  (0.0f, 0.0f, 0.0f);
    glVertex3f  (1.0f, 0.0f, 0.0f);
    glVertex3f  (1.0f, 1.0f, 0.0f);
  }
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a half wire box from (-0.5, -0.5, 0.0) to (0.5, 0.5, 0.0)

inline void drawWireQuadCenter()
{
  glBegin(GL_LINE_LOOP);
    glVertex3f(-0.5, 0.5, 0.0);
    glVertex3f(-0.5, -0.5, 0.0);

    glVertex3f(0.5, -0.5, 0.0);
    glVertex3f(0.5, 0.5, 0.0);
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a wire triangle

inline void drawWireTriangle()
{
  glBegin(GL_LINE_LOOP);
    glVertex3f(-0.5, -0.5, 0.0);
    glVertex3f(0.5, -0.5, 0.0);

    glVertex3f(0.0, 0.5, 0.0);
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a half wire Circle from (-0.5, -0.5, 0.0) to (0.5, 0.5, 0.0)

inline void drawWireCircle(unsigned int sides = 24)
{
  glBegin(GL_LINE_LOOP);
    for(unsigned int i=0; i != sides; ++i)
    {
      float value = i*gloost::PI/sides*2.0f;
      glVertex3f(cos(value)*0.5f, sin(value)*0.5f, 0.0f);
    }
  glEnd();

//  glBegin(GL_LINES);
//    glVertex3f(0.0f, 0.0f, 0.0f);
//    glVertex3f(0.0f, 0.5f, 0.0f);
//  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a wire Circle from (-0.5, -0.5, 0.0) to (0.5, 0.5, 0.0)

inline void drawWireCircle48()
{
  glBegin(GL_LINE_LOOP);
  {
    glVertex3f(0.5, 0, 0);
    glVertex3f(0.495722, 0.0652631, 0);
    glVertex3f(0.482963, 0.12941, 0);
    glVertex3f(0.46194, 0.191342, 0);
    glVertex3f(0.433013, 0.25, 0);
    glVertex3f(0.396677, 0.304381, 0);
    glVertex3f(0.353553, 0.353553, 0);
    glVertex3f(0.304381, 0.396677, 0);
    glVertex3f(0.25, 0.433013, 0);
    glVertex3f(0.191342, 0.46194, 0);
    glVertex3f(0.12941, 0.482963, 0);
    glVertex3f(0.0652631, 0.495722, 0);
    glVertex3f(-2.18557e-08, 0.5, 0);
    glVertex3f(-0.0652631, 0.495722, 0);
    glVertex3f(-0.12941, 0.482963, 0);
    glVertex3f(-0.191342, 0.46194, 0);
    glVertex3f(-0.25, 0.433013, 0);
    glVertex3f(-0.304381, 0.396677, 0);
    glVertex3f(-0.353553, 0.353553, 0);
    glVertex3f(-0.396677, 0.304381, 0);
    glVertex3f(-0.433013, 0.25, 0);
    glVertex3f(-0.46194, 0.191342, 0);
    glVertex3f(-0.482963, 0.129409, 0);
    glVertex3f(-0.495722, 0.065263, 0);
    glVertex3f(-0.5, -4.37114e-08, 0);
    glVertex3f(-0.495722, -0.0652631, 0);
    glVertex3f(-0.482963, -0.12941, 0);
    glVertex3f(-0.46194, -0.191342, 0);
    glVertex3f(-0.433013, -0.25, 0);
    glVertex3f(-0.396677, -0.304381, 0);
    glVertex3f(-0.353553, -0.353553, 0);
    glVertex3f(-0.304381, -0.396677, 0);
    glVertex3f(-0.25, -0.433013, 0);
    glVertex3f(-0.191342, -0.46194, 0);
    glVertex3f(-0.129409, -0.482963, 0);
    glVertex3f(-0.065263, -0.495722, 0);
    glVertex3f(5.96244e-09, -0.5, 0);
    glVertex3f(0.0652632, -0.495722, 0);
    glVertex3f(0.12941, -0.482963, 0);
    glVertex3f(0.191342, -0.46194, 0);
    glVertex3f(0.25, -0.433013, 0);
    glVertex3f(0.304381, -0.396677, 0);
    glVertex3f(0.353554, -0.353553, 0);
    glVertex3f(0.396677, -0.304381, 0);
    glVertex3f(0.433013, -0.25, 0);
    glVertex3f(0.46194, -0.191342, 0);
    glVertex3f(0.482963, -0.129409, 0);
    glVertex3f(0.495722, -0.0652631, 0);
  }
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a wire Circle from (-0.5, -0.5, 0.0) to (0.5, 0.5, 0.0)

inline void drawWireCircle128()
{
  glBegin(GL_LINE_LOOP);
  {
    glVertex3f( 0, 0.5, 0.0 );
    glVertex3f( 0.0245338, 0.499398, 0.0 );
    glVertex3f( 0.0490086, 0.497592, 0.0 );
    glVertex3f( 0.0733652, 0.494588, 0.0 );
    glVertex3f( 0.0975452, 0.490393, 0.0 );
    glVertex3f( 0.12149, 0.485016, 0.0 );
    glVertex3f( 0.145142, 0.47847, 0.0 );
    glVertex3f( 0.168445, 0.470772, 0.0 );
    glVertex3f( 0.191342, 0.46194, 0.0 );
    glVertex3f( 0.213778, 0.451995, 0.0 );
    glVertex3f( 0.235698, 0.440961, 0.0 );
    glVertex3f( 0.257051, 0.428864, 0.0 );
    glVertex3f( 0.277785, 0.415735, 0.0 );
    glVertex3f( 0.29785, 0.401604, 0.0 );
    glVertex3f( 0.317197, 0.386505, 0.0 );
    glVertex3f( 0.335779, 0.370476, 0.0 );
    glVertex3f( 0.353553, 0.353553, 0.0 );
    glVertex3f( 0.370476, 0.335779, 0.0 );
    glVertex3f( 0.386505, 0.317197, 0.0 );
    glVertex3f( 0.401604, 0.29785, 0.0 );
    glVertex3f( 0.415735, 0.277785, 0.0 );
    glVertex3f( 0.428864, 0.257051, 0.0 );
    glVertex3f( 0.440961, 0.235698, 0.0 );
    glVertex3f( 0.451995, 0.213778, 0.0 );
    glVertex3f( 0.46194, 0.191342, 0.0 );
    glVertex3f( 0.470772, 0.168445, 0.0 );
    glVertex3f( 0.47847, 0.145142, 0.0 );
    glVertex3f( 0.485016, 0.12149, 0.0 );
    glVertex3f( 0.490393, 0.0975451, 0.0 );
    glVertex3f( 0.494588, 0.0733652, 0.0 );
    glVertex3f( 0.497592, 0.0490085, 0.0 );
    glVertex3f( 0.499398, 0.0245338, 0.0 );
    glVertex3f( 0.5, -2.18557e-08, 0.0 );
    glVertex3f( 0.499398, -0.0245339, 0.0 );
    glVertex3f( 0.497592, -0.0490086, 0.0 );
    glVertex3f( 0.494588, -0.0733653, 0.0 );
    glVertex3f( 0.490393, -0.0975452, 0.0 );
    glVertex3f( 0.485016, -0.12149, 0.0 );
    glVertex3f( 0.47847, -0.145142, 0.0 );
    glVertex3f( 0.470772, -0.168445, 0.0 );
    glVertex3f( 0.46194, -0.191342, 0.0 );
    glVertex3f( 0.451995, -0.213778, 0.0 );
    glVertex3f( 0.440961, -0.235698, 0.0 );
    glVertex3f( 0.428864, -0.257051, 0.0 );
    glVertex3f( 0.415735, -0.277785, 0.0 );
    glVertex3f( 0.401604, -0.29785, 0.0 );
    glVertex3f( 0.386505, -0.317197, 0.0 );
    glVertex3f( 0.370476, -0.33578, 0.0 );
    glVertex3f( 0.353553, -0.353553, 0.0 );
    glVertex3f( 0.335779, -0.370476, 0.0 );
    glVertex3f( 0.317197, -0.386505, 0.0 );
    glVertex3f( 0.29785, -0.401604, 0.0 );
    glVertex3f( 0.277785, -0.415735, 0.0 );
    glVertex3f( 0.257051, -0.428864, 0.0 );
    glVertex3f( 0.235698, -0.440961, 0.0 );
    glVertex3f( 0.213778, -0.451995, 0.0 );
    glVertex3f( 0.191342, -0.46194, 0.0 );
    glVertex3f( 0.168445, -0.470772, 0.0 );
    glVertex3f( 0.145142, -0.47847, 0.0 );
    glVertex3f( 0.12149, -0.485016, 0.0 );
    glVertex3f( 0.0975451, -0.490393, 0.0 );
    glVertex3f( 0.0733652, -0.494588, 0.0 );
    glVertex3f( 0.0490085, -0.497592, 0.0 );
    glVertex3f( 0.0245338, -0.499398, 0.0 );
    glVertex3f( -4.37114e-08, -0.5, 0.0 );
    glVertex3f( -0.0245339, -0.499398, 0.0 );
    glVertex3f( -0.0490086, -0.497592, 0.0 );
    glVertex3f( -0.0733653, -0.494588, 0.0 );
    glVertex3f( -0.0975452, -0.490393, 0.0 );
    glVertex3f( -0.12149, -0.485016, 0.0 );
    glVertex3f( -0.145142, -0.47847, 0.0 );
    glVertex3f( -0.168445, -0.470772, 0.0 );
    glVertex3f( -0.191342, -0.46194, 0.0 );
    glVertex3f( -0.213778, -0.451995, 0.0 );
    glVertex3f( -0.235698, -0.440961, 0.0 );
    glVertex3f( -0.257051, -0.428864, 0.0 );
    glVertex3f( -0.277785, -0.415735, 0.0 );
    glVertex3f( -0.29785, -0.401604, 0.0 );
    glVertex3f( -0.317197, -0.386505, 0.0 );
    glVertex3f( -0.33578, -0.370476, 0.0 );
    glVertex3f( -0.353553, -0.353553, 0.0 );
    glVertex3f( -0.370476, -0.335779, 0.0 );
    glVertex3f( -0.386505, -0.317197, 0.0 );
    glVertex3f( -0.401604, -0.29785, 0.0 );
    glVertex3f( -0.415735, -0.277785, 0.0 );
    glVertex3f( -0.428864, -0.257051, 0.0 );
    glVertex3f( -0.440961, -0.235698, 0.0 );
    glVertex3f( -0.451995, -0.213777, 0.0 );
    glVertex3f( -0.46194, -0.191342, 0.0 );
    glVertex3f( -0.470772, -0.168445, 0.0 );
    glVertex3f( -0.47847, -0.145142, 0.0 );
    glVertex3f( -0.485016, -0.12149, 0.0 );
    glVertex3f( -0.490393, -0.0975451, 0.0 );
    glVertex3f( -0.494588, -0.0733652, 0.0 );
    glVertex3f( -0.497592, -0.0490085, 0.0 );
    glVertex3f( -0.499398, -0.0245338, 0.0 );
    glVertex3f( -0.5, 6.55671e-08, 0.0 );
    glVertex3f( -0.499398, 0.0245339, 0.0 );
    glVertex3f( -0.497592, 0.0490086, 0.0 );
    glVertex3f( -0.494588, 0.0733653, 0.0 );
    glVertex3f( -0.490393, 0.0975452, 0.0 );
    glVertex3f( -0.485016, 0.12149, 0.0 );
    glVertex3f( -0.47847, 0.145142, 0.0 );
    glVertex3f( -0.470772, 0.168445, 0.0 );
    glVertex3f( -0.46194, 0.191342, 0.0 );
    glVertex3f( -0.451995, 0.213778, 0.0 );
    glVertex3f( -0.440961, 0.235698, 0.0 );
    glVertex3f( -0.428864, 0.257051, 0.0 );
    glVertex3f( -0.415735, 0.277785, 0.0 );
    glVertex3f( -0.401604, 0.29785, 0.0 );
    glVertex3f( -0.386505, 0.317197, 0.0 );
    glVertex3f( -0.370476, 0.33578, 0.0 );
    glVertex3f( -0.353553, 0.353553, 0.0 );
    glVertex3f( -0.335779, 0.370476, 0.0 );
    glVertex3f( -0.317197, 0.386505, 0.0 );
    glVertex3f( -0.29785, 0.401604, 0.0 );
    glVertex3f( -0.277785, 0.415735, 0.0 );
    glVertex3f( -0.257051, 0.428864, 0.0 );
    glVertex3f( -0.235698, 0.440961, 0.0 );
    glVertex3f( -0.213777, 0.451995, 0.0 );
    glVertex3f( -0.191342, 0.46194, 0.0 );
    glVertex3f( -0.168445, 0.470772, 0.0 );
    glVertex3f( -0.145142, 0.47847, 0.0 );
    glVertex3f( -0.12149, 0.485016, 0.0 );
    glVertex3f( -0.0975451, 0.490393, 0.0 );
    glVertex3f( -0.0733652, 0.494588, 0.0 );
    glVertex3f( -0.0490085, 0.497592, 0.0 );
    glVertex3f( -0.0245338, 0.499398, 0.0 );
  }
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a wire Circle from (-0.5, -0.5, 0.0) to (0.5, 0.5, 0.0)

inline void drawWireCircle24()
{
  glBegin(GL_LINE_LOOP);
  {
    glVertex3f(0.5, 0, 0);
    glVertex3f(0.482963, 0.12941, 0);
    glVertex3f(0.433013, 0.25, 0);
    glVertex3f(0.353553, 0.353553, 0);
    glVertex3f(0.25, 0.433013, 0);
    glVertex3f(0.12941, 0.482963, 0);
    glVertex3f(-2.18557e-08, 0.5, 0);
    glVertex3f(-0.12941, 0.482963, 0);
    glVertex3f(-0.25, 0.433013, 0);
    glVertex3f(-0.353553, 0.353553, 0);
    glVertex3f(-0.433013, 0.25, 0);
    glVertex3f(-0.482963, 0.129409, 0);
    glVertex3f(-0.5, -4.37114e-08, 0);
    glVertex3f(-0.482963, -0.12941, 0);
    glVertex3f(-0.433013, -0.25, 0);
    glVertex3f(-0.353553, -0.353553, 0);
    glVertex3f(-0.25, -0.433013, 0);
    glVertex3f(-0.129409, -0.482963, 0);
    glVertex3f(5.96244e-09, -0.5, 0);
    glVertex3f(0.12941, -0.482963, 0);
    glVertex3f(0.25, -0.433013, 0);
    glVertex3f(0.353554, -0.353553, 0);
    glVertex3f(0.433013, -0.25, 0);
    glVertex3f(0.482963, -0.129409, 0);
  }
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a wire Circle from (-0.5, -0.5, 0.0) to (0.5, 0.5, 0.0)

inline void drawWireCircle12()
{
  glBegin(GL_LINE_LOOP);
  {
    glVertex3f(0.482963, 0.12941, 0);
    glVertex3f(0.353553, 0.353553, 0);
    glVertex3f(0.12941, 0.482963, 0);
    glVertex3f(-0.12941, 0.482963, 0);
    glVertex3f(-0.353553, 0.353553, 0);
    glVertex3f(-0.482963, 0.129409, 0);
    glVertex3f(-0.482963, -0.12941, 0);
    glVertex3f(-0.353553, -0.353553, 0);
    glVertex3f(-0.129409, -0.482963, 0);
    glVertex3f(0.12941, -0.482963, 0);
    glVertex3f(0.353554, -0.353553, 0);
    glVertex3f(0.482963, -0.129409, 0);
  }
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a solid Circle from (-0.5, -0.5, 0.0) to (0.5, 0.5, 0.0)

inline void drawSolidCircle12()
{
  glBegin(GL_TRIANGLE_FAN);
  {
//    glColor3f(1.0,1.0,1.0);

    glVertex3f(0.0f, 0.0f, 0.0f);

    glVertex3f(0.482963, 0.12941, 0);
    glVertex3f(0.353553, 0.353553, 0);
    glVertex3f(0.12941, 0.482963, 0);
    glVertex3f(-0.12941, 0.482963, 0);
    glVertex3f(-0.353553, 0.353553, 0);
    glVertex3f(-0.482963, 0.129409, 0);
    glVertex3f(-0.482963, -0.12941, 0);
    glVertex3f(-0.353553, -0.353553, 0);
    glVertex3f(-0.129409, -0.482963, 0);
    glVertex3f(0.12941, -0.482963, 0);
    glVertex3f(0.353554, -0.353553, 0);
    glVertex3f(0.482963, -0.129409, 0);
    glVertex3f(0.482963, 0.12941, 0);
  }
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a solid Circle from (-0.5, -0.5, 0.0) to (0.5, 0.5, 0.0)

inline void drawSolidCircle48()
{
  glBegin(GL_TRIANGLE_FAN);
  {
//    glColor3f(1.0,1.0,1.0);

    glVertex3f(0.0f, 0.0f, 0.0f);

    glVertex3f(0.5, 0, 0);
    glVertex3f(0.495722, 0.0652631, 0);
    glVertex3f(0.482963, 0.12941, 0);
    glVertex3f(0.46194, 0.191342, 0);
    glVertex3f(0.433013, 0.25, 0);
    glVertex3f(0.396677, 0.304381, 0);
    glVertex3f(0.353553, 0.353553, 0);
    glVertex3f(0.304381, 0.396677, 0);
    glVertex3f(0.25, 0.433013, 0);
    glVertex3f(0.191342, 0.46194, 0);
    glVertex3f(0.12941, 0.482963, 0);
    glVertex3f(0.0652631, 0.495722, 0);
    glVertex3f(-2.18557e-08, 0.5, 0);
    glVertex3f(-0.0652631, 0.495722, 0);
    glVertex3f(-0.12941, 0.482963, 0);
    glVertex3f(-0.191342, 0.46194, 0);
    glVertex3f(-0.25, 0.433013, 0);
    glVertex3f(-0.304381, 0.396677, 0);
    glVertex3f(-0.353553, 0.353553, 0);
    glVertex3f(-0.396677, 0.304381, 0);
    glVertex3f(-0.433013, 0.25, 0);
    glVertex3f(-0.46194, 0.191342, 0);
    glVertex3f(-0.482963, 0.129409, 0);
    glVertex3f(-0.495722, 0.065263, 0);
    glVertex3f(-0.5, -4.37114e-08, 0);
    glVertex3f(-0.495722, -0.0652631, 0);
    glVertex3f(-0.482963, -0.12941, 0);
    glVertex3f(-0.46194, -0.191342, 0);
    glVertex3f(-0.433013, -0.25, 0);
    glVertex3f(-0.396677, -0.304381, 0);
    glVertex3f(-0.353553, -0.353553, 0);
    glVertex3f(-0.304381, -0.396677, 0);
    glVertex3f(-0.25, -0.433013, 0);
    glVertex3f(-0.191342, -0.46194, 0);
    glVertex3f(-0.129409, -0.482963, 0);
    glVertex3f(-0.065263, -0.495722, 0);
    glVertex3f(5.96244e-09, -0.5, 0);
    glVertex3f(0.0652632, -0.495722, 0);
    glVertex3f(0.12941, -0.482963, 0);
    glVertex3f(0.191342, -0.46194, 0);
    glVertex3f(0.25, -0.433013, 0);
    glVertex3f(0.304381, -0.396677, 0);
    glVertex3f(0.353554, -0.353553, 0);
    glVertex3f(0.396677, -0.304381, 0);
    glVertex3f(0.433013, -0.25, 0);
    glVertex3f(0.46194, -0.191342, 0);
    glVertex3f(0.482963, -0.129409, 0);
    glVertex3f(0.495722, -0.0652631, 0);
    glVertex3f(0.5, 0, 0);
  }
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a solid Circle from (-0.5, -0.5, 0.0) to (0.5, 0.5, 0.0)

inline void drawSolidCircle24()
{
  glBegin(GL_TRIANGLE_FAN);
  {
//    glColor3f(1.0,1.0,1.0);

    glVertex3f(0.0f, 0.0f, 0.0f);

    glVertex3f(0.5, 0, 0);
    glVertex3f(0.482963, 0.12941, 0);
    glVertex3f(0.433013, 0.25, 0);
    glVertex3f(0.353553, 0.353553, 0);
    glVertex3f(0.25, 0.433013, 0);
    glVertex3f(0.12941, 0.482963, 0);
    glVertex3f(-2.18557e-08, 0.5, 0);
    glVertex3f(-0.12941, 0.482963, 0);
    glVertex3f(-0.25, 0.433013, 0);
    glVertex3f(-0.353553, 0.353553, 0);
    glVertex3f(-0.433013, 0.25, 0);
    glVertex3f(-0.482963, 0.129409, 0);
    glVertex3f(-0.5, -4.37114e-08, 0);
    glVertex3f(-0.482963, -0.12941, 0);
    glVertex3f(-0.433013, -0.25, 0);
    glVertex3f(-0.353553, -0.353553, 0);
    glVertex3f(-0.25, -0.433013, 0);
    glVertex3f(-0.129409, -0.482963, 0);
    glVertex3f(5.96244e-09, -0.5, 0);
    glVertex3f(0.12941, -0.482963, 0);
    glVertex3f(0.25, -0.433013, 0);
    glVertex3f(0.353554, -0.353553, 0);
    glVertex3f(0.433013, -0.25, 0);
    glVertex3f(0.482963, -0.129409, 0);
    glVertex3f(0.5, 0, 0);
  }
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a solid Flower from (-0.5, -0.5, 0.0) to (0.5, 0.5, 0.0)

inline void drawSolidFlower1()
{
  glBegin(GL_TRIANGLE_STRIP);
  {
    glVertex3f(0.482963, 0.12941, 0);
    glVertex3f(0.353553, 0.353553, 0);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.12941, 0.482963, 0);
    glVertex3f(-0.12941, 0.482963, 0);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.353553, 0.353553, 0);
    glVertex3f(-0.482963, 0.129409, 0);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.482963, -0.12941, 0);
    glVertex3f(-0.353553, -0.353553, 0);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.129409, -0.482963, 0);
    glVertex3f(0.12941, -0.482963, 0);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.353554, -0.353553, 0);
    glVertex3f(0.482963, -0.129409, 0);
    glVertex3f(0.0f, 0.0f, 0.0f);
  }
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a Circle from (-0.5, -0.5, 0.0) to (0.5, 0.5, 0.0)

inline void drawCircle(unsigned int sides = 24)
{
  glBegin(GL_TRIANGLE_FAN);
    for(unsigned int i=0; i != sides; ++i)
    {
      float value = i*gloost::PI/sides*2.0f;
      glVertex3f(cos(value)*0.5f, sin(value)*0.5f, 0.0f);
    }
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws a kind of marker arround the origin

inline void drawWireFrameEdges()
{
  glBegin(GL_LINES);

    /// north-west
    glVertex3f(-0.5f,  0.25f, 0.0f);
    glVertex3f(-0.5f,  0.5f, 0.0f);
    glVertex3f(-0.5f,  0.5f, 0.0f);
    glVertex3f(-0.25f, 0.5f, 0.0f);

    /// north-east
    glVertex3f(0.5f,  0.25f, 0.0);
    glVertex3f(0.5f,  0.5f, 0.0f);
    glVertex3f(0.5f,  0.5f, 0.0f);
    glVertex3f(0.25f, 0.5f, 0.0f);


    /// south-west
    glVertex3f(-0.5f,  -0.25f, 0.0);
    glVertex3f(-0.5f,  -0.5f, 0.0f);
    glVertex3f(-0.5f,  -0.5f, 0.0f);
    glVertex3f(-0.25f, -0.5f, 0.0f);

    /// south-east
    glVertex3f(0.5f,  -0.25f, 0.0f);
    glVertex3f(0.5f,  -0.5f, 0.0f);
    glVertex3f(0.5f,  -0.5f, 0.0f);
    glVertex3f(0.25f, -0.5f, 0.0f);

  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws kind of a solid arrow shape

inline void drawSolidSimpleArrow()
{
  glBegin(GL_TRIANGLE_FAN);
  {
     glVertex3f( 0.0f,  0.0f,  0.0f);

     glVertex3f( 0.4f,  -0.2f,  0.0f);
     glVertex3f( 0.4f,  -0.0f,  0.0f);

     glVertex3f( 0.0f,  0.2f,  0.0f);

     glVertex3f( -0.4f,  -0.0f,  0.0f);
     glVertex3f( -0.4f,  -0.2f,  0.0f);
  }
  glEnd();
}


///////////////////////////////////////////////////////////////////////////////


  /// draws kind of a wire arrow shape

inline void drawWireSimpleArrow()
{
  glBegin(GL_LINE_LOOP);
  {
     glVertex3f( 0.0f,  0.0f,  0.0f);

     glVertex3f( 0.4f,  -0.2f,  0.0f);
     glVertex3f( 0.4f,  -0.0f,  0.0f);

     glVertex3f( 0.0f,  0.2f,  0.0f);

     glVertex3f( -0.4f,  -0.0f,  0.0f);
     glVertex3f( -0.4f,  -0.2f,  0.0f);
  }
  glEnd();



  glBegin(GL_LINES);
  {
    glVertex3f( -0.1f,  -0.0f,  0.0f);
    glVertex3f( -0.1f,  -0.2f,  0.0f);

    glVertex3f( 0.1f,  -0.0f,  0.0f);
    glVertex3f( 0.1f,  -0.2f,  0.0f);

    glVertex3f( -0.1f,  -0.2f,  0.0f);
    glVertex3f( 0.1f,  -0.2f,  0.0f);
  }
  glEnd();
}



}  // namespace gloost


#endif // #ifndef GLOOST_RENDERGOODIES_H

