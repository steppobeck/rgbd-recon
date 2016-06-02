
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
#include <UniformSet.h>
#include <TextureManager.h>
#include <Shader.h>



/// cpp includes




namespace gloost
{



/* static */
unsigned int UniformSet::_current_texture_offset = 0;



///////////////////////////////////////////////////////////////////////////////


 /// class constructor

UniformSet::UniformSet():
   _map_float(),
   _map_vec2(),
   _map_vec3(),
   _map_vec4(),
   _map_int(),
   _map_ivec2(),
   _map_ivec3(),
   _map_ivec4(),
   _map_bool(),
   _map_bvec2(),
   _map_bvec3(),
   _map_bvec4(),
   _map_mat2(),
   _map_mat3(),
   _map_mat4(),
   _map_mat4v(),
   _map_sampler1D(),
   _map_sampler2D(),
   _map_sampler3D(),
   _map_samplerCube(),
   _map_sampler1DShadow(),
   _map_sampler2DShadow()

{
	// insert your code here
}


///////////////////////////////////////////////////////////////////////////////


 /// class destructor

UniformSet::~UniformSet()
{
  /// Uniforms
  _map_float.clear();
  _map_vec2.clear();
  _map_vec3.clear();
  _map_vec4.clear();
  _map_int.clear();
  _map_ivec2.clear();
  _map_ivec3.clear();
  _map_ivec4.clear();
  _map_bool.clear();
  _map_bvec2.clear();
  _map_bvec3.clear();
  _map_bvec4.clear();
  _map_mat2.clear();
  _map_mat3.clear();
  _map_mat4.clear();


  // TextureManager
  TextureManager* texManager = TextureManager::getInstance();



  _map_sampler1D.clear();



  // drop all texture references
  std::map<std::string, sampler2D>::iterator iter = _map_sampler2D.begin();

  for ( ; iter != _map_sampler2D.end(); ++iter)
  {
    texManager->dropReference((*iter).second.handle);
  }
  _map_sampler2D.clear();



  _map_sampler3D.clear();
  _map_samplerCube.clear();
  _map_sampler1DShadow.clear();
  _map_sampler2DShadow.clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a float value to the shader

void
UniformSet::set_float(const std::string uniformName, float value)
{
  _map_float[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

float
UniformSet::get_float(const std::string uniformName)
{
  std::map<std::string, float>::iterator pos = _map_float.find(uniformName);

  if (pos == _map_float.end())
  {
    return 0.0;
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a vec2 value to the shader

void
UniformSet::set_vec2(const std::string uniformName, vec2 value)
{
  _map_vec2[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

vec2
UniformSet::get_vec2(const std::string uniformName)
{
  std::map<std::string, vec2>::iterator pos = _map_vec2.find(uniformName);

  if (pos == _map_vec2.end())
  {
    return vec2();
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a vec3 value to the shader

void
UniformSet::set_vec3(const std::string uniformName, vec3 value)
{
  _map_vec3[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

vec3
UniformSet::get_vec3(const std::string uniformName)
{
  std::map<std::string, vec3>::iterator pos = _map_vec3.find(uniformName);

  if (pos == _map_vec3.end())
  {
    return vec3();
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a vec4 value to the shader

void
UniformSet::set_vec4(const std::string uniformName, vec4 value)
{
  _map_vec4[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

vec4
UniformSet::get_vec4(const std::string uniformName)
{
  std::map<std::string, vec4>::iterator pos = _map_vec4.find(uniformName);

  if (pos == _map_vec4.end())
  {
    return vec4();
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a int value to the shader

void
UniformSet::set_int(const std::string uniformName, int value)
{
  _map_int[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

int
UniformSet::get_int(const std::string uniformName)
{
  std::map<std::string, int>::iterator pos = _map_int.find(uniformName);

  if (pos == _map_int.end())
  {
    return 0;
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a ivec2 value to the shader

void
UniformSet::set_ivec2(const std::string uniformName, ivec2 value)
{
  _map_ivec2[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

ivec2
UniformSet::get_ivec2(const std::string uniformName)
{
  std::map<std::string, ivec2>::iterator pos = _map_ivec2.find(uniformName);

  if (pos == _map_ivec2.end())
  {
    return ivec2();
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a ivec3 value to the shader

void
UniformSet::set_ivec3(const std::string uniformName, ivec3 value)
{
  _map_ivec3[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

ivec3
UniformSet::get_ivec3(const std::string uniformName)
{
  std::map<std::string, ivec3>::iterator pos = _map_ivec3.find(uniformName);

  if (pos == _map_ivec3.end())
  {
    return ivec3();
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a ivec4 value to the shader

void
UniformSet::set_ivec4(const std::string uniformName, ivec4 value)
{
  _map_ivec4[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

ivec4
UniformSet::get_ivec4(const std::string uniformName)
{
  std::map<std::string, ivec4>::iterator pos = _map_ivec4.find(uniformName);

  if (pos == _map_ivec4.end())
  {
    return ivec4();
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a bool value to the shader

void
UniformSet::set_bool(const std::string uniformName, bool value)
{
  _map_bool[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

bool
UniformSet::get_bool(const std::string uniformName)
{
  std::map<std::string, bool>::iterator pos = _map_bool.find(uniformName);

  if (pos == _map_bool.end())
  {
    return 0;
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a bvec2 value to the shader

void
UniformSet::set_bvec2(const std::string uniformName, bvec2 value)
{
  _map_bvec2[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

bvec2
UniformSet::get_bvec2(const std::string uniformName)
{
  std::map<std::string, bvec2>::iterator pos = _map_bvec2.find(uniformName);

  if (pos == _map_bvec2.end())
  {
    return bvec2();
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a bvec3 value to the shader

void
UniformSet::set_bvec3(const std::string uniformName, bvec3 value)
{
  _map_bvec3[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

bvec3
UniformSet::get_bvec3(const std::string uniformName)
{
  std::map<std::string, bvec3>::iterator pos = _map_bvec3.find(uniformName);

  if (pos == _map_bvec3.end())
  {
    return bvec3();
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a bvec4 value to the shader

void
UniformSet::set_bvec4(const std::string uniformName, bvec4 value)
{
  _map_bvec4[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

bvec4
UniformSet::get_bvec4(const std::string uniformName)
{
  std::map<std::string, bvec4>::iterator pos = _map_bvec4.find(uniformName);

  if (pos == _map_bvec4.end())
  {
    return bvec4();
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a mat2 value to the shader

void
UniformSet::set_mat2(const std::string uniformName, mat2 value)
{
  _map_mat2[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

mat2
UniformSet::get_mat2(const std::string uniformName)
{
  std::map<std::string, mat2>::iterator pos = _map_mat2.find(uniformName);

  if (pos == _map_mat2.end())
  {
    return mat2();
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a mat3 value to the shader

void
UniformSet::set_mat3(const std::string uniformName, mat3 value)
{
  _map_mat3[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

mat3
UniformSet::get_mat3(const std::string uniformName)
{
  std::map<std::string, mat3>::iterator pos = _map_mat3.find(uniformName);

  if (pos == _map_mat3.end())
  {
    return mat3();
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a mat4 value to the shader

void
UniformSet::set_mat4(const std::string uniformName, mat4 value)
{
  _map_mat4[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

mat4
UniformSet::get_mat4(const std::string uniformName)
{
  std::map<std::string, mat4>::iterator pos = _map_mat4.find(uniformName);

  if (pos == _map_mat4.end())
  {
    return mat4();
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a vector of mat4 value to the shader

void
UniformSet::set_mat4v(const std::string uniformName, const std::vector<mat4>& value)
{
  _map_mat4v[uniformName] = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////

 /// Add a sampler1D value to the shader

void
UniformSet::set_sampler1D(const std::string uniformName, sampler1D value)
{
  _map_sampler1D[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

sampler1D
UniformSet::get_sampler1D(const std::string uniformName)
{
  std::map<std::string, sampler1D>::iterator pos = _map_sampler1D.find(uniformName);

  if (pos == _map_sampler1D.end())
  {
    return sampler1D();
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a sampler2D value to the shader

void
UniformSet::set_sampler2D(const std::string uniformName, sampler2D value)
{
  _map_sampler2D[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

sampler2D
UniformSet::get_sampler2D(const std::string uniformName)
{
  std::map<std::string, sampler2D>::iterator pos = _map_sampler2D.find(uniformName);

  if (pos == _map_sampler2D.end())
  {
    return sampler2D();
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a sampler3D value to the shader

void
UniformSet::set_sampler3D(const std::string uniformName, sampler3D value)
{
  _map_sampler3D[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

sampler3D
UniformSet::get_sampler3D(const std::string uniformName)
{
  std::map<std::string, sampler3D>::iterator pos = _map_sampler3D.find(uniformName);

  if (pos == _map_sampler3D.end())
  {
    return sampler3D();
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a samplerCube value to the shader

void
UniformSet::set_samplerCube(const std::string uniformName, samplerCube value)
{
  _map_samplerCube[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

samplerCube
UniformSet::get_samplerCube(const std::string uniformName)
{
  std::map<std::string, samplerCube>::iterator pos = _map_samplerCube.find(uniformName);

  if (pos == _map_samplerCube.end())
  {
    return samplerCube();
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a sampler1DShadow value to the shader

void
UniformSet::set_sampler1DShadow(const std::string uniformName, sampler1DShadow value)
{
  _map_sampler1DShadow[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

sampler1DShadow
UniformSet::get_sampler1DShadow(const std::string uniformName)
{
  std::map<std::string, sampler1DShadow>::iterator pos = _map_sampler1DShadow.find(uniformName);

  if (pos == _map_sampler1DShadow.end())
  {
    return sampler1DShadow();
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////


 /// Add a sampler2DShadow value to the shader

void
UniformSet::set_sampler2DShadow(const std::string uniformName, sampler2DShadow value)
{
  _map_sampler2DShadow[uniformName] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////


 ///

sampler2DShadow
UniformSet::get_sampler2DShadow(const std::string uniformName)
{
  std::map<std::string, sampler2DShadow>::iterator pos = _map_sampler2DShadow.find(uniformName);

  if (pos == _map_sampler2DShadow.end())
  {
    return sampler2DShadow();
  }
  else
  {
    return (*pos).second;
  }
}


///////////////////////////////////////////////////////////////////////////////


 /// set uniforms to shader

void
UniformSet::applyToShader(const unsigned int shaderHandle, unsigned int contextId)
{
  _current_texture_offset = 0;
  addToShader(shaderHandle, contextId);
}


///////////////////////////////////////////////////////////////////////////////


 /// set uniforms to shader

void
UniformSet::applyToShader(Shader* shader, unsigned int contextId)
{
  _current_texture_offset = 0;
  addToShader(shader->getShaderHandle(), contextId);
}


///////////////////////////////////////////////////////////////////////////////


 /// set uniforms to shader

void
UniformSet::addToShader(const unsigned int shaderHandle, unsigned int contextId)
{

  // TextureManager
  TextureManager* texManager = TextureManager::getInstance();


  // Uniform handle
  unsigned int uniformPos = 0;


  // Bind floats
  if(_map_float.size())
  {
    std::map<std::string, float>::const_iterator index = _map_float.begin();
    std::map<std::string, float>::const_iterator end   = _map_float.end();

    for (; index != end; ++index)
    {
      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniform1f(uniformPos, (*index).second);
    }
  }




  // Bind vec2
  if(_map_vec2.size())
  {
    std::map<std::string, vec2>::const_iterator index = _map_vec2.begin();
    std::map<std::string, vec2>::const_iterator end   = _map_vec2.end();

    for (; index != end; ++index)
    {
      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniform2f(uniformPos, (*index).second.u, (*index).second.v);
    }
  }

  // Bind vec3
  if(_map_vec3.size())
  {
    std::map<std::string, vec3>::const_iterator index = _map_vec3.begin();
    std::map<std::string, vec3>::const_iterator end   = _map_vec3.end();

    for (; index != end; ++index)
    {
      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniform3f(uniformPos, (*index).second.x, (*index).second.y, (*index).second.z);
    }
  }

  // Bind vec4
  if(_map_vec4.size())
  {
    std::map<std::string, vec4>::const_iterator index = _map_vec4.begin();
    std::map<std::string, vec4>::const_iterator end   = _map_vec4.end();

    for (; index != end; ++index)
    {
      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniform4f(uniformPos, (*index).second.r, (*index).second.g,
                              (*index).second.b, (*index).second.a);
    }
  }


  //////////////////////////////////////////////////////////////////////


  // Bind int
  if(_map_int.size())
  {
    std::map<std::string, int>::const_iterator index = _map_int.begin();
    std::map<std::string, int>::const_iterator end   = _map_int.end();

    for (; index != end; ++index)
    {
      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniform1i(uniformPos, (*index).second);
    }
  }

  // Bind ivec2
  if(_map_ivec2.size())
  {
    std::map<std::string, ivec2>::const_iterator index = _map_ivec2.begin();
    std::map<std::string, ivec2>::const_iterator end   = _map_ivec2.end();

    for (; index != end; ++index)
    {
      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniform2i(uniformPos, (*index).second.u, (*index).second.v);
    }
  }

  // Bind ivec3
  if(_map_ivec3.size())
  {
    std::map<std::string, ivec3>::const_iterator index = _map_ivec3.begin();
    std::map<std::string, ivec3>::const_iterator end   = _map_ivec3.end();

    for (; index != end; ++index)
    {
      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniform3i(uniformPos, (*index).second.x, (*index).second.y, (*index).second.z);
    }
  }

  // Bind ivec4
  if(_map_ivec4.size())
  {
    std::map<std::string, ivec4>::const_iterator index = _map_ivec4.begin();
    std::map<std::string, ivec4>::const_iterator end   = _map_ivec4.end();

    for (; index != end; ++index)
    {
      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniform4i(uniformPos, (*index).second.r, (*index).second.g,
                              (*index).second.b, (*index).second.a);
    }
  }


  //////////////////////////////////////////////////////////////////////


  // Bind bool
  if(_map_bool.size())
  {
    std::map<std::string, bool>::const_iterator index = _map_bool.begin();
    std::map<std::string, bool>::const_iterator end   = _map_bool.end();

    for (; index != end; ++index)
    {
      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniform1i(uniformPos, (*index).second);
    }
  }

  // Bind bvec2
  if(_map_bvec2.size())
  {
    std::map<std::string, bvec2>::const_iterator index = _map_bvec2.begin();
    std::map<std::string, bvec2>::const_iterator end   = _map_bvec2.end();

    for (; index != end; ++index)
    {
      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniform2i(uniformPos, (*index).second.u, (*index).second.v);
    }
  }

  // Bind bvec3
  if(_map_bvec3.size())
  {
    std::map<std::string, bvec3>::const_iterator index = _map_bvec3.begin();
    std::map<std::string, bvec3>::const_iterator end   = _map_bvec3.end();

    for (; index != end; ++index)
    {
      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniform3i(uniformPos, (*index).second.x, (*index).second.y, (*index).second.z);
    }
  }

  // Bind bvec4
  if(_map_bvec4.size())
  {
    std::map<std::string, bvec4>::const_iterator index = _map_bvec4.begin();
    std::map<std::string, bvec4>::const_iterator end   = _map_bvec4.end();

    for (; index != end; ++index)
    {
      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniform4i(uniformPos, (*index).second.r, (*index).second.g,
                              (*index).second.b, (*index).second.a);
    }
  }


  //////////////////////////////////////////////////////////////////////


  // Bind mat2
  if(_map_mat2.size())
  {
    std::map<std::string, mat2>::const_iterator index = _map_mat2.begin();
    std::map<std::string, mat2>::const_iterator end   = _map_mat2.end();

    for (; index != end; ++index)
    {
      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniformMatrix2fv(uniformPos, 4, GL_FALSE, (GLfloat*)&((*index).second));
    }
  }

  // Bind mat3
  if(_map_mat3.size())
  {
    std::map<std::string, mat3>::const_iterator index = _map_mat3.begin();
    std::map<std::string, mat3>::const_iterator end   = _map_mat3.end();

    for (; index != end; ++index)
    {
      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniformMatrix3fv(uniformPos, 1, GL_FALSE, (GLfloat*)&(*index).second  );
    }
  }

  // Bind mat4
  if(_map_mat4.size())
  {
    std::map<std::string, mat4>::const_iterator index = _map_mat4.begin();
    std::map<std::string, mat4>::const_iterator end   = _map_mat4.end();

    for (; index != end; ++index)
    {
      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniformMatrix4fv(uniformPos, 1, GL_FALSE, (GLfloat*)&((*index).second));
    }
  }


  // Bind mat4v
  if(_map_mat4v.size())
  {
    std::map<std::string, std::vector<mat4> >::const_iterator index = _map_mat4v.begin();
    std::map<std::string, std::vector<mat4> >::const_iterator end   = _map_mat4v.end();

    for (; index != end; ++index)
    {
      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniformMatrix4fv(uniformPos, (*index).second.size() /*count*/, GL_FALSE /*transpose it?*/, (GLfloat*)&((*index).second[0]));
    }
  }

  //////////////////////////////////////////////////////////////////////


  // Bind sampler1D
  if(_map_sampler1D.size())
  {
    std::map<std::string, sampler1D>::const_iterator index = _map_sampler1D.begin();
    std::map<std::string, sampler1D>::const_iterator ende  = _map_sampler1D.end();

    for (; index != ende; index++ )
    {
      Texture* texture = texManager->getTextureWithoutRefcount((*index).second.handle);

      texture->bind(GL_TEXTURE0+_current_texture_offset, contextId);

      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniform1i(uniformPos, _current_texture_offset);

      ++_current_texture_offset;
    }
  }

  // Bind sampler2D
  if(_map_sampler2D.size())
  {
    std::map<std::string, sampler2D>::const_iterator index = _map_sampler2D.begin();
    std::map<std::string, sampler2D>::const_iterator ende  = _map_sampler2D.end();

    for (; index != ende; index++ )
    {
      Texture* texture = texManager->getTextureWithoutRefcount((*index).second.handle);

      texture->bind(GL_TEXTURE0+_current_texture_offset, contextId);

      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniform1i(uniformPos, _current_texture_offset);

      ++_current_texture_offset;
    }

  }


  // Bind sampler3D
  if(_map_sampler3D.size())
  {
    std::map<std::string, sampler3D>::const_iterator index = _map_sampler3D.begin();
    std::map<std::string, sampler3D>::const_iterator ende  = _map_sampler3D.end();

    for (; index != ende; index++ )
    {
      Texture* texture = texManager->getTextureWithoutRefcount((*index).second.handle);

      texture->bind(GL_TEXTURE0+_current_texture_offset, contextId);

      uniformPos = glGetUniformLocation (shaderHandle, (*index).first.c_str());
      glUniform1i(uniformPos, _current_texture_offset);

      ++_current_texture_offset;
    }
  }


  // Bind samplerCube
  if(_map_samplerCube.size())
  {
    std::map<std::string, samplerCube>::const_iterator index = _map_samplerCube.begin();
    std::map<std::string, samplerCube>::const_iterator ende  = _map_samplerCube.end();

    for (; index != ende; index++ )
    {
        std::cout << std::endl << "UniformSet: Bind samplerCube: This feature is not jet implemented."
                                   << "Do it yourself if you need it...";
    }
  }

  // Bind sampler1DShadow
  if(_map_sampler1DShadow.size())
  {
    std::map<std::string, sampler1DShadow>::const_iterator index = _map_sampler1DShadow.begin();
    std::map<std::string, sampler1DShadow>::const_iterator ende  = _map_sampler1DShadow.end();

    for (; index != ende; index++ )
    {
        std::cout << std::endl << "UniformSet: Bind sampler1DShadow: This feature is not jet implemented."
                                   << "Do it yourself if you need it...";
    }
  }

  // Bind sampler2DShadow
  if(_map_sampler2DShadow.size())
  {
    std::map<std::string, sampler2DShadow>::const_iterator index = _map_sampler2DShadow.begin();
    std::map<std::string, sampler2DShadow>::const_iterator ende  = _map_sampler2DShadow.end();

    for (; index != ende; index++ )
    {
        std::cout << std::endl << "UniformSet: Bind sampler2DShadow: This feature is not jet implemented."
                                   << "Do it yourself if you need it...";
    }
  }


}


///////////////////////////////////////////////////////////////////////////////

 ///





} // namespace gloost

