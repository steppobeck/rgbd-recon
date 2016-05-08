#version 430
#extension GL_EXT_texture_array : enable

in vec2 in_position;

uniform sampler2DArray kinect_depths;
uniform sampler3D[5] cv_xyz;
uniform sampler3D[5] cv_uv;
uniform uint layer;

uniform mat4 gl_ModelViewMatrix;
uniform mat4 gl_ProjectionMatrix;

flat out vec2 geo_texcoord;
flat out vec3 geo_pos_es;
flat out vec3 geo_pos_cs;
flat out float geo_depth;

void main() {

  vec3 coords = vec3(in_position,layer);
  float depth = texture2DArray(kinect_depths, coords).r;

  // lookup from calibvolume
  geo_pos_cs        = texture(cv_xyz[layer], vec3(in_position, depth)).rgb;
  geo_pos_es        = (gl_ModelViewMatrix * vec4(geo_pos_cs, 1.0)).xyz;
  geo_texcoord      = texture(cv_uv[layer],  vec3(in_position, depth)).rg;
  geo_depth         = depth;

  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(geo_pos_cs, 1.0);
}