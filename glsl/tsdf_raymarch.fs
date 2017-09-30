#version 330
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_shader_image_load_store : require

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

uniform bool skipSpace;
uniform sampler2D depth_peels;
uniform vec2 viewport_offset;
uniform mat4 img_to_eye_curr;

layout(r32f) uniform image2D tex_num_samples;

float sampleDistance = limit * 0.5f;
const float IsoValue = 0.0;
const int refinement_num = 4;

out vec4 out_Color;

out float gl_FragDepth;

#include </shading.glsl>

#include </inc_bbox_test.glsl>

#define GRAD_NORMALS

vec3 get_gradient(const vec3 pos);
float sample(const vec3 pos);
vec3[5] getColors(const in vec3 sample_pos);
vec3[5] getNormals(const in vec3 sample_pos);
vec4 blendColors(const in vec3 sample_pos);
vec4 blendColors2(const in vec3 sample_pos);
vec3 blendNormals(const in vec3 sample_pos);
vec3 blendCameras(const in vec3 sample_pos);
// cube-ray intersection from http://prideout.net/blog/?p=64
bool intersectBox(const vec3 origin, const vec3 dir, out float t0, out float t1);
void submitFragment(const in vec3 sample_pos);
vec4 getStartPos(ivec2 coords);
void writeNumSamples(uint num_samples);

void main() {
  // multiply with dimensions to scale direction by dimension relation
  vec3 sampleStep = normalize(pass_Position - CameraPos) * sampleDistance;

  uint max_num_samples = 0u;
  vec3 sample_pos = vec3(0.0);

  if (skipSpace) { 
    vec4 posEnd = getStartPos(ivec2(gl_FragCoord.xy - viewport_offset));

    sample_pos = posEnd.xyz;
    max_num_samples = uint(ceil(posEnd.w / sampleDistance));
  }
  else {
    // get ray beginning in volume cube
    float t0, t1 = 0.0;
    bool is_t0 = intersectBox(CameraPos, sampleStep, t0, t1);
    float t_near = (is_t0 ? t0 : t1);
    // if camera is within cube, start from camera, else move inside a little
    t_near = (t_near < 0.0 ? 0.0 : t_near);
    float t_far = (is_t0 ? t1 : t0);

    sample_pos = CameraPos + sampleStep * t_near;
    max_num_samples = uint(ceil(abs(t_far - t_near)));
  }

  // initial sample is assumed to be outside the object 
  float prev_density = -limit; 

  uint num_samples = 0u;
  while (num_samples < max_num_samples) {
    num_samples += 1u;
     // get sample
    float density = sample(sample_pos);

    // check if cell is inside contour
    if (density > IsoValue) {
      // approximate ray-cell intersection
      sample_pos = (sample_pos - sampleStep) - sampleStep * (prev_density / (density - prev_density));

      submitFragment(sample_pos);

      writeNumSamples(num_samples);
      return;
    }

    prev_density = density;
    sample_pos += sampleStep;
  }
  // no surface found 
  writeNumSamples(num_samples);
  discard;
}

void submitFragment(const in vec3 sample_pos) {
  float final_density = sample(sample_pos);
  #ifdef GRAD_NORMALS
  vec3 view_normal = normalize((NormalMatrix * vec4(get_gradient(sample_pos), 0.0)).xyz);
  #else
  vec3 view_normal = normalize((NormalMatrix * vec4(blendNormals(sample_pos), 0.0)).xyz);
  #endif
  vec3 view_pos = (gl_ModelViewMatrix * vol_to_world * vec4(sample_pos, 1.0)).xyz;

  if (g_shade_mode == 3) {
    out_Color = vec4(blendCameras(sample_pos), 1.0);
  }
  else {
    vec4 diffuseColor = blendColors(sample_pos);
    out_Color = vec4(shade(view_pos, view_normal, diffuseColor.rgb), diffuseColor.a);
  }
  // apply projection matrix on z component of view-space position
  gl_FragDepth = (gl_ProjectionMatrix[2].z *view_pos.z + gl_ProjectionMatrix[3].z) / -view_pos.z * 0.5f + 0.5f;
}

float sample(const vec3 pos) {
  return texture(volume_tsdf, pos).r;
}

vec3 get_gradient(const vec3 pos) {
  vec3 x_offset = vec3(sampleDistance, 0, 0);
  vec3 y_offset = vec3(0, sampleDistance, 0);
  vec3 z_offset = vec3(0, 0, sampleDistance);
  // invert direction because it points to bigger density
  return -normalize(vec3(
   sample(pos + x_offset) - sample(pos - x_offset),
   sample(pos + y_offset) - sample(pos - y_offset),
   sample(pos + z_offset) - sample(pos - z_offset)));
}

float[5] getWeights(const in vec3 sample_pos) {
  float weights[5] =float[5](0.0, 0.0, 0.0, 0.0, 0.0);
  for (uint i = 0u; i < num_kinects; ++i) {
    vec3 pos_calib = texture(cv_xyz_inv[i], sample_pos).xyz;
    float depth = texture(kinect_depths, vec3(pos_calib.xy, float(i))).r;
    float quality = 0.0;
    // blend if in valid depth range
    float dist = abs(depth - pos_calib.z);
    if(dist < limit) {
      quality = texture(kinect_qualities, vec3(pos_calib.xy, float(i))).r;
    }

    weights[i] = quality;
  }
  return weights;
}
float[5] getDistances(const in vec3 sample_pos) {
  float distances[5] =float[5](0.0, 0.0, 0.0, 0.0, 0.0);
  for (uint i = 0u; i < num_kinects; ++i) {
    vec3 pos_calib = texture(cv_xyz_inv[i], sample_pos).xyz;
    float depth = texture(kinect_depths, vec3(pos_calib.xy, float(i))).r;
    distances[i] = abs(depth - pos_calib.z);
  }
  return distances;
}
float[5] getQualities(const in vec3 sample_pos) {
  float qualities[5] =float[5](0.0, 0.0, 0.0, 0.0, 0.0);
  for (uint i = 0u; i < num_kinects; ++i) {
    vec3 pos_calib = texture(cv_xyz_inv[i], sample_pos).xyz;
    float quality = 0.0;
    quality = texture(kinect_qualities, vec3(pos_calib.xy, float(i))).r;
    qualities[i] = quality;
  }
  return qualities;
}

float[5] getNormalDev(const in vec3 sample_pos) {
  vec3 normal = get_gradient(sample_pos);
  vec3[5] normals = getNormals(sample_pos);
  float weights[5] =float[5](0.0, 0.0, 0.0, 0.0, 0.0);
  for (uint i = 0u; i < num_kinects; ++i) {
    weights[i] = min(dot(-normal, normals[i]), 0.0);
  }
  return weights;
}

float[5] getNormalMax(const in vec3 sample_pos) {
  vec3 normal = get_gradient(sample_pos);
  vec3[5] normals = getNormals(sample_pos);
  float weights[5] =float[5](0.0, 0.0, 0.0, 0.0, 0.0);
  float min_dev = min(dot(-normal, normals[0]), 0.0);
  uint min_i = 0u;
  for (uint i = 1u; i < num_kinects; ++i) {
    float dev = min(dot(-normal, normals[i]), 0.0);
    if(dev < min_dev) {
      min_dev = dev;
      min_i = i;
    }
  }
  weights[min_i] = 1.0;
  return weights;
}

float[5] getNormalTwo(const in vec3 sample_pos) {
  vec3 normal = get_gradient(sample_pos);
  vec3[5] normals = getNormals(sample_pos);
  float weights[5] =float[5](0.0, 0.0, 0.0, 0.0, 0.0);
  float min_dev = min(dot(-normal, normals[0]), 0.0);
  uint min_i = 0u;
  float min_dev2 = min(dot(-normal, normals[1]), 0.0);
  uint min_i2 = 1u;
  for (uint i = 1u; i < num_kinects; ++i) {
    float dev = min(dot(-normal, normals[i]), 0.0);
    if(dev < min_dev2) {
      if(dev < min_dev) {
        min_dev = dev;
        min_i = i;
      }
      else {
        min_dev2 = dev;
        min_i2 = i;
      }
    }
  }
  weights[min_i] = 1.0;
  weights[min_i2] = 1.0;
  return weights;
}

vec3[5] getColors(const in vec3 sample_pos) {
  vec3 colors[5] =vec3[5](vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0));
  for (uint i = 0u; i < num_kinects; ++i) {
    vec3 pos_calib = texture(cv_xyz_inv[i], sample_pos).xyz;
    vec2 pos_color = texture(cv_uv[i], pos_calib).xy;
    colors[i] = texture(kinect_colors, vec3(pos_color.xy, float(i))).rgb;
  }
  return colors;
}

vec3[5] getNormals(const in vec3 sample_pos) {
  vec3 normals[5] =vec3[5](vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0));
  for (uint i = 0u; i < num_kinects; ++i) {
    vec3 pos_calib = texture(cv_xyz_inv[i], sample_pos).xyz;
    normals[i] = texture(kinect_normals, vec3(pos_calib.xy, float(i))).rgb;
  }
  return normals;
}

vec4 blendColors2(const in vec3 sample_pos) {
  vec3 total_color = vec3(0.0);
  vec3[5] colors = getColors(sample_pos);
  float total_weight = 0.0;
  float[5] weights = getWeights(sample_pos);
  for(uint i = 0u; i < num_kinects; ++i) {
    total_color += colors[i] * weights[i];
    total_weight += weights[i];
  }
  // total_color /= total_weight;
  // if(total_weight > 0.0) 
  //   return vec4(total_color, 1.0);
  // else
  //   return vec4(vec3(0.5), 1.0);
  total_color = vec3(0.0);
  // weights = getWeights_inv(sample_pos);
  float[5] distances = getDistances(sample_pos);
  float[5] qualities = getQualities(sample_pos);
  float[5] normalDev = getNormalDev(sample_pos);
  // float[5] normalMax = getNormalMax(sample_pos);
  float[5] normalTwo = getNormalTwo(sample_pos);
  for(uint i = 0u; i < num_kinects; ++i) {
    // smooth
    // float weight = qualities[i] * normalDev[i];
    // float weight = weights[i];
    // float weight = 1.0 / distances[i];
    // sharper
    // float weight = normalTwo[i] / distances[i];
    float weight = normalDev[i] / distances[i];
    total_color += colors[i] * weight;
    total_weight += weight;
  }
  total_color /= total_weight;
  return vec4(total_color, -1.0);
}

vec4 blendColors(const in vec3 sample_pos) {
  vec3 total_color = vec3(0.0);
  vec3 total_color2 = vec3(0.0);
  float total_weight = 0.0;
  float total_weight2 = 0.0;
  vec3 colors[5] =vec3[5](vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0));
  float distances[5] =float[5](0.0, 0.0, 0.0, 0.0, 0.0);
  for(uint i = 0u; i < num_kinects; ++i) {
    vec3 pos_calib = texture(cv_xyz_inv[i], sample_pos).xyz;
    vec2 pos_color = texture(cv_uv[i], pos_calib).xy;
    colors[i] = texture(kinect_colors, vec3(pos_color.xy, float(i))).rgb;

    float depth = texture(kinect_depths, vec3(pos_calib.xy, float(i))).r;
    float quality = 0.0;
    distances[i] = abs(depth - pos_calib.z);
    // blend if in valid depth range
    if(distances[i] < limit) {
      quality = texture(kinect_qualities, vec3(pos_calib.xy, float(i))).r;
    }
    // quality plus inverse distance
    total_color += colors[i] * quality / (distances[i] + 0.01);
    total_weight += quality / (distances[i] + 0.01);
    // fallback color blending
    total_color2 += colors[i] / distances[i];
    total_weight2 += 1.0 / distances[i];
  }
  if(total_weight > 0.0) {
    total_color /= total_weight;
    return vec4(total_color, 1.0);
  } 
  else {
    total_color2 /= total_weight2;
    // return vec4(vec3(0.0), -1.0);
    return vec4(total_color2, -1.0);
  }
}

vec3 blendNormals(const in vec3 sample_pos) {
  vec3 total_normal = vec3(0.0);
  vec3[5] normals = getNormals(sample_pos);
  float total_weight = 0.0;
  float[5] weights = getWeights(sample_pos);
  for(uint i = 0u; i < num_kinects; ++i) {
    total_normal += normals[i] * weights[i];
    total_weight += weights[i];
  }

  total_normal /= total_weight;
  return total_normal;
}

vec3 blendCameras(const in vec3 sample_pos) {
  vec3 total_color = vec3(0.0);
  float total_weight = 0.0;
  float[5] weights = getWeights(sample_pos);
  for(uint i = 0u; i < num_kinects; ++i) {
    vec3 pos_calib = texture(cv_xyz_inv[i], sample_pos).xyz;
    vec3 color = camera_colors[i];

    total_color += color * weights[i];
    total_weight += weights[i];
  }

  total_color /= total_weight;
  if(total_weight <= 0.0) total_color = vec3(1.0);
  return total_color;
}

bool intersectBox(const vec3 origin, const vec3 dir, out float t0, out float t1) {
  vec3 invR = 1.0 / dir;
  vec3 tbot = invR * (vec3(0.0) - origin);
  vec3 ttop = invR * (vec3(1.0) - origin);
  vec3 tmin = min(ttop, tbot);
  vec3 tmax = max(ttop, tbot);
  vec2 t = max(tmin.xx, tmin.yz);
  t0 = max(t.x, t.y);
  t = min(tmax.xx, tmax.yz);
  t1 = min(t.x, t.y);
  return t0 <= t1;
}

vec3 screenToVol(vec3 frag_coord) {
  vec4 position_curr = img_to_eye_curr * vec4(frag_coord,1.0);
  vec4 position_curr_es = vec4(position_curr.xyz / position_curr.w, 1.0);
  vec4 position_curr_ws = inverse(gl_ModelViewMatrix) * position_curr_es;
  vec3 position_vol = (inverse(vol_to_world) * position_curr_ws).xyz;
  return position_vol;  
}

vec4 getStartPos(ivec2 coords) {
  vec3 depthMinMax = texelFetch(depth_peels, coords, 0).rgb;
  // if closest back face is closest face -> front face culled
  depthMinMax.r = (depthMinMax.r >= depthMinMax.b) ? gl_DepthRange.near : depthMinMax.r;
  vec3 pos_front = screenToVol(vec3(gl_FragCoord.xy - viewport_offset,depthMinMax.r));
  vec3 pos_back = screenToVol(vec3(gl_FragCoord.xy - viewport_offset,-depthMinMax.g));
  //no valid closest face found
  pos_back = (depthMinMax.r >= 1.0) ? pos_front : pos_back; 
  return vec4(pos_front, distance(pos_front, pos_back));
}

void writeNumSamples(uint num_samples) {
  float samples = float(num_samples) * 0.0027; 
  imageStore(tex_num_samples, ivec2(gl_FragCoord.xy), vec4(samples, 0.0, 0.0, 0.0));
}
