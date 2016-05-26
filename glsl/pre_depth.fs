#version 130
#extension GL_ARB_explicit_attrib_location : enable

noperspective in vec2 pass_TexCoord;

uniform sampler2D gauss;
uniform sampler2DArray kinect_depths;
uniform sampler2DArray bg_depths;
//uniform sampler2DArray kinect_colors;
uniform vec2 texSizeInv;
uniform bool filter_textures;

uniform sampler3D[5] cv_xyz;

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

layout(location = 0) out float out_Depth;
layout(location = 1) out float out_Quality;
layout(location = 2) out float out_Silhouette;

uniform vec3 bbox_min;
uniform vec3 bbox_max;
bool in_bbox(vec3 p){
  if(p.x >= bbox_min.x &&
     p.y >= bbox_min.y &&
     p.z >= bbox_min.z &&
     p.x <= bbox_max.x &&
     p.y <= bbox_max.y &&
     p.z <= bbox_max.z){
    return true;
  }
  return false;
}

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
#endif

  return vec2(filtered_depth, quality_strong);
}

void main(void) {
  vec3 coords = vec3(pass_TexCoord, layer);
  float depth = sample(coords);
  float depth_norm = normalize_depth(depth);
  vec3 pos_world = texture(cv_xyz[layer], vec3(pass_TexCoord, depth_norm)).xyz;
  bool is_in_box = in_bbox(pos_world);

  vec2 res = bilateral_filter(coords);
  if(!filter_textures) {
    float raw_depth = normalize_depth(sample(coords));
    out_Depth = raw_depth;
  }
  else {
    out_Depth = normalize_depth(res.x);
  }
  out_Quality = res.y;

  float bg_depth = texture(bg_depths, coords).r;
  const float min_bg_dist = 0.05f;
  if(bg_depth - out_Depth > min_bg_dist && out_Depth > 0.0f && is_in_box) {
    out_Silhouette = 1.0f;
  }
  else {
    out_Silhouette = 0.0f;
  }
}
