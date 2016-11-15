#version 130
#extension GL_ARB_explicit_attrib_location : enable

noperspective in vec2 pass_TexCoord;

// uniform sampler2D gauss;
uniform sampler2DArray kinect_depths;
uniform sampler2DArray kinect_normals;
uniform sampler2DArray kinect_colors_lab;
uniform vec2 texSizeInv;
uniform vec3[5] camera_positions;

uniform uint layer;
uniform bool processed_depth;
uniform sampler3D[5] cv_xyz;

const int kernel_size = 6; // in pixel
const int kernel_end = kernel_size + 1;

layout(location = 0) out float out_Quality;

float dist_space_max_inv = 1.0/float(kernel_size);
float computeGaussSpace(float dist_space){
  float gauss_coord = dist_space * dist_space_max_inv;
  return 1.0 - gauss_coord;//texture2D(gauss,vec2(gauss_coord,0.5)).r;
}

float dist_range_max = 0.05f; // in meter
float dist_range_max_inv = 1.0/dist_range_max;
float computeGaussRange(float dist_range){
  float gauss_coord = min(dist_range, dist_range_max) * dist_range_max_inv;
  return 1.0 - gauss_coord;//texture2D(gauss,vec2(gauss_coord,0.5)).r;
}

float sample(vec3 coords) {
  return texture(kinect_depths, coords).r;
}

bool is_outside(float d){
  return (d <= 0.0f) || (d >= 1.0f);
}

float normal_angle(vec3 position, uint layer) {
  vec3 world_normal = texture(kinect_normals, vec3(position.xy, float(layer))).xyz;
  vec3 world_pos = texture(cv_xyz[layer], position).xyz;
  float angle = dot(normalize(camera_positions[layer] - world_pos), world_normal);
  return angle;
}

float get_color_diff(vec2 coords) {
  vec3 color = texture(kinect_colors_lab, vec3(coords, layer)).rgb;
  float total_dist = 0.0f;
  float num_samples = 0.0f;
  for(int y = -kernel_size; y < kernel_end; ++y){
    for(int x = -kernel_size; x < kernel_end; ++x){
      num_samples += 1.0f;
      vec2 coords_s = coords + vec2(x, y) * texSizeInv;
      vec3 color_s = texture(kinect_colors_lab, vec3(coords_s, layer)).rgb;
      total_dist += distance(color, color_s);
    }
  }
  return total_dist / num_samples;
}

float bilateral_filter(vec3 coords){

  float depth = sample(coords);
  if(is_outside(depth)){
    return 0.0f;
  }
  // the valid range scales with depth
  const float max_depth = 1.0f; // Kinect V2
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

  float lateral_quality  = 1.0f - border_samples / num_samples;
  float exponent = 6.0f;
  float quality_strong = pow(lateral_quality, exponent);
  quality_strong *= pow(w_range / num_samples, exponent);
  quality_strong /= depth * 6.5f;
  //float filtered_depth = depth_bf/w;
  float angle = normal_angle(vec3(coords.xy, depth), uint(coords.z));
  quality_strong *= pow(angle, 2.0f);
  float color_diff = get_color_diff(coords.xy);
  // quality_strong *= color_diff;
  // return angle;
  return quality_strong;
}

void main(void) {
  vec3 coords = vec3(pass_TexCoord, layer);
  out_Quality = bilateral_filter(coords);
}