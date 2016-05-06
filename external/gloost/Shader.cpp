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
#include <Shader.h>
#include <glErrorUtil.h>
#include <gloostConfig.h>

#include <glbinding/gl/functions-patches.h>
// cpp includes
#include <string>


namespace gloost
{


/**
  \class Shader
  \brief Wrapper for GLSL shader supporting vertex, fragment and geometry shader programs

  \author Felix Weiszig, Stephan Beck
  \date   December 2009

  \remarks 1. You don't need to compile and upload the shader explicitly. This will be
           done when if you call set() the first time.
  \remarks 2. If you call disable(), all texture units will be cleared and all texture matrices
           will be set to identity.
  \remarks 3. This class supports multiple contexts
*/


///////////////////////////////////////////////////////////////////////////////////////////////


/**
  \brief Class constructur taking vertex- and fragmentshader files

  \param vertexShaderFileName file path to a vertex shader file (ASCII)
  \param fragmentShaderFileName file path to a fragment shader file (ASCII)
  \param geometryShaderFileName file path to a fragment shader file (ASCII)
*/
Shader::Shader(const std::string& vertexShaderFileName,
               const std::string& fragmentShaderFileName,
               const std::string& geometryShaderFileName):
    MultiGlContext(GLOOST_SYSTEM_NUM_RENDER_CONTEXTS),
    _vertexShaderFileName(vertexShaderFileName),
    _fragmentShaderFileName(fragmentShaderFileName),
    _geometryShaderFileName(geometryShaderFileName),
    _vertexShaderSource(0),
    _fragmentShaderSource(0),
    _geometryShaderSource(0),
    _nullVertexSource(  "void main (void) { gl_Position  = ftransform(); }"),
    _nullFragmentSource("void main (void) { gl_FragColor = vec4( 0.8, 0.1, 0.1, 1 ); }"),
    _nullGeometrySource("void main (void) { gl_Position = gl_PositionIn[0]; EmitVertex(); EndPrimitiv(); }")


{
  if (_vertexShaderFileName != GLOOST_SHADER_FILE_NONE)
  {

#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_MESSAGES
    std::cout << std::endl;
    std::cout << std::endl << "Message from Shader::Shader() on SharedResource " << getSharedResourceId() << ":";
    std::cout << std::endl << "             Loading shader files \"" << _vertexShaderFileName << "\", ";
    std::cout << std::endl << "                                  \"" << _fragmentShaderFileName << "\", ";
    std::cout << std::endl << "                                  \"" << _geometryShaderFileName << "\"";
    std::cout << std::endl;
#endif

    /// Read Shaderfiles
    _vertexShaderSource   =  loadSourceFile( _vertexShaderFileName.c_str()   );
    _fragmentShaderSource =  loadSourceFile( _fragmentShaderFileName.c_str() );

    if (geometryShaderFileName != GLOOST_SHADER_FILE_NONE)
    {
      _geometryShaderSource = loadSourceFile( _geometryShaderFileName.c_str()   );
    }


    /// Set nullshader if no source file was loaded
    if (!_vertexShaderSource || !_fragmentShaderSource)
    {

#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_WARNINGS
      std::cout << std::endl;
      std::cout << std::endl << "Warning from Shader::Shader() on SharedResource " << getSharedResourceId() << ":";
      std::cout << std::endl << "             Applying GLOOST_SHADER_FILE_NONE";
      std::cout << std::endl;
#endif

      _vertexShaderSource   = _nullVertexSource;
      _fragmentShaderSource = _nullFragmentSource;
    }
    else
    {
      if ((geometryShaderFileName != GLOOST_SHADER_FILE_NONE) && (!_geometryShaderSource))
      {
#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_WARNINGS
        std::cout << std::endl;
        std::cout << std::endl << "Warning from Shader::Shader() on SharedResource " << getSharedResourceId() << ":";
        std::cout << std::endl << "             Could NOT load geometry shader file \"" << geometryShaderFileName << "\"";
        std::cout << std::endl;
#endif
        _geometryShaderSource = _nullGeometrySource;

      }
    }
  }
  else
  {

#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_WARNINGS
    std::cout << std::endl;
    std::cout << std::endl << "Warning from Shader::Shader() on SharedResource " << getSharedResourceId() << ":";
    std::cout << std::endl << "             Applying red GLOOST_SHADER_FILE_NONE";
    std::cout << std::endl;
#endif

    _vertexShaderSource   = _nullVertexSource;
    _fragmentShaderSource = _nullFragmentSource;
    _geometryShaderSource = NULL;
  }
}




///////////////////////////////////////////////////////////////////////////////////////////////


/**
  \brief Class destructor. Removes Shader from all contexts/GPUs.
*/

Shader::~Shader()
{
  for (unsigned int i=0; i!=GLOOST_SYSTEM_NUM_RENDER_CONTEXTS; ++i)
  {
    removeFromContext(i);
  }
  _programParameter.clear();
}


////////////////////////////////////////////////////////////////////////////////////////////////


/**
  \brief Compiles the shader and links the programs for a given context

  \param contextId id of the context the shader will be generated for. Use 0 or no
         argument if you work with only one context.
*/

bool
Shader::initInContext(unsigned int contextId)
{
  // handler for the single shaders
  GLuint vs = 0;
  GLuint fs = 0;
  GLuint gs = 0;

  // return values for the compile process
  GLboolean vsCompiled = GL_FALSE;
  GLboolean fsCompiled = GL_FALSE;
  GLboolean gsCompiled = GL_FALSE;

  // iterator for program parameter
  std::map<GLenum,int>::iterator params;



#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_MESSAGES
  std::cout << std::endl;
  std::cout << std::endl << "Message from Message from Shader::initInContext() on SharedResource " << getSharedResourceId() << ":";
  std::cout << std::endl << "             Building for context " << contextId << ".";
  std::cout << std::endl;
#endif
  // remove this resource if it allready exists
  removeFromContext(contextId);


  /// create a vertex, a fragment and a geometry shader handle
  vs  = glCreateShader( GL_VERTEX_SHADER );
  fs  = glCreateShader( GL_FRAGMENT_SHADER );

  if (_geometryShaderSource)
  {
    gs  = glCreateShader( GL_GEOMETRY_SHADER_EXT );
  }


  /// send shader source to the driver
  glShaderSource(vs, 1, &_vertexShaderSource, NULL);
  glShaderSource(fs, 1, &_fragmentShaderSource, NULL);

  if (_geometryShaderSource)
  {
    glShaderSource(gs, 1, &_geometryShaderSource, NULL);
  }


  // vertex shader
  glCompileShader(vs);
  glGetShaderbv(vs, GL_COMPILE_STATUS, &vsCompiled);

  if (vsCompiled == GL_FALSE)
  {

#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_ERRORS
    std::cout << std::endl;
    std::cout << std::endl << "ERROR in shader::create() on SharedResource " << getSharedResourceId() << ":";
    std::cout << std::endl << "         Can't compile vertexshader \"" << _vertexShaderFileName << "\"";
    std::cout << std::endl;
#endif

  }

  // validate the vertex shader and print compiler log
  validateShader(vs, _vertexShaderFileName);



  // fragment shader
  glCompileShader( fs );
  glGetShaderbv(fs, GL_COMPILE_STATUS, &fsCompiled);


  if (fsCompiled == GL_FALSE)
  {

#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_ERRORS
    std::cout << std::endl;
    std::cout << std::endl << "ERROR in shader::create() on SharedResource " << getSharedResourceId() << ":";
    std::cout << std::endl << "         Cant't compile fragmentshader \"" << _fragmentShaderFileName << "\"";
    std::cout << std::endl;
#endif

  }

  // validate the fragment shader and print compiler log
  validateShader(fs, _fragmentShaderFileName);


  // geometry shader
  if (gs)
  {
    glCompileShader( gs );
    glGetShaderbv(gs, GL_COMPILE_STATUS, &gsCompiled);

    if (gsCompiled == GL_FALSE)
    {

#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_ERRORS
      std::cout << std::endl;
      std::cout << std::endl << "ERROR in shader::create() on SharedResource " << getSharedResourceId() << ":";
      std::cout << std::endl << "         Cant't compile geometryshader \"" << _geometryShaderFileName << "\"";
      std::cout << std::endl;
#endif

    }

    // validate the fragment shader and print compiler log
    validateShader(gs, _geometryShaderFileName);
  }
  else
  {
    // shader not used - set compile flag anyway
    gsCompiled = GL_TRUE;
  }// if (geometry shader exist)



  /// create a shader program and attache the compiled vertex, fragment and geometry binaries
  unsigned int shaderHandle = glCreateProgram();


  /// make a association between the context and the GL handle of this shader
  setResourceHandleForContext(shaderHandle, contextId);


  /// attache programs to shaderh handle
  glAttachShader( shaderHandle, fs );
  glAttachShader( shaderHandle, vs );

  if (gs)
  {
    glAttachShader( shaderHandle, gs );

    for ( params =_programParameter.begin(); params != _programParameter.end(); params++ )
    {
        glProgramParameteriEXT(shaderHandle, (*params).first, (*params).second);
        CheckErrorsGL(("Shader::initInContext() for Shared Resource " + gloost::toString(getSharedResourceId())).c_str());
    }

    std::cout << std::endl << "   Attached Geometry Shader        " ;
  }


  /// link programs
  glLinkProgram(shaderHandle);
  GLint success = 0;
  glGetProgramiv(shaderHandle, GL_LINK_STATUS, &success);
  if(success == 0) {
    // get log length
    GLint log_size = 0;
    glGetProgramiv(shaderHandle, GL_INFO_LOG_LENGTH, &log_size);
    // get log
    GLchar* log_buffer = (GLchar*)malloc(sizeof(GLchar) * log_size);
    glGetProgramInfoLog(shaderHandle, log_size, &log_size, log_buffer);
    
    // utils::output_log(log_buffer, paths);

    std::string error{};
    std::istringstream error_stream{log_buffer};
    while(std::getline(error_stream, error)) {
      std::cerr << error << std::endl;
    }
    // free broken shaderHandle
    glDeleteProgram(shaderHandle);
    free(log_buffer);

    // throw std::logic_error("Linking of " + paths);
    vsCompiled = GL_FALSE;
  }


#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_MESSAGES
  if (vsCompiled == GL_TRUE && fsCompiled == GL_TRUE && gsCompiled == GL_TRUE)
  {
    std::cout << std::endl;
    std::cout << std::endl << "Message from Shader::compileToProgram() on SharedResource " << getSharedResourceId() << ":";
    std::cout << std::endl << "             Successfully compiled and linked.";
    std::cout << std::endl << "               \"" << _vertexShaderFileName << "\", ";
    std::cout << std::endl << "               \"" << _fragmentShaderFileName << "\"";
    if (gs)
    {
      std::cout << ", "<< std::endl << "             \"" << _geometryShaderFileName << "\"";
    }
    std::cout << std::endl;
    return true;
  }
#endif

  return false;
}


///////////////////////////////////////////////////////////////////////////////

/**
  \brief Removes a shader program from a context

  \param contextId id of the context, the shader will be removed from. Use 0 or no
         argument if you work with only one context.

  \remarks If you call bind() on this shader, the shader will be automaticly initialized
           for the context again
*/

/*virtual*/
void
Shader::removeFromContext(unsigned int contextId)
{
  if (handleInContextExists(contextId))
  {
    glDeleteObjectARB(getResourceHandleForContext(contextId));
    setResourceHandleForContext(0, contextId);
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////

/**
  \brief Validates a shader program by using glGetShaderInfoLog

  \param shaderHandle OpenGL handle of this shader
  \param shaderFileName The name of the shader source file, just for output purpose

  \remarks On ATI cards there will be always a message that the shader was build correctly
*/
void
Shader::validateShader(GLuint shaderHandle, const std::string& shaderFileName)
{
  const unsigned int BUFFER_SIZE = 512;
  char               buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);
  GLsizei length = 0;

  glGetShaderInfoLog(shaderHandle, BUFFER_SIZE, &length, buffer);
  if (length > 0)
  {

#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_ERRORS
    std::cout << std::endl;
    std::cout << std::endl << "ERROR in Shader::validateShader on SharedResource " << getSharedResourceId() << ":";
    std::cout << std::endl << "         Compile error(s) in file \"" << shaderFileName << "\" !";
    std::cout << std::endl;
    std::cout << std::endl << buffer;
    std::cout << std::endl;
    std::cout << std::endl;
#endif

  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


/**
  \brief Returns the OpenGL shader handle for a specified context

  \param contextId The context id of the context you want the handle from. Use 0 or no
         argument if you work with only one context.

  \remarks The shader will be initialized for the context if not allready done
*/

unsigned int
Shader::getShaderHandle(unsigned int contextId)
{

  if (!handleInContextExists(contextId))
  {
    initInContext(contextId);
  }

  return getResourceHandleForContext(contextId);
}


///////////////////////////////////////////////////////////////////////////////////////////////


/**
  \brief Loads a ASCII shader source file

  \param fileName File path of the shader source file
*/

char*
Shader::loadSourceFile(const std::string& fileName)
{
  FILE *fp = 0;
  char *content = NULL;

  int count;
  struct stat dateiinfo;
  stat(fileName.c_str(), &dateiinfo);
  count = (int) dateiinfo.st_size;

  if (fileName.c_str() != NULL)
  {
    fp = fopen(fileName.c_str(),"rt");

    if (fp != NULL)
    {
      if (count > 0)
      {
        content = (char *)malloc(sizeof(char) * (count+1));
        count = fread(content,sizeof(char),count,fp);
        content[count] = '\0';
      }
      fclose(fp);
    }
    else
    {

#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_ERRORS
      std::cout << std::endl;
      std::cout << std::endl << "ERROR in shader::loadSourceFile() on SharedResource " << getSharedResourceId() << ":";
      std::cout << std::endl << "         Can't read shader source file: \"" << fileName << "\"";
      std::cout << std::endl;
#endif

      return 0;
    }
  }

  return content;
}


///////////////////////////////////////////////////////////////////////////////////////////////


/**
  \brief Binds/sets the shader into the specified GL state

  \param contextId The context id of the context you want the shader set to. Use 0 or no
         argument if you work with only one context.

  \remarks The shader will be initialized for the context if not allready done
*/
void
Shader::set(unsigned int contextId)
{
  if (!handleInContextExists(contextId))
  {
    initInContext(contextId);
  }

  // SetShader
  glUseProgram(getResourceHandleForContext(contextId));
}


///////////////////////////////////////////////////////////////////////////////////////////////


/**
  \brief Set the shader parameter

  \param type The type of the shader argument, i.e.: GL_GEOMETRY_INPUT_TYPE_EXT, GL_GEOMETRY_OUTPUT_TYPE_EXT,
              GL_GEOMETRY_VERTICES_OUT_EXT,
  \param parameter The value the shader argument. See OpenGL specification.

  \remarks 1. Only valid for geometry shader extension.
  \remarks 2. If your type is GL_GEOMETRY_INPUT_TYPE_EXT your parameter can be:
              GL_POINTS, GL_LINES, GL_LINES_ADJACENCY_EXT, GL_TRIANGLES, GL_TRIANGLES_ADJACENCY_EXT
  \remarks 3. If your type is GL_GEOMETRY_OUTPUT_TYPE_EXT your parameter can be:
              GL_POINTS, GL_LINE_STRIP, GL_TRIANGLE_STRIP
*/

void Shader::setProgramParameter(GLenum type, GLint parameter)
{
    _programParameter[type] = parameter;
}

///////////////////////////////////////////////////////////////////////////////////////////////


/**
  \brief Disable all GLSL shader rendering and fall back to the standard pipeline

  \param contextId The context id of the context you want the shader set to. Use 0 or no
         argument if you work with only one context.

  \remarks all texture units will be cleared and all texture matrices will be set to identity.
*/


/*static*/
void
Shader::disable()
{
  glUseProgram(0);

  /* 2010-03-12 thiesje: Code produce side effects if you are not using 2D-textures */
  /* 2010-03-20 otaco  : Thats not the case, but it's realy to much overhead here*/

#ifdef GLOOST_SYSTEM_CLEAR_TEXUNITS_ON_SHADER_DISABLE
  glMatrixMode(GL_TEXTURE);
  {
    int end;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &end);
    for (int i= 0; i < end; ++i)
    {
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, 0);
      glLoadIdentity();
    }
  }
  glMatrixMode(GL_MODELVIEW);
#endif /* GLOOST_SYSTEM_CLEAR_TEXUNITS_ON_SHADER_DISABLE */

}


///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace gloost
