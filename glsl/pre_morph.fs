#version 130
#extension GL_ARB_explicit_attrib_location : enable

noperspective in vec2 pass_TexCoord;

uniform uint layer;
uniform sampler2DArray kinect_depths;
uniform sampler2DArray eroded_depths;
uniform vec2 texSizeInv;
uniform uint mode;

layout(location = 0) out float out_Depth;

float sample(vec3 coords) {
  float depth = texture(kinect_depths, coords).r;
  return (depth - 0.5f) / 4.0f;
}

const float max_depth = 0.9f;

const int kernel_size = 1; // in pixel
const int kernel_end = kernel_size + 1;

float erode(const in vec3 coords) {
  float depth = sample(coords);
  if (depth <= 0.0f) {
    return 0.0f;
  }
  for(int y = -kernel_size; y < kernel_end; ++y){
    for(int x = -kernel_size; x < kernel_end; ++x){
      // if(abs(x))
      vec3 coords_s = coords + vec3(vec2(x, y) * texSizeInv, 0.0f);
      float depth_s = sample(coords_s);
      if (depth_s <= 0.0f || depth_s >= max_depth) {
        return 0.0f;
      }
    }
  }
  return depth;
}

float dilate(const in vec3 coords) {
  float average_depth = 0.0f;
  float num_samples = 0.0f;
  bool valid = false;
  for(int y = -kernel_size; y < kernel_end; ++y){
    for(int x = -kernel_size; x < kernel_end; ++x){
      vec3 coords_s = coords + vec3(vec2(x, y) * texSizeInv, 0.0f);
      float depth_s = texture(eroded_depths, coords_s).r;
      if (depth_s > 0.0f && depth_s < max_depth) {
        valid = true;
        average_depth += depth_s;
        num_samples += 1.0f;
      }
    }
  }
  if(!valid) return 0.0f;
  return average_depth / num_samples;
}
// average depth with new value
void main(void) {
  vec3 coords = vec3(pass_TexCoord, layer);
  // erode
  if(mode == 0u) {
    out_Depth = erode(coords);
  }
  // dilate
  else {
    out_Depth = dilate(coords);
  }
}