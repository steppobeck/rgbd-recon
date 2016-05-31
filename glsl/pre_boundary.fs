#version 130
#extension GL_ARB_explicit_attrib_location : enable

noperspective in vec2 pass_TexCoord;

uniform uint layer;
uniform sampler2DArray kinect_depths;
uniform sampler2DArray kinect_colors;
uniform sampler2DArray kinect_colors_lab;
uniform sampler3D[5] cv_uv;

uniform vec2 texSizeInv;

layout(location = 0) out vec2 out_Depth;

const int kernel_size = 6; // in pixel
const int kernel_end = kernel_size + 1;

#include </inc_color.glsl>

vec3 get_color(vec3 coords) {
  vec2 coords_c = texture(cv_uv[layer], coords).xy;
  return texture(kinect_colors, vec3(coords_c, layer)).rgb;
}

float get_depth(vec2 coords) {
  vec3 color = texture(kinect_colors_lab, vec3(coords, layer)).rgb;
  float total_dist = 0.0f;
  vec3 total_color = vec3(0.0f);
  float total_depth = 0.0f;
  float num_samples = 0.0f;
  for(int y = -kernel_size; y < kernel_end; ++y){
    for(int x = -kernel_size; x < kernel_end; ++x){
      vec2 coords_s = coords + vec2(x, y) * texSizeInv;
      vec2 depth_s = texture(kinect_depths, vec3(coords_s, layer)).rg;
      if(depth_s.x > 0.0f ) {
        num_samples += 1.0f;
        vec3 color_s = texture(kinect_colors_lab, vec3(coords_s, layer)).rgb;
        total_color += color_s;
        total_depth += depth_s.x;
        // total_dist += distance(color, color_s);
      }
    }
  }
  return total_depth / num_samples;
}
// average depth with new value
void main(void) {
  vec2 depth = texture(kinect_depths, vec3(pass_TexCoord, layer)).rg;
  if(depth.y > 0.0f) {
    // depth.x = get_depth(pass_TexCoord);
    depth.x = -1.0f;
  }
  out_Depth = depth;
}