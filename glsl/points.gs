//#version 140
#extension GL_EXT_gpu_shader4 : enable
#extension GL_EXT_geometry_shader4 : enable
///////////////////////////////////////////////////////////////////////////////
// input
///////////////////////////////////////////////////////////////////////////////
varying in vec2 geo_texcoord[];
varying in vec3 geo_pos_es[];
varying in vec3 geo_pos_cs[];
varying in float geo_depth[];
varying in float geo_lateral_quality[];
///////////////////////////////////////////////////////////////////////////////
// output
///////////////////////////////////////////////////////////////////////////////
varying out vec2  pass_texcoord;
varying out vec3  pass_pos_es;
varying out vec3  pass_pos_cs;

varying out float pass_depth;
varying out float pass_lateral_quality;
varying out vec3  pass_normal_es;

///////////////////////////////////////////////////////////////////////////////
// methods 
///////////////////////////////////////////////////////////////////////////////
const vec3 bbx_max = vec3( 1.,2.2,   1.);
const vec3 bbx_min = vec3(-1.,0.0, -1.);

bool clip(vec3 p){
  if(p.x < bbx_min.x ||
     p.y < bbx_min.y ||
     p.z < bbx_min.z ||
     p.x > bbx_max.x ||
     p.y > bbx_max.y ||
     p.z > bbx_max.z){
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
  gl_Position   = gl_PositionIn[0];

  float dist = length(gl_ModelViewMatrix * vec4(geo_pos_cs[0], 1.0));
  gl_PointSize  = 5.0f / dist;
  
  EmitVertex();
}