#version 430

in vec3 in_Position; 
// input
uniform sampler2DArray kinect_colors;
uniform sampler2DArray kinect_depths;
uniform sampler2DArray kinect_qualities;
uniform sampler2DArray kinect_normals;
uniform sampler2DArray kinect_silhouettes;

// calibration
uniform sampler3D[5] cv_xyz_inv;

layout(r32f) uniform image3D volume_tsdf;

uniform float limit;
uniform mat4 vol_to_world;
uniform uint num_kinects;
uniform uvec3 res_tsdf;
uniform uvec2 res_depth;

bool is_outside(float depth) {
  return depth < 0.0f || depth > 1.0f;
}

void main() {
  float weighted_tsd = limit;
  float weight = 0;
  for (uint i = 0u; i < num_kinects; ++i) {
    vec3 pos_calib = texture(cv_xyz_inv[i], in_Position).xyz;
    float depth = texture(kinect_depths, vec3(pos_calib.xy, float(i))).r;
    float silhouette = texture(kinect_silhouettes, vec3(pos_calib.xy, float(i))).r;
    if (silhouette < 1.0f) {
      // no write yet -> voxel outside of surface
      if (weighted_tsd >= limit) {
        weighted_tsd = -limit;
        continue;
      }
      // tsd = 10.0f;
      // break;
    }
    float sdist = pos_calib.z - depth;
    // if (sdist <= -limit ) {
    //   // weighted_tsd = -limit;
    //   // break;
    // }
    if (sdist >= limit ) {
      // do nothing
    }
    else {
      if(weighted_tsd >= limit) {
        weighted_tsd = 0.0f;
      }
      float tsd = clamp(sdist, -limit, limit);
      float quality = texture(kinect_qualities, vec3(pos_calib.xy, float(i))).r;

      weighted_tsd = (weighted_tsd * weight + quality * tsd) / (weight + quality);
      weight += quality;
    }
  }

  // coordinates must be even pixels
  ivec3 ipos_vol = ivec3(round(in_Position * res_tsdf - vec3(0.5f)));
  imageStore(volume_tsdf, ipos_vol, vec4(weighted_tsd, 0.0f, 0.0f, 0.0f));
}