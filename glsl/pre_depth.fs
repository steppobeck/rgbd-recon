#version 130
#extension GL_ARB_explicit_attrib_location : enable

noperspective in vec2 pass_TexCoord;

uniform sampler2D gauss;
uniform sampler2DArray kinect_depths;
uniform sampler2DArray kinect_colors;
uniform sampler2DArray bg_depths;
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
////////////////////////////////////////////////////////////////////////
const vec3 white_reference = vec3(95.047, 100.000, 108.883);
const float epsilon = 0.008856f;
const float kappa   = 903.3f;

float pivot_RGB(float n) {
  return (n > 0.04045 ? pow((n + 0.055) / 1.055, 2.4) : n / 12.92) * 100.0;
}

vec3 rgb_to_xyz(vec3 rgbCol) {

  float r = pivot_RGB(rgbCol[0] / 255.0f);
  float g = pivot_RGB(rgbCol[1] / 255.0f);
  float b = pivot_RGB(rgbCol[2] / 255.0f);

  vec3 xyz_col;

  xyz_col[0] = r * 0.4124 + g * 0.3576 + b * 0.1805;
  xyz_col[1] = r * 0.2126 + g * 0.7152 + b * 0.0722;
  xyz_col[2] = r * 0.0193 + g * 0.1192 + b * 0.9505;
  
  return xyz_col; 
}

float pivot_XYZ(float n) {
        return n > epsilon ? pow(n, 1.0/3.0) : (kappa * n + 16) / 116;
}

vec3 xyz_to_lab(vec3 xyzCol) {
  float x = pivot_XYZ(xyzCol[0] / white_reference[0]);
  float y = pivot_XYZ(xyzCol[1] / white_reference[1]);
  float z = pivot_XYZ(xyzCol[2] / white_reference[2]);

  vec3 lab_col;
  
  lab_col[0] = max(0.0, 116*y -16);
  lab_col[1] = 500 * (x - y);
  lab_col[2] = 200 * (y - z);

  return lab_col;
}

vec3 rgb_to_lab(vec3 rgb) {
  return xyz_to_lab(rgb_to_xyz(rgb));
}

float calc_delta_E(vec3 c1, vec3 c2) {
  return sqrt(
        pow(c1[0]-c2[0],2) + 
        pow(c1[1]-c2[1],2) +
        pow(c1[2]-c2[2],2) 
       ); 
}

///////////////////////////////////////////////////////////////////////



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

float sample(vec3 coords) {
  float depth = 0.0f;
  if(compress){
    depth = uncompress(texture(kinect_depths, coords).r);
  }
  else{
    depth = texture(kinect_depths, coords).r;
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

  float depth = sample(coords);
  if(is_outside(depth)){
    return vec2(-1.0,0.0);
  }
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
      vec3 coords_s = coords + vec3(vec2(x, y) * texSizeInv, 0.0f);
      
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

  float lateral_quality  = 1.0f - border_samples/num_samples;
  // float lateral_quality  = 1.0f - border_samples/num_samples * w_range / 40.0f;
  // lateral_quality  = 1.0f - weight_border_samples/weight_samples;
  float quality_strong = pow(lateral_quality,30.0);
  quality_strong /= depth;
  float filtered_depth = 0.0f;
  if(w > 0.0)
    filtered_depth = depth_bf/w;
  else {
    // if (!processed_depth) 
    {
      filtered_depth = -1.0f;
    }
  }

#if 1
  if(!processed_depth) 
  {
    if(w_range < (num_samples * 0.65)){
      filtered_depth = -1.0f;
    }
  }
  else {

  }
#endif

  return vec2(filtered_depth, quality_strong);
}

void main(void) {
  vec3 coords = vec3(pass_TexCoord, layer);
  float depth = sample(coords);
  float depth_norm = normalize_depth(depth);
  vec3 pos_world = texture(cv_xyz[layer], vec3(pass_TexCoord, depth_norm)).xyz;
  bool is_in_box = in_bbox(pos_world);
  if (!is_in_box) {
    out_Depth.x = 0.0f;
    out_Silhouette = 0.0f;
    out_Color = rgb_to_lab(get_color(vec3(coords.xy, (depth_norm <= 0.0f) ? 0.0f : 1.0f)));
    return;
  }
  vec2 res = bilateral_filter(coords);
  if(!filter_textures) {
    res.x = depth;
  }
  res.x = normalize_depth(res.x);
  out_Depth.x = res.x;

  float bg_depth = texture(bg_depths, coords).r;
  const float min_bg_dist = 0.01f;
  if(bg_depth - res.x > min_bg_dist && res.x > 0.0f && is_in_box) {
    out_Silhouette = 1.0f;
  }
  else {
    out_Silhouette = 0.0f;
  }

  if(res.x < 0.0f) {
    out_Color = vec3(0.0f, 1.0f, 0.0f);
  }
  else {
    out_Color = rgb_to_lab(get_color(vec3(coords.xy, (res.x <= 0.0f) ? 0.0f : 1.0f)));  
  }
}
