
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



#ifndef H_GLOOST_SHADER
#define H_GLOOST_SHADER



/// gloost system includes
#include <MultiGlContext.h>



// cpp includes
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#ifndef __APPLE__
#   include <malloc.h>
#endif
#include <map>
#include <vector>
#include <fcntl.h>
#include <cstdlib>
#include <fstream>
#include <sys/stat.h>
#include <string.h>


#ifndef __GLEW_H__
#include <GL/glew.h>
#endif

#ifdef __APPLE__
#   include <GLUT/glut.h>
#else
#   include <GL/glut.h>
#endif



namespace gloost
{

// name for the default shader
#define GLOOST_SHADER_FILE_NONE "GLOOST_NULL_SHADER"


  // Wrapper for GLSL shader supporting vertex, fragment and geometry shader programs

class Shader : public MultiGlContext
{
	public:

	Shader(const std::string& vertexShaderFileName   = GLOOST_SHADER_FILE_NONE,
         const std::string& fragmentShaderFileName = GLOOST_SHADER_FILE_NONE,
	       const std::string& geometryShaderFileName = GLOOST_SHADER_FILE_NONE);


    // class destructor
		~Shader();


    // compile shader for a particular context
    /*virtual*/ bool initInContext(unsigned int contextId = 0);

	  // remove the shader program from a context
	  /*virtual*/ void removeFromContext(unsigned int contextId = 0);



    // Return the shader handle
    unsigned int getShaderHandle(unsigned int contextId = 0);


    // Set/unset the Shader for following geometry
    void set(unsigned int contextId = 0);

    // set/update Shader parameter
    void setProgramParameter(GLenum type, GLint parameter);

    // disable all GLSL shader rendering
    static void disable();


	private:

    /// file name of the vertex shader source file
    std::string _vertexShaderFileName;
    /// file name of the fragment shader source file
    std::string _fragmentShaderFileName;
    /// file name of the geometry shader source file
    std::string _geometryShaderFileName;


    /// source of the vertex shader
    const char* _vertexShaderSource;
    /// source of the fragment shader
    const char* _fragmentShaderSource;
    /// source of the geometry shader
    const char* _geometryShaderSource;


    // default vertex shader source
    const char* _nullVertexSource;
    // default fragment shader source
    const char* _nullFragmentSource;
    // default geometry shader source
    const char* _nullGeometrySource;

    // name of the shader
    // std::string _shaderName;

    // Loads a shader source file
    char* loadSourceFile (const std::string& fileName);

    // validates a shader program by using glGetShaderInfoLog
	void validateShader(GLuint shaderHandle, const std::string& shaderFileName);

	// Program parameter
    std::map<GLenum, int>_programParameter;
};


} // namespace gloost

#endif // H_GLOOST_SHADER
