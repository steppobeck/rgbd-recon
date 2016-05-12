#version 430

in vec3 in_Position; 

uniform uint layer;
uniform sampler3D[5] cv_xyz;
uniform sampler3D[5] cv_uv;

uniform sampler3D[5] cv_xyz_inv;
uniform sampler3D[5] cv_uv_inv;

uniform sampler3D volume_tsdf;

uniform mat4 gl_ModelViewMatrix;
uniform mat4 gl_ProjectionMatrix;
uniform mat4 vol_to_world;

flat out vec3 geo_pos_volume;
flat out vec3 geo_pos_world;
flat out vec3 geo_pos_view;
flat out vec2 geo_texcoord;

void main() {
  geo_pos_volume = in_Position;
  vec3 pos_calib  = texture(cv_xyz_inv[layer], geo_pos_volume).rgb;
  vec3 pos_vol  = texture(cv_xyz[layer], pos_calib).rgb;
  // pos_calib = geo_pos_volume;
  geo_pos_world  = (vol_to_world * vec4(geo_pos_volume, 1.0)).xyz;
  // geo_pos_world  = (vol_to_world * vec4(pos_vol, 1.0)).xyz;
  // geo_pos_world  = (vec4(pos_vol, 1.0)).xyz;s

  geo_pos_view   = (gl_ModelViewMatrix * vec4(geo_pos_world, 1.0)).xyz;
  // geo_texcoord   = texture(cv_uv[layer], pos_calib).xy;
  // geo_texcoord   = texture(cv_uv_inv[layer], geo_pos_volume).xy;
  
  float val = texture(volume_tsdf, geo_pos_volume).r;
  geo_texcoord = vec2(val, val);
  gl_Position = gl_ProjectionMatrix * vec4(geo_pos_view, 1.0);
  // gl_PointSize = 3.0f;
}