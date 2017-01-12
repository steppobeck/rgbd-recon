#version 130
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_shader_storage_buffer_object : enable
#extension GL_ARB_shading_language_include : require

noperspective in vec2 pass_TexCoord;

uniform uint layer;
uniform sampler2DArray kinect_depths;
uniform vec2 texSizeInv;

uniform sampler3D[5] cv_xyz;
uniform sampler3D[5] cv_uv;


layout(location = 0) out vec3 out_Normal;

#include </inc_bbox_test.glsl>

#include </bricks.glsl>

bool is_outside(float d){
  return (d <= 0.0) || (d >= 1.0);
}

vec3 calculate_normal(const in vec2 tex_pos) {
  float depth = texture(kinect_depths, vec3(tex_pos, layer)).r;
  if(is_outside(depth)) {
    return vec3(0.0);
  }
  // mark the containing brick as active
  vec3 world = texture(cv_xyz[layer], vec3(tex_pos, depth)).xyz;
  mark_brick(world);

  vec2 tex_t = tex_pos + vec2(0.0, texSizeInv.y);
  vec2 tex_b = tex_pos - vec2(0.0, texSizeInv.y);
  vec2 tex_l = tex_pos - vec2(texSizeInv.x, 0.0);
  vec2 tex_r = tex_pos + vec2(texSizeInv.x, 0.0);

  float depth_t = texture(kinect_depths, vec3(tex_t, layer)).r;
  float depth_b = texture(kinect_depths, vec3(tex_b, layer)).r;
  float depth_l = texture(kinect_depths, vec3(tex_l, layer)).r;
  float depth_r = texture(kinect_depths, vec3(tex_r, layer)).r;
  // correct depth is pixel is invalidated
  depth_t = is_outside(depth_t) ? depth : depth_t;
  depth_b = is_outside(depth_b) ? depth : depth_b;
  depth_l = is_outside(depth_l) ? depth : depth_l;
  depth_r = is_outside(depth_r) ? depth : depth_r;
  // lookup world positions
  vec3 world_t = texture(cv_xyz[layer], vec3(tex_t, depth_t)).xyz;
  vec3 world_b = texture(cv_xyz[layer], vec3(tex_b, depth_b)).xyz;
  vec3 world_l = texture(cv_xyz[layer], vec3(tex_l, depth_l)).xyz;
  vec3 world_r = texture(cv_xyz[layer], vec3(tex_r, depth_r)).xyz;

  return normalize(cross(world_b - world_t, world_l - world_r));
}

void main(void) {
  out_Normal = calculate_normal(pass_TexCoord);
}
