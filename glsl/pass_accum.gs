/*

author: 2008 by Stephan Beck
mail:   stephan@pixelstars.de

*/

//#version 140
#extension GL_EXT_gpu_shader4 : enable
#extension GL_EXT_geometry_shader4 : enable

uniform float min_length;// = 0.0125;

varying in vec2 tex_coord[];
varying in vec3 pos_es[];
varying in vec3 pos_d[];
varying in vec3 pos_ws[];

varying in float  depth[];
varying in vec2  ddepth[];

varying out vec2 tex_coord_int;
varying out vec3 pos_es_int;
varying out vec3 pos_d_int;
varying out vec3 pos_ws_int;
varying out float depth_int;
varying out vec2  ddepth_int;


bool validSurface(vec3 a, vec3 b, vec3 c){
  float avg_depth = (depth[0] + depth[1] + depth[2])/3.0;
  float baselength = 0.005;
  float l = min_length * avg_depth + baselength;
  if((length(a) > l) || (length(b) > l) || (length(c) > l)){
    return false;
  }
  if(depth[0] < 0.1 || depth[1] < 0.1 || depth[2] < 0.1)
  {
        return false;
  }
#if 0
  if(pos_ws[0].y < 0.03 || pos_ws[1].y < 0.03 || pos_ws[2].y < 0.03)
  {
        return false;
  }
#endif
  return true;
}

void main(void){


  vec3 a = pos_es[1] - pos_es[0];
  vec3 b = pos_es[2] - pos_es[0];
  vec3 c = pos_es[2] - pos_es[1];
  


  if(validSurface(a,b,c)){
    gl_Position = gl_PositionIn[0];
    tex_coord_int = tex_coord[0];
    pos_es_int = pos_es[0];
    pos_d_int = pos_d[0];
    pos_ws_int = pos_ws[0];
    depth_int = depth[0];
    ddepth_int = ddepth[0];
    EmitVertex();
    
    gl_Position = gl_PositionIn[1];
    tex_coord_int = tex_coord[1];
    pos_es_int = pos_es[1];
    pos_d_int = pos_d[1];
    pos_ws_int = pos_ws[1];
    depth_int = depth[1];
    ddepth_int = ddepth[1];
    EmitVertex();
    
    gl_Position = gl_PositionIn[2];
    tex_coord_int = tex_coord[2];
    pos_es_int = pos_es[2];
    pos_d_int = pos_d[2];
    pos_ws_int = pos_ws[2];
    depth_int = depth[2];
    ddepth_int = ddepth[2];
    EmitVertex();
       
       
    EndPrimitive();
  }
  
}
