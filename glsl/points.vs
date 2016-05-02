#version 130
#extension GL_EXT_texture_array : enable

in vec2 in_position;

uniform sampler2DArray kinect_depths;
uniform sampler3D cv_xyz;
uniform sampler3D cv_uv;

uniform float cv_min_d;
uniform float cv_max_d;
uniform vec2 tex_size_inv;
uniform int layer;

out vec2 geo_texcoord;
out vec3 geo_pos_es;
out vec3 geo_pos_cs;
out float geo_depth;

bool is_outside(float d){
  return (d < cv_min_d) || (d > cv_max_d);
}

void main() {

  vec3 coords = vec3(in_position,layer);
  float depth = texture2DArray(kinect_depths, coords).r;

  if (is_outside(depth)) {
    depth = -1.0f;
  }

  // lookup from calibvolume
  float d_idx = (depth - cv_min_d)/(cv_max_d - cv_min_d);

  geo_pos_cs        = texture(cv_xyz, vec3(in_position, d_idx)).rgb;
  geo_pos_es        = (gl_ModelViewMatrix * vec4(geo_pos_cs, 1.0)).xyz;
  geo_texcoord      = texture(cv_uv,  vec3(in_position, d_idx)).rg;
  geo_depth         = depth;

  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(geo_pos_cs, 1.0);
}