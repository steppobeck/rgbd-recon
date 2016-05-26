#version 420

layout(points) in;
layout(points, max_vertices = 1) out;
///////////////////////////////////////////////////////////////////////////////
// input
///////////////////////////////////////////////////////////////////////////////
flat in vec2 geo_texcoord[];
flat in vec2 geo_pos_norm[];
flat in vec3 geo_pos_es[];
flat in vec3 geo_pos_cs[];
flat in float geo_depth[];
flat in float geo_quality[];
///////////////////////////////////////////////////////////////////////////////
// output
///////////////////////////////////////////////////////////////////////////////
flat out vec2  pass_texcoord;
flat out vec2  pass_pos_norm;
flat out vec3  pass_pos_es;
flat out vec3  pass_pos_cs;
flat out float pass_depth;
flat out float pass_quality;
flat out vec3  pass_normal_es;
flat out vec4  pass_glpos;

layout (std140, binding = 1) uniform Settings {
  uint g_shade_mode;
};

#include </inc_bbox_test.glsl>
///////////////////////////////////////////////////////////////////////////////
// main
///////////////////////////////////////////////////////////////////////////////
void main() {

  if(!in_bbox(geo_pos_cs[0]) || geo_depth[0] <= -1.0f) {
    return;
  }

  pass_texcoord = geo_texcoord[0];
  pass_pos_norm = geo_pos_norm[0];
  pass_pos_es        = geo_pos_es[0];
  pass_pos_cs        = geo_pos_cs[0];
  pass_quality = geo_quality[0];
  pass_depth         = geo_depth[0];
  pass_normal_es     = vec3(1.0f);
  gl_Position   = gl_in[0].gl_Position;
  pass_glpos = gl_Position;

  float dist = length(geo_pos_es[0]);

  float max_size = 10.0f;
  // if cameras are visualized, make points smaller to prevent occlusion
  if (g_shade_mode == 3u) {
    max_size = 4.0f;
  }
  gl_PointSize = max_size / dist;
  
  EmitVertex();
}