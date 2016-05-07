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
  geo_pos_world  = texture(cv_xyz2[layer], geo_pos_volume).rgb;
  geo_pos_view   = (gl_ModelViewMatrix * vec4(geo_pos_world, 1.0)).xyz;
  geo_texcoord   = texture(cv_uv2[layer],  geo_pos_volume).rg;
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(geo_pos_world, 1.0);
  // gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(in_Position, 1.0);
}