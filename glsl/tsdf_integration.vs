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
uniform uint num_kinects;
uniform uvec3 res_tsdf;

void main() {
  float weighted_tsd = limit;
  float total_weight = 0;
  for (uint i = 0u; i < num_kinects; ++i) {
    vec3 pos_calib = texture(cv_xyz_inv[i], in_Position).xyz;
    float silhouette = texture(kinect_silhouettes, vec3(pos_calib.xy, float(i))).r;
    if (silhouette < 1.0f) {
      // no write yet -> voxel outside of surface
      if (weighted_tsd >= limit) {
        weighted_tsd = -limit;
        continue;
      }
    }
    float depth = texture(kinect_depths, vec3(pos_calib.xy, float(i))).r;
    float sdist = pos_calib.z - depth;
    if (sdist <= -limit ) {
      weighted_tsd = -limit;
      // break;
    }
    else if (sdist >= limit ) {
      // do nothing
    }
    else {
      float weight = texture(kinect_qualities, vec3(pos_calib.xy, float(i))).r;

      weighted_tsd = (weighted_tsd * total_weight + weight * sdist) / (total_weight + weight);
      total_weight += weight;
    }
  }
  // coordinates must be even pixels
  ivec3 ipos_vol = ivec3(in_Position * res_tsdf);
  imageStore(volume_tsdf, ipos_vol, vec4(weighted_tsd, 0.0f, 0.0f, 0.0f));
}