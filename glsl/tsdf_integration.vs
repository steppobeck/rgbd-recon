#version 430
#extension GL_EXT_texture_array : enable

in vec3 in_Position; 
// input
uniform sampler2DArray kinect_colors;
uniform sampler2DArray kinect_depths;
uniform sampler2DArray kinect_qualities;
// calibration
// uniform sampler3D[5] cv_xyz;
// uniform sampler3D[5] cv_uv;

uniform sampler3D[5] cv_xyz_inv;
uniform sampler3D[5] cv_uv_inv;

layout(r32f) uniform image3D volume_tsdf;

uniform float limit;
uniform uint num_kinects;
uniform uvec3 res_tsdf;
uniform uvec2 res_depth;

uniform mat4 gl_ModelViewMatrix;
uniform mat4 gl_ProjectionMatrix;

flat out vec3 geo_pos_volume;
flat out vec3 geo_pos_world;
flat out vec3 geo_pos_view;
flat out vec2 geo_texcoord;

bool is_outside(float depth) {
  return depth < 0.0f || depth > 1.0f;
}

void main() {
  geo_pos_volume = in_Position;
  float weighted_tsd = limit;
  float weight = 0;
  for (uint i = 0u; i < num_kinects; ++i) {
    // uint i = 0;
    vec3 pos_calib = texture(cv_xyz_inv[i], geo_pos_volume).xyz;
    float depth = texture2DArray(kinect_depths, vec3(pos_calib.xy, float(i))).r;
    // float lateral_quality = texture2DArray(kinect_qualities, vec3(pos_calib.xy * res_depth, float(i))).r;
    // float quality = lateral_quality/(pos_calib.z * 4.0f+ 0.5f);

    float sdist = depth - pos_calib.z;
    float tsd = clamp(sdist, -limit, limit);
    if (is_outside(depth)) {
      // weight = 1.0f;
      // weighted_tsd = limit;
      // weighted_tsd = 10.0f;
      tsd = 10.0f;
      // break;
    }
    if(pos_calib.x < 0.0f) tsd = 10.0f;
    // weighted_tsd += quality * tsd;
    // weight += quality;
    if(abs(tsd) < abs(weighted_tsd)) weighted_tsd = tsd;
    // weighted_tsd = tsd;
  }
    // weighted_tsd = pos_calib.x;
  // weighted_tsd /= weight;
  // weighted_tsd = tsd;
  // pos_calib = geo_pos_volume;
  // geo_pos_world  = texture(cv_xyz[0], geo_pos_volume).rgb;

  geo_pos_view   = (gl_ModelViewMatrix * vec4(geo_pos_world, 1.0)).xyz;
  // geo_texcoord   = texture(cv_uv[layer], pos_calib).xy;
  geo_texcoord   = texture(cv_uv_inv[0], geo_pos_volume).xy;
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(geo_pos_world, 1.0);

  // vec3 pos_vol = (world_to_vol * vec4(geo_pos_world, 1.0f)).xyz; 
  ivec3 ipos_vol = ivec3(round(in_Position * res_tsdf));
  imageStore(volume_tsdf, ipos_vol, vec4(weighted_tsd, 0.0f, 0.0f, 0.0f));
}