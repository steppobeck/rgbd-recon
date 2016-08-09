#version 420
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_shader_storage_buffer_object : require

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
///////////////////////////////////////////////////////////////////////////////
// input
///////////////////////////////////////////////////////////////////////////////
in vec3 geo_Position[];

uniform uint id;
///////////////////////////////////////////////////////////////////////////////
// output
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// main
///////////////////////////////////////////////////////////////////////////////
#include </inc_bbox_test.glsl>

layout (std430, binding = 3) buffer Bricks {
  float brick_size;
  uint pad[3];
  uvec3 resolution;
  uint pad2[1];
  uint[] bricks;
};

uint get_id(uvec3 brick_index) {
  return brick_index.z * resolution.y * resolution.x + brick_index.y * resolution.x + brick_index.x;
}

uvec3 index_3d(uint brick_id) {
  uvec3 index = uvec3(0u);
  index.z = brick_id / (resolution.x * resolution.y);
  brick_id %= (resolution.x * resolution.y);
  index.y = brick_id / resolution.x;
  brick_id %= resolution.x;
  index.x = brick_id;
  return index;
}

void mark_brick(in vec3 pos) {
  pos -= bbox_min;
  uvec3 index = uvec3(floor(pos / brick_size));
  bricks[get_id(index)] = 1u;
}

bool brick_occupied(uint index) {
  return bricks[index] > 0u;
}

vec3 to_world(vec3 position, uvec3 index) {
  return vec3(index) * brick_size + bbox_min + position * brick_size;
}

void main() {
  vec3 sum = geo_Position[0] + geo_Position[1] + geo_Position[2];

  if (sum.x < 1.0 || sum.x > 2.0) {
    uvec3 index = index_3d(id);
    if (brick_occupied(get_id(index + ivec3(sum.x / 3.0 * 2.0 - 1.0, 0, 0)))) {
      return;
    }
  }
  else if (sum.y < 1.0 || sum.y > 2.0) {
    uvec3 index = index_3d(id);
    if (brick_occupied(get_id(index + ivec3(0, sum.y / 3.0 * 2.0 - 1.0, 0)))) {
      return;
    }
  }
  else if (sum.z < 1.0 || sum.z > 2.0) {
    uvec3 index = index_3d(id);
    if (brick_occupied(get_id(index + ivec3(0, 0,sum.z / 3.0 * 2.0 - 1.0)))) {
      return;
    }
  }

  for(uint i = 0u; i < 3; ++i) {
    gl_Position   = gl_in[i].gl_Position;
    
    EmitVertex();
  }
}