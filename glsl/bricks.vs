#version 130
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_shader_storage_buffer_object : require

// input
in vec3 in_Position; 

uniform mat4 gl_ModelViewMatrix;
uniform mat4 gl_ProjectionMatrix;
uniform uint id;

out vec3 geo_Position;

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

vec3 to_world(vec3 position, uvec3 index) {
  return vec3(index) * brick_size + bbox_min + position * brick_size;
}

void main() {
  geo_Position = in_Position;
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(to_world(geo_Position, index_3d(id)), 1.0);
}