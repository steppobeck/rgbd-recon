
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


#ifndef GLOOST_SYSTEM_CONFIG_H
#define GLOOST_SYSTEM_CONFIG_H


/*
  In this file you can change some fundamental properties and behaviors of
  your gloost lib. Read the comments carefully and be sure you know what you
  are doing.
*/



/// cpp includes
#include <iostream>



////////////////////////////////////////////////////////////////////////////////


  /*
     Here you can disable Messages, Warnings and Error Statements of gloost objects.
     Just uncomment the defines to disable console output.
  */

//  #define GLOOST_SYSTEM_DISABLE_OUTPUT_MESSAGES     /// <- let you know what gloost is doing
//  #define GLOOST_SYSTEM_DISABLE_OUTPUT_WARNINGS     /// <- occurs if something is going wrong. your app will not crash
//  #define GLOOST_SYSTEM_DISABLE_OUTPUT_ERRORS       /// <- occurs if something is going eh ... wronger. maybe your app will crash
//  #define GLOOST_SYSTEM_DISABLE_OUTPUT_DEVNOTES     /// <- will give you information if a method is not implemented or obsolete


////////////////////////////////////////////////////////////////////////////////


  /*
     Here you can choose the type of gloost::mathType
     Choose wisely one of the defines.
     Type used by Point3, Vector3, Ray, Matrix, ... .
  */

// #define GLOOST_SYSTEM_MATH_WITH_DOUBLE
#define GLOOST_SYSTEM_MATH_WITH_FLOAT


////////////////////////////////////////////////////////////////////////////////


  /*
     if you use more than one GL contexts, for example on more than one gfx card
     you can adjust the number of render contexts gloost will handle up to 32
  */

#define GLOOST_SYSTEM_NUM_RENDER_CONTEXTS 6  /// <-- default: 5


////////////////////////////////////////////////////////////////////////////////



  /*
    This define will cause the gloost::Shader::disable method to reset all
    available texture units and set all texture matrices to identity. This
    adds a certain amount of overhead to your cpu time every time a shader
    is disabled. On the pro side, the state of your texture units is always
    defined when disabling a shader.

    You have to keep track of your texture state yourselfe if you remove this
    define. In many cases this will be trivial, especially if you dont use
    texture matrices
  */

#define GLOOST_SYSTEM_CLEAR_TEXUNITS_ON_SHADER_DISABLE


////////////////////////////////////////////////////////////////////////////////

/// BETTER DONT CHANGE ANYTHING BELOW THIS LINE ////////////////////////////////

////////////////////////////////////////////////////////////////////////////////


namespace gloost
{

////////////////////////////////////////////////////////////////////////////////


  /// for mathType float

#ifdef GLOOST_SYSTEM_MATH_WITH_FLOAT

  /// ...
  typedef float mathType;

  /// ...
  #define GLOOST_MATH_TYPE_MAX_VALUE 10000000000.0f

  /// ...
  #define GLOOST_MATH_TYPE_EPSYLON   0.00001f



  /// openGL function replacements (this list may not be complete)
  #define gloostMultMatrix    glMultMatrixf
  #define gloostLoadMatrix    glLoadMatrixf
  #define gloostGetv          glGetFloatv

  #define gloostVertex2v      glVertex2fv
  #define gloostVertex3v      glVertex3fv
  #define gloostNormal2v      glNormal2fv
  #define gloostNormal3v      glNormal3fv
  #define gloostTexCoord1v    glTexCoord1fv
  #define gloostTexCoord2v    glTexCoord2fv
  #define gloostTexCoord3v    glTexCoord3fv
  #define gloostTexCoord4v    glTexCoord4fv

  #define gloostColor4v       glColor4fv
  #define gloostColor3v       glColor3fv

#endif


////////////////////////////////////////////////////////////////////////////////


  /// for mathType double

#ifdef GLOOST_SYSTEM_MATH_WITH_DOUBLE

  /// ...
  typedef double mathType;

  /// ...
  #define GLOOST_MATH_TYPE_MAX_VALUE 10000000000.0

  /// ...
  #define GLOOST_MATH_TYPE_EPSYLON   0.00001



  /// openGL function replacements (this list may not be complete)
  #define gloostMultMatrix    glMultMatrixd
  #define gloostLoadMatrix    glLoadMatrixd
  #define gloostGetv          glGetDoublev

  #define gloostVertex2v      glVertex2dv
  #define gloostVertex3v      glVertex3dv
  #define gloostNormal2v      glNormal2dv
  #define gloostNormal3v      glNormal3dv
  #define gloostTexCoord1v    glTexCoord1dv
  #define gloostTexCoord2v    glTexCoord2dv
  #define gloostTexCoord3v    glTexCoord3dv
  #define gloostTexCoord4v    glTexCoord4dv

  #define gloostColor4v       glColor4dv
  #define gloostColor3v       glColor3dv

#endif






////////////////////////////////////////////////////////////////////////////////
//
//
//  /// this functions prints some information about the configuration of your gloost
//
//void printSystemInfo()
//{
//  std::cout << std::endl;
//  std::cout << std::endl << "";
//
//
//  std::cout << std::endl << "                  ___                            __";
//  std::cout << std::endl << "                 /\"_ \"                          /\" \"__";
//  std::cout << std::endl << "              __ \"//\" \"     ___     ___     ____\" \"  _\"";
//  std::cout << std::endl << "            /'_ `\" \" \" \"   / __`\"  / __`\"  /  __\"\" \" \"/";
//  std::cout << std::endl << "           /\" \" \" \" \"_\" \"_/\" \" \" \"/\" \" \" \"/\"__   \"\" \" \"_";
//  std::cout << std::endl << "           \" \"____ \"/\"____\" \"____/\" \"____/\"/\"____/ \" \"__\"";
//  std::cout << std::endl << "            \"/___/\" \"/____/\"/___/  \"/___/  \"/___/   \"/__/";
//  std::cout << std::endl << "              /\"____/";
//  std::cout << std::endl << "              \"_/__/";
//  std::cout << std::endl << "";
//  std::cout << std::endl << "              OpenGL framework for fast demo programming";
//  std::cout << std::endl << "";
//  std::cout << std::endl << "                        http://www.gloost.org";
//
//
//  std::cout << std::endl << "########## gloost system info ##########";
//  std::cout << std::endl << "";
//  std::cout << std::endl << "gloost math works with ......... ";
//
//#ifdef GLOOST_SYSTEM_MATH_WITH_DOUBLE
//  std::cout << "double";
//#endif
//#ifdef GLOOST_SYSTEM_MATH_WITH_FLOAT
//  std::cout << "float";
//#endif
//
//  std::cout << std::endl << "";
//  std::cout << std::endl << "messages are ......... ";
//
//#ifdef GLOOST_SYSTEM_DISABLE_OUTPUT_MESSAGES
//  std::cout << "off";
//
//#endif
//
//
//
//
//  std::cout << std::endl;
//}
//



} /// namespace gloost



#endif /// GLOOST_SYSTEM_CONFIG_H
