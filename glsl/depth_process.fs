#version 130
#extension GL_ARB_explicit_attrib_location : enable

noperspective in vec2 pass_TexCoord;

uniform sampler2D gauss;
uniform sampler2DArray kinect_depths;
//uniform sampler2DArray kinect_colors;
uniform vec2 texSizeInv;
uniform bool filter_textures;

uniform uint layer;
uniform bool compress;
uniform float scale;
uniform float near;
uniform float scaled_near;

uniform float cv_min_ds;
uniform float cv_max_ds;
uniform int mode;

const int kernel_size = 6; // in pixel
const int kernel_end = kernel_size + 1;

layout(location = 0) out float out_Depth;
layout(location = 1) out float out_Quality;

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
vec2 bilateral_filter(vec3 coords){

  float depth = sample(coords);
  if(is_outside(depth)){
    return vec2(-1.0,0.0);
  }
  // the valid range scales with depth
  float max_depth = 4.5f; // Kinect V2
  float d_dmax = depth / max_depth;
  dist_range_max = 0.35f * d_dmax; // threshold around 
  dist_range_max_inv = 1.0f / dist_range_max;

  float depth_bf = 0.0f;

  float w = 0.0f;
  float w_range = 0.0f;
  float border_samples = 0.0f;
  float num_samples = 0.0f;
  for(int y = -kernel_size; y < kernel_end; ++y){
    for(int x = -kernel_size; x < kernel_end; ++x){
      num_samples += 1.0f;
      vec3 coords_s = coords + vec3(vec2(x, y) * texSizeInv, 0.0f);
      
      float depth_s = sample(coords_s);
      float depth_range = abs(depth_s - depth);
      if(is_outside(depth_s) || (depth_range > dist_range_max)){
        border_samples += 1.0f;
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
  float filtered_depth = 0.0f;
  if(w > 0.0)
    filtered_depth = depth_bf/w;
  else
    filtered_depth = -1.0f;

#if 1
  if(w_range < (num_samples * 0.65)){
    filtered_depth = -1.0f;
  }
#endif

  return vec2(filtered_depth, pow(lateral_quality,30.0));
}

void main(void) {
  vec3 coords = vec3(pass_TexCoord, layer);
  vec2 res = bilateral_filter(coords);
  if(!filter_textures) {
    res.x = sample(coords);
  }

  out_Depth = normalize_depth(res.x);
  out_Quality = res.y;
}
