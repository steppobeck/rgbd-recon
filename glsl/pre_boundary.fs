#version 130
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_shading_language_include : require

noperspective in vec2 pass_TexCoord;

uniform uint layer;
uniform sampler2DArray kinect_depths;
uniform sampler2DArray kinect_colors;
uniform sampler2DArray kinect_colors_lab;
uniform sampler3D[5] cv_uv;
uniform bool refine;

uniform vec2 texSizeInv;

layout(location = 0) out vec2 out_Depth;
layout(location = 1) out float out_Silhouette;

const float max_color_dist = 0.9f;

const int kernel_size = 2; // in pixel
const int kernel_end = kernel_size + 1;
const int total_samples = (kernel_size * 2) * (kernel_size * 2);

#include </inc_color.glsl>

const float min_range = 0.65f;
bool foreground(float depth) {
  return depth > min_range;
}

vec3 get_color(vec3 coords) {
  vec2 coords_c = texture(cv_uv[layer], coords).xy;
  return texture(kinect_colors, vec3(coords_c, layer)).rgb;
}

float get_color_diff(vec2 coords) {
  vec3 color = texture(kinect_colors_lab, vec3(coords, layer)).rgb;
  float total_dist = 0.0f;
  float num_samples = 0.0f;
  for(int y = -kernel_size; y < kernel_end; ++y){
    for(int x = -kernel_size; x < kernel_end; ++x){
      vec2 coords_s = coords + vec2(x, y) * texSizeInv;
      vec2 depth_s = texture(kinect_depths, vec3(coords_s, layer)).rg;
      if(depth_s.x > 0.0f && foreground(depth_s.y)) {
        num_samples += 1.0f;
        vec3 color_s = texture(kinect_colors_lab, vec3(coords_s, layer)).rgb;
        total_dist += distance(color, color_s);
      }
    }
  }
  // atleas one neighbour must be valid
  if(num_samples < total_samples * 0.5f) return 1.0f;
  return total_dist / num_samples;
}

float recompute_depth(vec2 coords) {
  vec3 total_color = vec3(0.0f);
  float total_depth = 0.0f;
  int num_samples = 0;
  for(int y = -kernel_size; y < kernel_end; ++y){
    for(int x = -kernel_size; x < kernel_end; ++x){
      vec2 coords_s = coords + vec2(x, y) * texSizeInv;
      vec2 depth_s = texture(kinect_depths, vec3(coords_s, layer)).rg;
      if(depth_s.x > 0.0f && foreground(depth_s.y)) {
        num_samples += 1;
        vec3 color_s = texture(kinect_colors_lab, vec3(coords_s, layer)).rgb;
        total_color += color_s;
        total_depth += depth_s.x;
      }
    }
  }
  // atleas one neighbour must be valid
  if(float(num_samples) < total_samples * 0.5f) return 0.0f;
  float avg_depth = total_depth / float(num_samples);
  vec2 pos_color = texture(cv_uv[layer], vec3(coords, avg_depth)).xy;
  vec3 color_rgb = texture(kinect_colors, vec3(pos_color.xy, layer)).rgb;
  vec3 color_lab = rgb_to_lab(color_rgb);
  if(distance(color_lab, total_color / float(num_samples)) > max_color_dist) {
    return 0.0f;
  }
  return avg_depth;
}

// average depth with new value
void main(void) {
  vec2 depth = texture(kinect_depths, vec3(pass_TexCoord, layer)).rg;
  out_Silhouette = 1.0f;
  // pixel is outside of box
  if(depth.x <= 0.0f) {
    float new_depth = recompute_depth(pass_TexCoord);
    if(refine && new_depth > 0.0f) {
      depth.x = new_depth;
      depth.y = 0.5f;
    }
    else {
      depth.y = 0.0f;
      out_Silhouette = 0.0f;
    }
  }
  // pixel was discarded in depth filtering
  else if(!foreground(depth.y)) {
    if(depth.y > 0.65) {
      out_Silhouette = 1.0f;
    }
    else {
      out_Silhouette = 0.0f;
    }
    float color_dist = get_color_diff(pass_TexCoord);
    if(color_dist > max_color_dist || !refine) {
      depth.x = -1.0f;
      depth.y = 0.1f;
      out_Silhouette = 0.0f;
    }
    else {
      depth.y = 1.0f;
    }
  }
  else {
    depth.y = 0.0f;
  }
  out_Depth = vec2(depth);
}