#version 150

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
///////////////////////////////////////////////////////////////////////////////
// input
///////////////////////////////////////////////////////////////////////////////
in vec2 to_geom_texture_coord[];
in vec3 to_geom_pos_es[];
in vec3 to_geom_pos_cs[];
in float to_geom_depth[];
in float to_geom_lateral_quality[];

uniform float min_length;// = 0.0125;
///////////////////////////////////////////////////////////////////////////////
// output
///////////////////////////////////////////////////////////////////////////////
out vec2  texture_coord;
out vec3  pos_es;
out vec3  pos_cs;
out float sq_area_cs;

out float depth;
out float lateral_quality;
out vec3  normal_es;
///////////////////////////////////////////////////////////////////////////////
// functions 
///////////////////////////////////////////////////////////////////////////////
bool validSurface(vec3 a, vec3 b, vec3 c,
                  float depth_a, float depth_b, float depth_c)
{
  float avg_depth = (depth_a + depth_b + depth_c)/3.0;
  float baselength = 0.005;
  float l = min_length * avg_depth + baselength;  

  // all position differences must be smaller than l
  return (length(a) < l) && (length(b) < l) && (length(c) < l);
}

float calcAreaSQ(vec3 a, vec3 b, vec3 c) {
  vec3 ab = b - a;
  vec3 ac = c - a;
  vec3 cc = cross(ab,ac);
  return dot(cc, cc);
}
///////////////////////////////////////////////////////////////////////////////
// main
///////////////////////////////////////////////////////////////////////////////
void main() {
  float depth_a = to_geom_depth[0];
  float depth_b = to_geom_depth[1];
  float depth_c = to_geom_depth[2];

  vec3 a_cs = to_geom_pos_cs[1] - to_geom_pos_cs[0];
  vec3 b_cs = to_geom_pos_cs[2] - to_geom_pos_cs[0];
  vec3 c_cs = to_geom_pos_cs[2] - to_geom_pos_cs[1];

  bool valid = validSurface(a_cs, b_cs, c_cs, depth_a, depth_b, depth_c);

  if (valid){
    vec3 a = to_geom_pos_es[1] - to_geom_pos_es[0];
    vec3 b = to_geom_pos_es[2] - to_geom_pos_es[0];
    vec3 c = to_geom_pos_es[2] - to_geom_pos_es[1];

    vec3 tri_normal = normalize(cross (a, b));

    float sq_area_in_cs      = calcAreaSQ(to_geom_pos_cs[0], to_geom_pos_cs[1], to_geom_pos_cs[2]);
      
    for(int i = 0; i < 3; i++){
      texture_coord = to_geom_texture_coord[i];
      pos_es        = to_geom_pos_es[i];
      pos_cs        = to_geom_pos_cs[i];
      sq_area_cs    = sq_area_in_cs;
      lateral_quality = to_geom_lateral_quality[i];
      depth         = to_geom_depth[i];
      normal_es     = tri_normal;
      
      gl_Position   = gl_in[i].gl_Position;
      
      EmitVertex();
    }
    EndPrimitive();
  }
}