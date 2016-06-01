#version 130
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_shading_language_include : require

noperspective in vec2 pass_TexCoord;

uniform sampler2D gauss;
uniform sampler2DArray kinect_depths;
uniform sampler2DArray kinect_colors;
uniform vec2 texSizeInv;
uniform bool filter_textures;

uniform sampler3D[5] cv_xyz;
uniform sampler3D[5] cv_uv;

uniform uint layer;
uniform bool compress;
uniform float scale;
uniform float near;
uniform float scaled_near;

uniform float cv_min_ds;
uniform float cv_max_ds;
uniform int mode;
uniform bool processed_depth;

const int kernel_size = 6; // in pixel
const int kernel_end = kernel_size + 1;

layout(location = 0) out vec2 out_Depth;
layout(location = 1) out float out_Silhouette;
layout(location = 2) out vec3 out_Color;

#include </inc_bbox_test.glsl>

#include </inc_color.glsl>

float dist_space_max_inv = 1.0/float(kernel_size);
float computeGaussSpace(float dist_space){
  float gauss_coord = dist_space * dist_space_max_inv;
  return 1.0 - gauss_coord;//texture2D(gauss,vec2(gauss_coord,0.5)).r;
}

float dist_range_max = 0.05; // in meter
float dist_range_max_inv = 1.0/dist_range_max;
float computeGaussRange(float dist_range){
  float gauss_coord = min(dist_range, dist_range_max) * dist_range_max_inv;
  return 1.0 - gauss_coord;//texture2D(gauss,vec2(gauss_coord,0.5)).r;
}


float uncompress(float d_c){
    if(d_c < scaled_near){
      return 0.0;
    }
    else{
      // linear mapping
      //return (d_c + 0.15 *scaled_near) * scale + near;
      // sqrt-mapping
      return (d_c * d_c + 0.15 *scaled_near) * scale + near;
    }
}

float sample(vec2 coords) {
  float depth = 0.0f;
  if(compress){
    depth = uncompress(texture(kinect_depths, vec3(coords, layer)).r);
  }
  else{
    depth = texture(kinect_depths, vec3(coords, layer)).r;
  }
  return depth;
}

bool is_outside(float d){
  return (d < cv_min_ds) || (d > cv_max_ds);
}

float normalize_depth(float depth) {
  return (depth - cv_min_ds)/(cv_max_ds - cv_min_ds);
}
vec3 get_color(vec3 coords) {
  vec2 coords_c = texture(cv_uv[layer], coords).xy;
  return texture(kinect_colors, vec3(coords_c, layer)).rgb;
}

vec2 bilateral_filter(vec3 coords){

  float depth = coords.z;
  // the valid range scales with depth
  const float max_depth = 4.5f; // Kinect V2
  float d_dmax = depth / max_depth;
  dist_range_max = 0.35f * d_dmax; // threshold around 
  dist_range_max_inv = 1.0f / dist_range_max;

  float depth_bf = 0.0f;

  float w = 0.0f;
  float w_range = 0.0f;
  float border_samples = 0.0f;
  float num_samples = 0.0f;
  float weight_samples = 0.0f;
  float weight_border_samples = 0.0f;
  for(int y = -kernel_size; y < kernel_end; ++y){
    for(int x = -kernel_size; x < kernel_end; ++x){
      num_samples += 1.0f;
      weight_samples +=  1.0f - length(vec2(x,y)) / length(vec2(0,6));
      vec2 coords_s = coords.xy + vec2(x, y) * texSizeInv;
      
      float depth_s = sample(coords_s);
      float depth_range = abs(depth_s - depth);
      if(is_outside(depth_s) || (depth_range > dist_range_max)){
        border_samples += 1.0f;
        weight_border_samples += 1.0f - length(vec2(x,y)) / length(vec2(0,6));
        continue;
      }
      float gauss_space = computeGaussSpace(length(vec2(x,y)));
      float gauss_range = computeGaussRange(depth_range);
      float w_s = gauss_space * gauss_range;
      depth_bf += w_s * depth_s;
      w += w_s;
      w_range += gauss_range;
    }
  }

  float discarded = 0.0f;
  float filtered_depth = 0.0f;
  if(w > 0.0)
    filtered_depth = depth_bf/w;
  else {
    discarded = 1.0f;
  }

  if(w_range < (num_samples * 0.65)) {
    discarded = 1.0f;
  }

  return vec2(normalize_depth(filtered_depth), discarded);
}

void main(void) {
  out_Depth = vec2(0.0f);
  out_Silhouette = 0.0f;

  float depth = sample(pass_TexCoord);
  float depth_norm = normalize_depth(depth);
  vec3 pos_world = texture(cv_xyz[layer], vec3(pass_TexCoord, depth_norm)).xyz;
  bool is_in_box = in_bbox(pos_world);
  
  if (!is_in_box) {
    out_Color = rgb_to_lab(get_color(vec3(pass_TexCoord, (depth_norm <= 0.0f) ? 0.0f : 1.0f)));
    return;
  }

  if(!filter_textures) {
    out_Depth = vec2(depth_norm, 0.0f);
    return;
  }

  out_Depth = bilateral_filter(vec3(pass_TexCoord, depth));
// if(out_Depth.y > 0.1f) {
//   out_Depth.x = -1.0f;
// }

  if(out_Depth.y < 1.0f) {
    out_Silhouette = 1.0f;
  }

  out_Color = rgb_to_lab(get_color(vec3(pass_TexCoord, (out_Depth.x <= 0.0f) ? 0.0f : 1.0f)));  
}