#version 430

in vec3 in_Position; 
// input
uniform sampler2DArray kinect_colors;
uniform sampler2DArray kinect_depths;
uniform sampler2DArray kinect_qualities;
uniform sampler2DArray kinect_normals;
// calibration
uniform sampler3D[5] cv_xyz_inv;

layout(r32f) uniform image3D volume_tsdf;

uniform float limit;
uniform uint num_kinects;
uniform uvec3 res_tsdf;
uniform uvec2 res_depth;
uniform vec3[5] camera_positions;

bool is_outside(float depth) {
  return depth < 0.0f || depth > 1.0f;
}

float normal_angle(vec3 position, uint layer) {
  vec3 world_normal = texture(kinect_normals, vec3(position.xy, float(layer))).xyz;
  float angle = dot(normalize(camera_positions[layer] - position), world_normal);
  return angle;
}

void main() {
  float weighted_tsd = limit;
  float weight = 0;
  for (uint i = 0u; i < num_kinects; ++i) {
    vec3 pos_calib = texture(cv_xyz_inv[i], in_Position).xyz;
    float depth = texture(kinect_depths, vec3(pos_calib.xy, float(i))).r;
    // if (is_outside(depth)) {
    //   // no write yet -> voxel outside of surface
    //   if (weighted_tsd >= limit) {
    //     weighted_tsd = -limit;
    //     continue;
    //   }
    //   // tsd = 10.0f;
    //   // break;
    // }
    float sdist = depth - pos_calib.z;
    if (sdist <= -limit ) {
      // weighted_tsd = -limit;
      // break;
    }
    else if (sdist >= limit ) {
      // do nothing
    }
    else {
      float tsd = clamp(sdist, -limit, limit);
      float quality = texture(kinect_qualities, vec3(pos_calib.xy, float(i))).r;
      float angle = normal_angle(vec3(in_Position.xy, depth), i);
      quality *= angle;
      weighted_tsd = (weighted_tsd * weight + quality * tsd) / (weight + quality);
      weight += quality;
    }
  }

  // coordinates must be even pixels
  ivec3 ipos_vol = ivec3(round(in_Position * res_tsdf - vec3(0.5f)));
  imageStore(volume_tsdf, ipos_vol, vec4(weighted_tsd, 0.0f, 0.0f, 0.0f));
}