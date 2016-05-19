#version 130
#extension GL_ARB_explicit_attrib_location : enable

noperspective in vec2 pass_TexCoord;

uniform uint layer;
uniform sampler2DArray kinect_depths;
uniform vec2 texSizeInv;

uniform sampler3D[5] cv_xyz;
uniform sampler3D[5] cv_uv;

layout(location = 0) out vec3 out_Normal;

vec3 calculate_normal(const in vec2 tex_pos) {
  vec2 tex_t = tex_pos + vec2(0.0f, texSizeInv.y * 1.0f);
  vec2 tex_b = tex_pos - vec2(0.0f, texSizeInv.y * 1.0f);
  vec2 tex_l = tex_pos - vec2(texSizeInv.x * 1.0f, 0.0f);
  vec2 tex_r = tex_pos + vec2(texSizeInv.x * 1.0f, 0.0f);
  float depth_t = texture(kinect_depths, vec3(tex_t, layer)).r;
  float depth_b = texture(kinect_depths, vec3(tex_b, layer)).r;
  float depth_l = texture(kinect_depths, vec3(tex_l, layer)).r;
  float depth_r = texture(kinect_depths, vec3(tex_r, layer)).r;
  vec3 world_t = texture(cv_xyz[layer], vec3(tex_t, depth_t)).xyz;
  vec3 world_b = texture(cv_xyz[layer], vec3(tex_b, depth_b)).xyz;
  vec3 world_l = texture(cv_xyz[layer], vec3(tex_l, depth_l)).xyz;
  vec3 world_r = texture(cv_xyz[layer], vec3(tex_r, depth_r)).xyz;

  return normalize(cross(world_b - world_t, world_r - world_l)) * 0.5f + 0.5f;
}

void main(void) {
  out_Normal = calculate_normal(pass_TexCoord);
}
