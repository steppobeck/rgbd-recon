#version 150

layout(points) in;
layout(points, max_vertices = 1) out;
///////////////////////////////////////////////////////////////////////////////
// input
///////////////////////////////////////////////////////////////////////////////
flat in vec2 geo_texcoord[];
flat in vec3 geo_pos_es[];
flat in vec3 geo_pos_cs[];
flat in float geo_depth[];
flat in float geo_lateral_quality[];
///////////////////////////////////////////////////////////////////////////////
// output
///////////////////////////////////////////////////////////////////////////////
flat out vec2  pass_texcoord;
flat out vec3  pass_pos_es;
flat out vec3  pass_pos_cs;
flat out float pass_depth;
flat out float pass_lateral_quality;
flat out vec3  pass_normal_es;
flat out vec4  pass_glpos;

///////////////////////////////////////////////////////////////////////////////
// methods 
///////////////////////////////////////////////////////////////////////////////
uniform vec3 bbox_max;
uniform vec3 bbox_min;

bool clip(vec3 p){
  if(p.x < bbox_min.x ||
     p.y < bbox_min.y ||
     p.z < bbox_min.z ||
     p.x > bbox_max.x ||
     p.y > bbox_max.y ||
     p.z > bbox_max.z){
    return true;
  }
  return false;
}
///////////////////////////////////////////////////////////////////////////////
// main
///////////////////////////////////////////////////////////////////////////////
void main() {

  if(clip(geo_pos_cs[0]) || geo_depth[0] <= -1.0f) {
    return;
  }

  pass_texcoord = geo_texcoord[0];
  pass_pos_es        = geo_pos_es[0];
  pass_pos_cs        = geo_pos_cs[0];
  pass_lateral_quality = geo_lateral_quality[0];
  pass_depth         = geo_depth[0];
  pass_normal_es     = vec3(1.0f);
  gl_Position   = gl_in[0].gl_Position;
  pass_glpos = gl_Position;

  float dist = length(geo_pos_es[0]);
  gl_PointSize  = 10.0f / dist;
  
  EmitVertex();
}