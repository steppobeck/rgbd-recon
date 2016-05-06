
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



#ifndef GLOOST_MATERIAL_H
#define GLOOST_MATERIAL_H



/// gloost includes
#include <UniformTypes.h>



/// cpp includes
#include <string>
#include <glbinding/gl/gl.h>
using namespace gl;
#include <GL/glut.h>



namespace gloost
{



  /// Wrapper for glMaterial with GLSL Uniform abilities

class Material
{
  public:

    // Constructor
    Material();

    Material(float KaR, float KaG, float KaB, float KaA,
             float KdR, float KdG, float KdB, float KdA,
             float KsR, float KsG, float KsB, float KsA,
             float  illumination,
             float  Ns);


    gloost::vec4 getAmbient() const;
    void setAmbient(const float& r, const float& g, const float& b, const float& a);
    void setAmbient(const gloost::vec4& color);


    gloost::vec4 getDiffuse() const;
    void setDiffuse(const float& r, const float& g, const float& b, const float& a);
    void setDiffuse(const gloost::vec4& color);


    gloost::vec4 getSpecular() const;
    void setSpecular(const float& r, const float& g, const float& b, const float& a);
    void setSpecular(const gloost::vec4& color);


    float getIllumination();
    void setIllumination(float ill);


    float getNs();
    void  setNs(float ns);



    // Set this color with openGL
    void setGlMaterial();


    float   _ambient[4];
    float   _diffuse[4];
    float   _specular[4];
    float   _illumination;
    float   _Ns;


    /// GL_FRONT, GL_BACK, GL_FRONT_AND_BACK (default)
    GLenum _faceDirection;


};


} // namespace gloost


#endif // GLOOST_MATERIAL_H
