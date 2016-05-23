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
  float weight = 1.0f;
  vec2 weighted_depth = texture(bg_depths, vec3(pass_TexCoord, layer)).rg;

  out_Depth = vec2(weighted_depth.x * weighted_depth.y + curr_depth * weight, weighted_depth.y + weight);
}
