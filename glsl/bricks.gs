#version 420
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_shader_storage_buffer_object : require

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
///////////////////////////////////////////////////////////////////////////////
// input
///////////////////////////////////////////////////////////////////////////////
in vec3 geo_Position[];

in uint geo_Id[];
///////////////////////////////////////////////////////////////////////////////
// output
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// main
///////////////////////////////////////////////////////////////////////////////
#include </bricks.glsl>

void main() {
  // uint id = bricks_ocscupied[gl_InstanceID];

  vec3 sum = geo_Position[0] + geo_Position[1] + geo_Position[2];

  if (sum.x < 1.0 || sum.x > 2.0) {
    uvec3 index = index_3d(geo_Id[0]);
    if (brick_occupied(get_id(index + ivec3(sum.x / 3.0 * 2.0 - 1.0, 0, 0)))) {
      return;
    }
  }
  else if (sum.y < 1.0 || sum.y > 2.0) {
    uvec3 index = index_3d(geo_Id[0]);
    if (brick_occupied(get_id(index + ivec3(0, sum.y / 3.0 * 2.0 - 1.0, 0)))) {
      return;
    }
  }
  else if (sum.z < 1.0 || sum.z > 2.0) {
    uvec3 index = index_3d(geo_Id[0]);
    if (brick_occupied(get_id(index + ivec3(0, 0,sum.z / 3.0 * 2.0 - 1.0)))) {
      return;
    }
  }

  for(uint i = 0u; i < 3; ++i) {
    gl_Position   = gl_in[i].gl_Position;
    
    EmitVertex();
  }
}