#version 330
#extension GL_ARB_shading_language_include : require

in vec3 pass_Position;
// input
uniform sampler2DArray kinect_colors;
uniform sampler2DArray kinect_depths;
uniform sampler2DArray kinect_qualities;
uniform sampler2DArray kinect_normals;
// calibration
uniform sampler3D[5] cv_xyz_inv;
uniform sampler3D[5] cv_uv;
uniform uint num_kinects;
uniform float limit;

uniform mat4 gl_ModelViewMatrix;
uniform mat4 gl_ProjectionMatrix;
uniform mat4 gl_NormalMatrix;

uniform mat4 NormalMatrix;
uniform mat4 vol_to_world;

uniform sampler3D volume_tsdf;
uniform vec3 CameraPos;
uniform vec3 Dimensions;

float sampleDistance = limit * 0.5f;
const float IsoValue = 0.0f;

out vec4 out_Color;
out float gl_FragDepth;

#include </shading.glsl>

#define GRAD_NORMALS

vec3 get_gradient(const vec3 pos);
bool isInside(const vec3 pos);
float sample(const vec3 pos);
vec3 blendColors(const in vec3 sample_pos);
vec3 blendNormals(const in vec3 sample_pos);
vec3 blendCameras(const in vec3 sample_pos);

void main() {
  // multiply with dimensions to scale direction by dimension relation
  vec3 sampleStep = normalize(pass_Position - CameraPos) * sampleDistance;

  vec3 sample_pos = pass_Position;
  bool inside = isInside(sample_pos);  
  // cache value of previous sample
  float prev_density = sample(sample_pos); 

  while (inside) {
     // get sample
    float density = sample(sample_pos);

    // check if cell is inside contour
    if (density < IsoValue && prev_density >= IsoValue) {
      // approximate ray-cell intersection
      sample_pos = (sample_pos - sampleStep) - sampleStep * (prev_density / (density - prev_density));

      float final_density = sample(sample_pos);
      #ifdef GRAD_NORMALS
      vec3 view_normal = normalize((NormalMatrix * vec4(get_gradient(sample_pos), 0.0f)).xyz);
      #else
      vec3 view_normal = normalize((NormalMatrix * vec4(blendNormals(sample_pos), 0.0f)).xyz);
      #endif
      vec3 view_pos = (gl_ModelViewMatrix * vol_to_world * vec4(sample_pos, 1.0f)).xyz;

      if (g_shade_mode == 3u) {
        out_Color = vec4(blendCameras(sample_pos), 1.0f);
      }
      else {
        vec3 diffuseColor = blendColors(sample_pos);
        out_Color = vec4(shade(view_pos, view_normal, diffuseColor), 1.0f);
      }
      // apply projection matrix on z component of view-space position
      gl_FragDepth = (gl_ProjectionMatrix[2].z *view_pos.z + gl_ProjectionMatrix[3].z) / -view_pos.z * 0.5f + 0.5f;
      return;
    }

    prev_density = density;
    sample_pos += sampleStep;
    inside = isInside(sample_pos); 
  }
  // no surface found 
  discard;
}

bool isInside(const vec3 pos) {
  return pos.x >= 0.0f && pos.x <= 1.0f
      && pos.y >= 0.0f && pos.y <= 1.0f
      && pos.z >= 0.0f && pos.z <= 1.0f;
}

float sample(const vec3 pos) {
  return texture(volume_tsdf, pos).r;
}

vec3 get_gradient(const vec3 pos) {
  vec3 x_offset = vec3(sampleDistance, 0, 0);
  vec3 y_offset = vec3(0, sampleDistance, 0);
  vec3 z_offset = vec3(0, 0, sampleDistance);
  // invert direction because it points to bigger density
  return normalize(vec3(
   sample(pos + x_offset) - sample(pos - x_offset),
   sample(pos + y_offset) - sample(pos - y_offset),
   sample(pos + z_offset) - sample(pos - z_offset)));
}

float[5] getWeights(const in vec3 sample_pos) {
  float weights[5] =float[5](0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
  for (uint i = 0u; i < num_kinects; ++i) {
    vec3 pos_calib = texture(cv_xyz_inv[i], sample_pos).xyz;
    float depth = texture(kinect_depths, vec3(pos_calib.xy, float(i))).r;
    float quality = 0.0f;
    // blend if in valid depth range
    if(abs(depth - pos_calib.z) < limit) {
      float lateral_quality = texture(kinect_qualities, vec3(pos_calib.xy, float(i))).r;
      quality = lateral_quality/(pos_calib.z * 4.0f+ 0.5f);
    }

    weights[i] = quality;
  }
  return weights;
}

vec3 blendColors(const in vec3 sample_pos) {
  vec3 total_color = vec3(0.0f);
  float total_weight = 0.0f;
  float[5] weights = getWeights(sample_pos);
  for(uint i = 0u; i < num_kinects; ++i) {
    vec3 pos_calib = texture(cv_xyz_inv[i], sample_pos).xyz;
    vec2 pos_color = texture(cv_uv[i], pos_calib).xy;
    vec3 color = texture(kinect_colors, vec3(pos_color.xy, float(i))).rgb;

    total_color += color * weights[i];
    total_weight += weights[i];
  }

  total_color /= total_weight;
  return total_color;
}

vec3 blendNormals(const in vec3 sample_pos) {
  vec3 total_color = vec3(0.0f);
  float total_weight = 0.0f;
  float[5] weights = getWeights(sample_pos);
  for(uint i = 0u; i < num_kinects; ++i) {
    vec3 pos_calib = texture(cv_xyz_inv[i], sample_pos).xyz;
    vec3 color = texture(kinect_normals, vec3(pos_calib.xy, float(i))).rgb;

    total_color += color * weights[i];
    total_weight += weights[i];
  }

  total_color /= total_weight;
  return total_color;
}

vec3 blendCameras(const in vec3 sample_pos) {
  vec3 total_color = vec3(0.0f);
  float total_weight = 0.0f;
  float[5] weights = getWeights(sample_pos);
  for(uint i = 0u; i < num_kinects; ++i) {
    vec3 pos_calib = texture(cv_xyz_inv[i], sample_pos).xyz;
    vec3 color = camera_colors[i];

    total_color += color * weights[i];
    total_weight += weights[i];
  }

  total_color /= total_weight;
  return total_color;
}