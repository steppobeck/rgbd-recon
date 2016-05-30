#version 130
#extension GL_ARB_explicit_attrib_location : enable

noperspective in vec2 pass_TexCoord;

// uniform sampler2D gauss;
uniform sampler2DArray kinect_depths;
uniform sampler2DArray kinect_normals;
uniform sampler2DArray kinect_colors;
uniform vec2 texSizeInv;
uniform vec3[5] camera_positions;

uniform uint layer;
uniform bool processed_depth;
uniform sampler3D[5] cv_xyz;
uniform sampler3D[5] cv_uv;

const int kernel_size = 6; // in pixel
const int kernel_end = kernel_size + 1;

layout(location = 0) out float out_Quality;
layout(location = 1) out vec3 out_Color;

const vec3 white_reference = vec3(95.047, 100.000, 108.883);
const float epsilon = 0.008856f;
const float kappa   = 903.3f;
float pivot_RGB(float n)
{
  return (n > 0.04045 ? pow((n + 0.055) / 1.055, 2.4) : n / 12.92) * 100.0;
}

vec3 rgb_to_xyz(vec3 rgbCol)
{

  float r = pivot_RGB(rgbCol[0] / 255.0f);
  float g = pivot_RGB(rgbCol[1] / 255.0f);
  float b = pivot_RGB(rgbCol[2] / 255.0f);

  vec3 xyz_col;

  xyz_col[0] = r * 0.4124 + g * 0.3576 + b * 0.1805;
  xyz_col[1] = r * 0.2126 + g * 0.7152 + b * 0.0722;
  xyz_col[2] = r * 0.0193 + g * 0.1192 + b * 0.9505;
  
  return xyz_col; 
}

float pivot_XYZ(float n)
{
        return n > epsilon ? pow(n, 1.0/3.0) : (kappa * n + 16) / 116;
}

vec3 xyz_to_lab(vec3 xyzCol)
{
  float x = pivot_XYZ(xyzCol[0] / white_reference[0]);
  float y = pivot_XYZ(xyzCol[1] / white_reference[1]);
  float z = pivot_XYZ(xyzCol[2] / white_reference[2]);

  vec3 lab_col;
  
  lab_col[0] = max(0.0, 116*y -16);
  lab_col[1] = 500 * (x - y);
  lab_col[2] = 200 * (y - z);

  return lab_col;
}


float calc_delta_E(vec3 c1, vec3 c2)
{
  return sqrt(
        pow(c1[0]-c2[0],2) + 
        pow(c1[1]-c2[1],2) +
        pow(c1[2]-c2[2],2) 
       ); 
}

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

vec3 get_color(vec3 coords) {
  float depth = sample(coords);
  if (depth <= 0.0f) depth = 1.0f;
  vec2 coords_c = texture(cv_uv[layer], vec3(coords.xy, depth)).xy;
  return texture(kinect_colors, vec3(coords_c, layer)).rgb;
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

  float lateral_quality  = 1.0f - border_samples/num_samples;
  // float lateral_quality  = 1.0f - border_samples/num_samples * w_range / 40.0f;
  lateral_quality  = 1.0f - weight_border_samples/weight_samples;
  float exponent = 3.0f;
  if(processed_depth) {
    exponent = 3.0f;
  }
  float quality_strong = pow(lateral_quality, exponent);
  quality_strong /= depth * 4.5f;
  // float fdiltered_depth = 0.0f;
  //float filtered_depth = depth_bf/w;
  float angle = normal_angle(vec3(coords.xy, depth), uint(coords.z));
  // quality_strong *= angle * angle;
  // return angle;
  return quality_strong;
}

void main(void) {
  vec3 coords = vec3(pass_TexCoord, layer);
  out_Quality = bilateral_filter(coords);
  out_Color = get_color(coords);
}