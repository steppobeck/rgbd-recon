#version 430
#extension GL_EXT_texture_array : enable

in vec2 in_Position; 

uniform sampler2DArray kinect_depths;
uniform sampler2DArray kinect_qualities;
uniform sampler3D[5] cv_xyz;
uniform sampler3D[5] cv_uv;

uniform mat4 gl_ModelViewMatrix;
uniform mat4 gl_ProjectionMatrix;

layout (std430, binding = 0) buffer LimitBuffer
{
  float[5] cv_min_d;
  float[5] cv_max_d;
};

uniform uint layer;

out vec2 geo_texcoord;
out vec3 geo_pos_es;
out vec3 geo_pos_cs;
out float geo_depth;
out float geo_lateral_quality;

void main() {

  vec3 coords = vec3(in_Position,layer);
  float depth = texture2DArray(kinect_depths, coords).r;

  // lookup from calibvolume
  float d_idx = (depth - cv_min_d[layer])/(cv_max_d[layer] - cv_min_d[layer]);

  geo_pos_cs        = texture(cv_xyz[layer], vec3(in_Position, d_idx)).rgb;
  geo_pos_es        = (gl_ModelViewMatrix * vec4(geo_pos_cs, 1.0)).xyz;
  geo_texcoord      = texture(cv_uv[layer],  vec3(in_Position, d_idx)).rg;
  geo_depth         = depth;
  geo_lateral_quality = texture2DArray(kinect_qualities, coords).r;

  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(geo_pos_cs, 1.0);
}