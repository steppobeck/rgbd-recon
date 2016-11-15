#version 130
#extension GL_ARB_explicit_attrib_location : enable

noperspective in vec2 pass_TexCoord;

uniform uint layer;
uniform sampler2DArray kinect_depths;
uniform sampler2DArray bg_depths;

layout(location = 0) out vec2 out_Depth;

// average depth with new value
void main(void) {
  float curr_depth = texture(kinect_depths, vec3(pass_TexCoord, layer)).r;
  curr_depth = (curr_depth - 0.5f) / 4.0f;
  if (curr_depth <= 0.0f) {
    curr_depth = 1.0f;
  }
  float weight = 1.0f;
  vec2 weighted_depth = texture(bg_depths, vec3(pass_TexCoord, layer)).rg;
  float total_weight = weighted_depth.y + weight;
  out_Depth = vec2((weighted_depth.x * weighted_depth.y + curr_depth * weight) / total_weight, total_weight);
  // out_Depth = vec2(curr_depth * weight, + weight);
}
