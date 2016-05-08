#version 430

in vec3 in_Position; 

uniform uint layer;
uniform sampler3D[5] cv_xyz;
uniform sampler3D[5] cv_uv;

layout(rgba32f) uniform image3D volume_xyz;
layout(rg32f) uniform image3D volume_uv;

uniform mat4 gl_ModelViewMatrix;
uniform mat4 gl_ProjectionMatrix;
uniform mat4 world_to_vol;
uniform uvec3 volume_res;

flat out vec3 geo_pos_volume;
flat out vec3 geo_pos_world;
flat out vec3 geo_pos_view;
flat out vec2 geo_texcoord;

void main() {
  geo_pos_volume = in_Position;
  geo_pos_world  = texture(cv_xyz[layer], geo_pos_volume).rgb;
  geo_pos_view   = (gl_ModelViewMatrix * vec4(geo_pos_world, 1.0)).xyz;
  geo_texcoord   = texture(cv_uv[layer],  geo_pos_volume).rg;
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(geo_pos_world, 1.0);
  // gl_PointSize = 3.0f;
  vec3 pos_vol = (world_to_vol * vec4(geo_pos_world, 1.0f)).xyz; 
  ivec3 ipos_vol = ivec3(round(pos_vol * volume_res));

  imageStore(volume_xyz, ipos_vol, vec4(geo_pos_volume, 0.0f));
  imageStore(volume_uv, ipos_vol, vec4(geo_texcoord, 0.0f, 0.0f));
}