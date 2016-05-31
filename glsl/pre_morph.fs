#version 130
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_shading_language_include : require

noperspective in vec2 pass_TexCoord;

uniform uint layer;
uniform sampler2DArray kinect_depths;
uniform vec2 texSizeInv;
uniform uint mode;

uniform sampler3D[5] cv_xyz;

layout(location = 0) out float out_Depth;

#include </inc_bbox_test.glsl>
// #define NORMALIZED

float sample(vec3 coords) {
  float depth = texture(kinect_depths, coords).r;
  #ifdef NORMALIZED
    return (depth - 0.5f) / 4.0f;
  #else
    return depth;
  #endif
}

#ifdef NORMALIZED
  const float min_depth = 0.0f;
  const float max_depth = 1.0f;
#else
  const float min_depth = 0.5f;
  const float max_depth = 4.5f;
#endif

bool is_valid(float depth) {
  // return true;
  return depth > min_depth && depth < max_depth;
}

float normalize_depth(float depth) {
  return (depth - min_depth)/(max_depth - min_depth);
}
bool in_bbox(vec2 texcoord, float depth) {
  float depth_norm = normalize_depth(depth);
  vec3 pos_world = texture(cv_xyz[layer], vec3(texcoord, depth_norm)).xyz;
  bool is_in_box = in_bbox(pos_world);
  return true;
  return is_in_box;
}

const int kernel_size = 1; // in pixel
const int kernel_end = kernel_size + 1;
const float max_dist = 0.2f;
float erode(const in vec3 coords, int kernel_size) {
  float depth = sample(coords);
  if (depth <= min_depth) {
    return 0.0f;
  }
  for(int y = -kernel_size; y < kernel_size + 1; ++y){
    for(int x = -kernel_size; x < kernel_size + 1; ++x){
      // if(abs(x))
      vec3 coords_s = coords + vec3(vec2(x, y) * texSizeInv, 0.0f);
      float depth_s = sample(coords_s);
      if (!is_valid(depth_s) || distance(depth, depth_s) > max_dist || !in_bbox(coords_s.xy, depth_s)) {
        return 0.0f;
      }
    }
  }
  return depth;
}

float dilate(const in vec3 coords, int kernel_size) {
  float depth = sample(coords);
  float average_depth = 0.0f;
  float num_samples = 0.0f;
  bool valid = false;
  for(int y = -kernel_size; y < kernel_size + 1; ++y){
    for(int x = -kernel_size; x < kernel_size + 1; ++x){
      vec3 coords_s = coords + vec3(vec2(x, y) * texSizeInv, 0.0f);
      float depth_s = sample(coords_s);
      if (is_valid(depth_s) && distance(depth, depth_s) < max_dist && in_bbox(coords_s.xy, depth_s) ) {
        valid = true;
        average_depth += depth_s;
        num_samples += 1.0f;
      }
    }
  }

  if(!valid) return 0.0f;
  // if (in_bbox(coords.xy, depth)) return depth;
  return average_depth / num_samples;
}
// average depth with new value
void main(void) {
  vec3 coords = vec3(pass_TexCoord, layer);
  float depth = sample(coords);
  // bool is_in_box = in_bbox(coords.xy, depth);
  // if (!is_in_box) {
  //   out_Depth = 0.0f;
  //   return;
  // }
  // erode
  if(mode == 0u) {
    out_Depth = sample(coords);
    // out_Depth = erode(coords, 1);
    out_Depth = dilate(coords, 1);
    // out_Depth = 0.75f;
  }
  // dilate
  else if (mode == 1u) {
    out_Depth = sample(coords);
    // out_Depth = erode(coords, 2);
    // out_Depth = dilate(coords, 1);
    // out_Depth = 0.25f;
  }
  // dilate
  else {
    out_Depth = 0.25f;
  }
}