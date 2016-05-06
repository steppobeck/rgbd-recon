
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



#ifndef GLOOST_UNIFORMSET_H
#define GLOOST_UNIFORMSET_H



/// gloost system includes
#include <UniformTypes.h>
#include <glErrorUtil.h>
#include <Shader.h>



/// cpp includes
#include <glbinding/gl/gl.h>
using namespace gl;
#include <GL/glut.h>
#include <string>
#include <map>
#include <vector>


namespace gloost
{


 ///  Set of GLSL shader uniform variables to configure a shader with

class UniformSet
{
	public:
    /// class constructor
    UniformSet();
    /// class destructor
    ~UniformSet();



    /// Add a float value to the shader
    void  set_float (const std::string uniformName, float value);
    ///
    float  get_float (const std::string uniformName);

    /// Add a vec2 to the shader
    void  set_vec2 (const std::string uniformName, vec2 value);
    ///
    vec2 get_vec2 (const std::string uniformName);

    /// Add a vec3 to the shader
    void set_vec3 (const std::string uniformName, vec3 value);
    ///
    vec3 get_vec3 (const std::string uniformName);

    /// Add a vec4 to the shader
    void set_vec4 (const std::string uniformName, vec4 value);
    ///
    vec4 get_vec4 (const std::string uniformName);



    /// Add a integer to the shader
    void set_int(const std::string uniformName, int value);
    ///
    int get_int(const std::string uniformName);

    /// Add a ivec2 to the shader
    void  set_ivec2 (const std::string uniformName, ivec2 value);
    ///
    ivec2 get_ivec2 (const std::string uniformName);

    /// Add a ivec3 to the shader
    void set_ivec3 (const std::string uniformName, ivec3 value);
    ///
    ivec3 get_ivec3 (const std::string uniformName);

    /// Add a ivec4 to the shader
    void set_ivec4 (const std::string uniformName, ivec4 value);
    ///
    ivec4 get_ivec4 (const std::string uniformName);



    /// Add a bool to the shader
    void set_bool(const std::string uniformName, bool value);
    ///
    bool get_bool(const std::string uniformName);

    /// Add a bvec2 to the shader
    void  set_bvec2 (const std::string uniformName, bvec2 value);
    ///
    bvec2 get_bvec2 (const std::string uniformName);

    /// Add a bvec3 to the shader
    void set_bvec3 (const std::string uniformName, bvec3 value);
    ///
    bvec3 get_bvec3 (const std::string uniformName);

    /// Add a bvec4 to the shader
    void set_bvec4 (const std::string uniformName, bvec4 value);
    ///
    bvec4 get_bvec4 (const std::string uniformName);



    /// Add a mat2 to the shader
    void set_mat2 (const std::string uniformName, mat2 value);
    ///
    mat2 get_mat2 (const std::string uniformName);

    /// Add a mat3 to the shader
    void set_mat3 (const std::string uniformName, mat3 value);
    ///
    mat3 get_mat3 (const std::string uniformName);

    /// Add a mat4 to the shader
    void set_mat4 (const std::string uniformName, mat4 value);
    ///
    mat4 get_mat4 (const std::string uniformName);

    /// Add a vector of mat4 to the shader
    void set_mat4v(const std::string uniformName, const std::vector<mat4>& value);


    /// Add a sampler1D to the shader
    void set_sampler1D (const std::string uniformName, sampler1D value);
    ///
    sampler1D get_sampler1D (const std::string uniformName);

    /// Add a sampler2D to the shader (this is the id from the gloost::TextureManager)
    void set_sampler2D (const std::string uniformName, sampler2D value);
    ///
    sampler2D get_sampler2D (const std::string uniformName);

    /// Add a sampler3D to the shader
    void set_sampler3D (const std::string uniformName, sampler3D value);
    ///
    sampler3D get_sampler3D (const std::string uniformName);



    /// Add a samplerCube to the shader
    void set_samplerCube (const std::string uniformName, samplerCube value);
    ///
    samplerCube get_samplerCube (const std::string uniformName);



    /// Add a sampler1DShadow to the shader
    void set_sampler1DShadow (const std::string uniformName, sampler1DShadow value);
    ///
    sampler1DShadow get_sampler1DShadow (const std::string uniformName);

    /// Add a sampler2DShadow to the shader
    void set_sampler2DShadow (const std::string uniformName, sampler2DShadow value);
    ///
    sampler2DShadow get_sampler2DShadow (const std::string uniformName);



    /// set uniforms to shader. uniform names will have prefix+name as name in the shader
    void applyToShader(const unsigned int shaderHandle, unsigned int contextId = 0);

    /// set uniforms to shader. uniform names will have prefix+name as name in the shader
    void applyToShader(Shader* shader, unsigned int contextId = 0);

    /// set uniforms to shader. This uniforms will accumulate to existing uniforms in the state
    void addToShader(const unsigned int shaderHandle, unsigned int contextId = 0);



	private:

    /// Uniforms
    std::map<std::string, float>     _map_float;
    std::map<std::string, vec2>      _map_vec2;
    std::map<std::string, vec3>      _map_vec3;
    std::map<std::string, vec4>      _map_vec4;

    std::map<std::string, int>       _map_int;
    std::map<std::string, ivec2>     _map_ivec2;
    std::map<std::string, ivec3>     _map_ivec3;
    std::map<std::string, ivec4>     _map_ivec4;

    std::map<std::string, bool>      _map_bool;
    std::map<std::string, bvec2>     _map_bvec2;
    std::map<std::string, bvec3>     _map_bvec3;
    std::map<std::string, bvec4>     _map_bvec4;

    std::map<std::string, mat2>     _map_mat2;
    std::map<std::string, mat3>     _map_mat3;
    std::map<std::string, mat4>     _map_mat4;
    std::map<std::string, std::vector<mat4> >  _map_mat4v;

    std::map<std::string, sampler1D>  _map_sampler1D;
    std::map<std::string, sampler2D>  _map_sampler2D;
    std::map<std::string, sampler3D>  _map_sampler3D;

    std::map<std::string, samplerCube> _map_samplerCube;

    std::map<std::string, sampler1DShadow> _map_sampler1DShadow;
    std::map<std::string, sampler2DShadow> _map_sampler2DShadow;


    //
    static unsigned int _current_texture_offset;



  protected:

    ///


};


} // namespace gloost


#endif // GLOOST_UNIFORMSET_H


