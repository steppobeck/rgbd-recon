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

uniform bool skipSpace;
uniform sampler2D depth_peels;
uniform vec2 viewportSizeInv;
uniform mat4 img_to_eye_curr;

float sampleDistance = limit * 0.5f;
const float IsoValue = 0.0f;
const int refinement_num = 4;

out vec4 out_Color;
out float out_Samples;

out float gl_FragDepth;

#include </shading.glsl>

#include </inc_bbox_test.glsl>

#define GRAD_NORMALS

vec3 get_gradient(const vec3 pos);
bool isInside(const vec3 pos);
float sample(const vec3 pos);
vec4 blendColors(const in vec3 sample_pos);
vec3 blendNormals(const in vec3 sample_pos);
vec3 blendCameras(const in vec3 sample_pos);
// cube-ray intersection from http://prideout.net/blog/?p=64
bool intersectBox(const vec3 origin, const vec3 dir, out float t0, out float t1);
void submitFragment(const in vec3 sample_pos);
vec4 getStartPos(ivec2 coords);

void main() {
  out_Samples = 0.0;
  // multiply with dimensions to scale direction by dimension relation
  vec3 sampleStep = normalize(pass_Position - CameraPos) * sampleDistance;
  // get ray beginning in volume cube
  float t0, t1 = 0.0f;
  bool is_t0 = intersectBox(CameraPos, sampleStep, t0, t1);
  float t_near = (is_t0 ? t0 : t1);
  // if camera is within cube, start from camera, else move inside a little
  t_near = (t_near < 0.0f ? 0.0f : t_near * 1.0000001f);

  vec3 sample_pos = CameraPos + sampleStep * t_near;
  float end = 0.0;
  if (skipSpace) { 
    vec4 posEnd = getStartPos(ivec2(gl_FragCoord.xy));
    sample_pos = posEnd.xyz;
  }
  // out_Color.rgb = sample_pos;
  // return;
  bool inside = isInside(sample_pos);  
  // cache value of previous sample
  float prev_density = sample(sample_pos); 

  while (inside) {
    ++out_Samples;
     // get sample
    float density = sample(sample_pos);

    // check if cell is inside contour
    if (density > IsoValue && prev_density <= IsoValue) {
      // approximate ray-cell intersection
      sample_pos = (sample_pos - sampleStep) - sampleStep * (prev_density / (density - prev_density));

      submitFragment(sample_pos);
      return;
    }

    prev_density = density;
    sample_pos += sampleStep;
    inside = isInside(sample_pos); 
  }
  // no surface found 
  discard;
}

void submitFragment(const in vec3 sample_pos) {
  float final_density = sample(sample_pos);
  #ifdef GRAD_NORMALS
  vec3 view_normal = normalize((NormalMatrix * vec4(get_gradient(sample_pos), 0.0f)).xyz);
  #else
  vec3 view_normal = normalize((NormalMatrix * vec4(blendNormals(sample_pos), 0.0f)).xyz);
  #endif
  vec3 view_pos = (gl_ModelViewMatrix * vol_to_world * vec4(sample_pos, 1.0f)).xyz;

  if (g_shade_mode == 3) {
    out_Color = vec4(blendCameras(sample_pos), 1.0f);
  }
  else {
    vec4 diffuseColor = blendColors(sample_pos);
    out_Color = vec4(shade(view_pos, view_normal, diffuseColor.rgb), diffuseColor.a);
  }
  // apply projection matrix on z component of view-space position
  gl_FragDepth = (gl_ProjectionMatrix[2].z *view_pos.z + gl_ProjectionMatrix[3].z) / -view_pos.z * 0.5f + 0.5f;
}

bool isInside(const vec3 pos) {
  // add tolarance for bricks at bbox borders
  return pos.x >= -0.001 && pos.x <= 1.001
      && pos.y >= -0.001 && pos.y <= 1.001
      && pos.z >= -0.001 && pos.z <= 1.001;
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
  float weights[5] =float[5](0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
  for (uint i = 0u; i < num_kinects; ++i) {
    vec3 pos_calib = texture(cv_xyz_inv[i], sample_pos).xyz;
    float depth = texture(kinect_depths, vec3(pos_calib.xy, float(i))).r;
    float quality = 0.0f;
    // blend if in valid depth range
    if(abs(depth - pos_calib.z) < limit) {
      quality = texture(kinect_qualities, vec3(pos_calib.xy, float(i))).r;
    }

    weights[i] = quality;
  }
  return weights;
}

vec4 blendColors(const in vec3 sample_pos) {
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
  if(total_weight <= 0.0f) return vec4(vec3(0.0), -1.0);
  return vec4(total_color, 1.0);
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
  if(total_weight <= 0.0f) total_color = vec3(1.0f);
  return total_color;
}

bool intersectBox(const vec3 origin, const vec3 dir, out float t0, out float t1) {
    vec3 invR = 1.0f / dir;
    vec3 tbot = invR * (vec3(0.0f) - origin);
    vec3 ttop = invR * (vec3(1.0f) - origin);
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
  vec2 depthMinMax = texelFetch(depth_peels, coords, 0).rg;
  vec3 pos_front = screenToVol(vec3(gl_FragCoord.xy,depthMinMax.r));
  vec3 pos_back = screenToVol(vec3(gl_FragCoord.xy,-depthMinMax.g));
  return vec4(pos_front, distance(pos_front, pos_back));
}
