#ifndef BRICKS_GLSL
#define BRICKS_GLSL

#extension GL_ARB_shading_language_420pack : require
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_shader_storage_buffer_object : require

#include </inc_bbox_test.glsl>

layout (std430, binding = 3) buffer Bricks {
  float brick_size;
  uint pad[3];
  uvec3 resolution;
  uint pad2[1];
  uint[] bricks;
};

layout (std430, binding = 4) buffer Occupied {
  uint[] bricks_occupied;
};

vec3 to_world(vec3 position, uvec3 index) {
  return vec3(index) * brick_size + bbox_min + position * brick_size;
}

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
  uvec3 index = uvec3(floor((pos - bbox_min) / brick_size));
  vec3 brick_center = to_world(vec3(0.5), index);
  vec3 difference = pos - brick_center;
  vec3 d_abs = abs(difference);
  float min_v = max(d_abs.x, max(d_abs.y, d_abs.z));
  uvec3 min_c = uvec3((d_abs.x < min_v) ? 0u : 1u,
                      (d_abs.y < min_v) ? 0u : 1u,
                      (d_abs.z < min_v) ? 0u : 1u
                     );
  ivec3 offset = ivec3(sign(difference * vec3(min_c)));
  // closest, 5,5%, 1.5ms
  atomicAdd(bricks[get_id(uvec3(clamp(ivec3(index) + offset, ivec3(0), ivec3(resolution - 1u))))], (d_abs.x > brick_size * 0.1) ? 1u : 0u);

  // bricks[get_id(index)] = 1u;
  // costs 0.01-2 ms more
  // no neighbours, 3%, 1.3ms
  atomicAdd(bricks[get_id(index)], 1u);
}

bool brick_occupied(uint index) {
  return bricks[index] > 10u;
}

#endif