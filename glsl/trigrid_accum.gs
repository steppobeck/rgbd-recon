#version 150

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
///////////////////////////////////////////////////////////////////////////////
// input
in vec2 geo_texcoord[];
in vec3 geo_pos_es[];
in vec3 geo_pos_cs[];
in float geo_depth[];
in float geo_lateral_quality[];

uniform float min_length;
///////////////////////////////////////////////////////////////////////////////
// output
out vec2  pass_texcoord;
out vec3  pass_pos_es;
out vec3  pass_pos_cs;
out float pass_sq_area_cs;
out float pass_depth;
out float pass_lateral_quality;
out vec3  pass_normal_es;

///////////////////////////////////////////////////////////////////////////////
// methods 
bool validSurface(vec3 a, vec3 b, vec3 c,
                  float depth_a, float depth_b, float depth_c)
{
  float avg_depth = (depth_a + depth_b + depth_c)/3.0;
  float baselength = 0.005;
  float l = min_length * avg_depth + baselength;	

  if((length(a) > l) || (length(b) > l) || (length(c) > l)) {
    return false;
  }

  if(depth_a < 0.1 || depth_b < 0.1 || depth_c < 0.1) {
    return false;
  }

  return true;
}
///////////////////////////////////////////////////////////////////////////////
// main
void main() {

  float depth_a = geo_depth[0];
  float depth_b = geo_depth[1];
  float depth_c = geo_depth[2];

  vec3 a_cs = geo_pos_cs[1] - geo_pos_cs[0];
  vec3 b_cs = geo_pos_cs[2] - geo_pos_cs[0];
  vec3 c_cs = geo_pos_cs[2] - geo_pos_cs[1];

  bool valid = validSurface(a_cs, b_cs, c_cs, depth_a, depth_b, depth_c);

  if (valid) {
    vec3 a = geo_pos_es[1] - geo_pos_es[0];
    vec3 b = geo_pos_es[2] - geo_pos_es[0];
    vec3 c = geo_pos_es[2] - geo_pos_es[1];

    vec3 tri_normal = normalize(cross (a, b));
      
    for(int i = 0; i < 3; i++) {
      pass_texcoord      = geo_texcoord[i];
      pass_pos_es        = geo_pos_es[i];
      pass_pos_cs        = geo_pos_cs[i];
      pass_lateral_quality = geo_lateral_quality[i];
      pass_depth         = geo_depth[i];
      pass_normal_es     = tri_normal;
      
      gl_Position   = gl_in[i].gl_Position;
      
      EmitVertex();
    }
    EndPrimitive();
  }
}
