
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



/// gloost includes
#include <Material.h>



/// cpp includes



namespace gloost
{

///////////////////////////////////////////////////////////////////////////////


Material::Material():
     _ambient(),
     _diffuse(),
     _specular(),
     _illumination(1.0f),
     _Ns(),
     _faceDirection(GL_FRONT_AND_BACK)
{
  _ambient[0]  = 0.0f; _ambient[1]  = 0.0f; _ambient[2]  = 0.0f; _ambient[3]  = 1.0f;
  _diffuse[0]  = 0.5f;  _diffuse[1] = 0.5; _diffuse[2]  = 0.5; _diffuse[3]  = 1.0f;
  _specular[0] = 1.0f; _specular[1] = 1.0f; _specular[2] = 1.0f; _specular[3] = 1.0f;

  _illumination  = 0.0f;
  _Ns            = 60.5f;
}


///////////////////////////////////////////////////////////////////////////////


Material::Material(float KaR, float KaG, float KaB, float KaA,
                   float KdR, float KdG, float KdB, float KdA,
                   float KsR, float KsG, float KsB, float KsA,
                   float  illumination,
                   float  Ns)
{
    _ambient[0]  = KaR; _ambient[1]  = KaG; _ambient[2]  = KaB; _ambient[3]  = KaA;
    _diffuse[0]  = KdR; _diffuse[1]  = KdG; _diffuse[2]  = KdB; _diffuse[3]  = KdA;
    _specular[0] = KsR; _specular[1] = KsG; _specular[2] = KsB; _specular[3] = KsA;
    _illumination = illumination;
    _Ns          = Ns;
}


///////////////////////////////////////////////////////////////////////////////


  ///

gloost::vec4
Material::getAmbient() const
{
  return gloost::vec4(_ambient[0], _ambient[1], _ambient[2], _ambient[3]);
}


///////////////////////////////////////////////////////////////////////////////


  ///

void
Material::setAmbient(const float& r, const float& g, const float& b, const float& a)
{
  _ambient[0] = r;
  _ambient[1] = g;
  _ambient[2] = b;
  _ambient[3] = a;
}


///////////////////////////////////////////////////////////////////////////////


  ///

void
Material::setAmbient(const gloost::vec4& color)
{
  _ambient[0] = color.r;
  _ambient[1] = color.g;
  _ambient[2] = color.b;
  _ambient[3] = color.a;
}


///////////////////////////////////////////////////////////////////////////////


  ///

gloost::vec4
Material::getDiffuse() const
{

  return gloost::vec4(_diffuse[0], _diffuse[1], _diffuse[2], _diffuse[3]);

}


///////////////////////////////////////////////////////////////////////////////


  ///

void
Material::setDiffuse(const float& r, const float& g, const float& b, const float& a)
{
  _diffuse[0] = r;
  _diffuse[1] = g;
  _diffuse[2] = b;
  _diffuse[3] = a;
}


///////////////////////////////////////////////////////////////////////////////


  ///

void
Material::setDiffuse(const gloost::vec4& color)
{
  _diffuse[0] = color.r;
  _diffuse[1] = color.g;
  _diffuse[2] = color.b;
  _diffuse[3] = color.a;
}


///////////////////////////////////////////////////////////////////////////////


  ///

gloost::vec4
Material::getSpecular() const
{
  return gloost::vec4(_specular[0], _specular[1], _specular[2], _specular[3]);
}


///////////////////////////////////////////////////////////////////////////////


  ///

void
Material::setSpecular(const float& r, const float& g, const float& b, const float& a)
{
  _specular[0] = r;
  _specular[1] = g;
  _specular[2] = b;
  _specular[3] = a;
}


///////////////////////////////////////////////////////////////////////////////


  ///

void
Material::setSpecular(const gloost::vec4& color)
{
  _specular[0] = color.r;
  _specular[1] = color.g;
  _specular[2] = color.b;
  _specular[3] = color.a;
}


///////////////////////////////////////////////////////////////////////////////


  ///

float
Material::getIllumination()
{
  return _illumination;
}


///////////////////////////////////////////////////////////////////////////////


  ///

void
Material::setIllumination(float ill)
{
  _illumination = ill;
}


///////////////////////////////////////////////////////////////////////////////


  ///

float
Material::getNs()
{
  return _Ns;
}


///////////////////////////////////////////////////////////////////////////////


  ///

void
Material::setNs(float ns)
{
  _Ns = ns;
}


///////////////////////////////////////////////////////////////////////////////


void
Material::setGlMaterial()
{
  glMaterialfv(_faceDirection, GL_AMBIENT,   _ambient);
  glMaterialfv(_faceDirection, GL_DIFFUSE,   _diffuse);
  glMaterialfv(_faceDirection, GL_SPECULAR,  _specular);
  glMaterialf( _faceDirection, GL_SHININESS,  _Ns);
}


///////////////////////////////////////////////////////////////////////////////

} // namespace gloost
