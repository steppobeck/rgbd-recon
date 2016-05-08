#version 430

in vec3 in_Position; 

uniform uint layer;
uniform sampler3D[5] cv_xyz;
uniform sampler3D[5] cv_uv;

uniform sampler3D[5] cv_xyz2;
uniform sampler3D[5] cv_uv2;

uniform mat4 gl_ModelViewMatrix;
uniform mat4 gl_ProjectionMatrix;

flat out vec3 geo_pos_volume;
flat out vec3 geo_pos_world;
flat out vec3 geo_pos_view;
flat out vec2 geo_texcoord;

void main() {
  geo_pos_volume = in_Position;
  vec3 pos_calib  = texture(cv_xyz2[layer], geo_pos_volume).rgb;
  // pos_calib = geo_pos_volume;
  geo_pos_world  = texture(cv_xyz[layer], pos_calib).rgb;

  geo_pos_view   = (gl_ModelViewMatrix * vec4(geo_pos_world, 1.0)).xyz;
  // geo_texcoord   = texture(cv_uv[layer], pos_calib).xy;
  geo_texcoord   = texture(cv_uv2[layer], geo_pos_volume).xy;
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(geo_pos_world, 1.0);
}