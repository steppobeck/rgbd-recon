#version 130
#extension GL_EXT_texture_array : enable

in vec2 in_Position; 

uniform sampler2DArray kinect_depths;
uniform sampler2DArray kinect_qualities;
uniform sampler3D cv_xyz;
uniform sampler3D cv_uv;

uniform float cv_min_d;
uniform float cv_max_d;
uniform int layer;

out vec2 to_geom_texture_coord;
out vec3 to_geom_pos_es;
out vec3 to_geom_pos_cs;
out float to_geom_depth;
out float to_geom_lateral_quality;

void main() {

  vec3 coords = vec3(in_Position,layer);
  float depth = texture2DArray(kinect_depths, coords).r;

  // lookup from calibvolume
  float d_idx = (depth - cv_min_d)/(cv_max_d - cv_min_d);

  to_geom_pos_cs        = texture(cv_xyz, vec3(in_Position, d_idx)).rgb;
  to_geom_pos_es        = (gl_ModelViewMatrix * vec4(to_geom_pos_cs, 1.0)).xyz;
  to_geom_texture_coord = texture(cv_uv,  vec3(in_Position, d_idx)).rg;
  to_geom_depth         = depth;
  to_geom_lateral_quality = texture2DArray(kinect_qualities, coords).r;

  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(to_geom_pos_cs, 1.0);

}