#extension GL_EXT_gpu_shader4 : enable
#extension GL_EXT_texture_array : enable

uniform sampler2DArray kinect_depths;
uniform sampler3D cv_xyz;
uniform sampler3D cv_uv;

uniform float cv_min_d;
uniform float cv_max_d;
uniform vec2 tex_size_inv;
uniform int layer;

varying vec2 to_geom_texture_coord;
varying vec3 to_geom_pos_es;
varying vec3 to_geom_pos_cs;
varying float to_geom_depth;
varying float to_geom_lateral_quality;
varying float to_geom_vizvalue;

int kernel_size = 6; // in pixel
int kernel_end = kernel_size + 1;


float dist_space_max_inv = 1.0/float(kernel_size);
float computeGaussSpace(float dist_space){
  float gauss_coord = dist_space * dist_space_max_inv;
  return 1.0 - gauss_coord;//texture2D(gauss,vec2(gauss_coord,0.5)).r;
}

float dist_range_max = 0.05; // in meter
float dist_range_max_inv = 1.0/dist_range_max;
float computeGaussRange(float dist_range){
  float gauss_coord = min(dist_range, dist_range_max) * dist_range_max_inv;
  return 1.0 - gauss_coord;//texture2D(gauss,vec2(gauss_coord,0.5)).r;
}

bool is_outside(float d){
  return (d < cv_min_d) || (d > cv_max_d);
}

vec2 bilateral_filter(){

  vec3 coords = vec3(gl_Vertex.xy,layer);
  float depth = texture2DArray(kinect_depths, coords).r;
  if(is_outside(depth)){
    return vec2(0.0,0.0);
  }

  // the valid range scales with depth
  float max_depth = 4.5; // Kinect V2
  float d_dmax = depth/max_depth;
  dist_range_max = 0.35 * d_dmax; // threshold around 
  dist_range_max_inv = 1.0/dist_range_max;



  float depth_bf = 0.0;

  float w = 0.0;
  float w_range = 0.0;
  float border_samples = 0.0;
  float num_samples = 0.0;
  for(int y = -kernel_size; y < kernel_end; ++y){
    for(int x = -kernel_size; x < kernel_end; ++x){
      num_samples += 1.0;
      vec3 coords_s = vec3(coords.s + float(x) * tex_size_inv.x, coords.t + float(y) * tex_size_inv.y, float(layer));
      
      float depth_s = texture2DArray(kinect_depths, coords_s).r;
      float depth_range = abs(depth_s - depth);
      if(is_outside(depth_s) || (depth_range > dist_range_max)){
	border_samples += 1.0;
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

  float lateral_quality  = 1.0 - border_samples/num_samples;
  float filtered_depth = 0.0;
  if(w > 0.0)
    filtered_depth = depth_bf/w;
  else
    filtered_depth = 0.0;

#if 1
  if(w_range < (num_samples * 0.65)){
    filtered_depth = 0.0;
  }
#endif

  return vec2(filtered_depth, pow(lateral_quality,30.0));
}


void main() {

  vec2 bf_result          = bilateral_filter();
  float depth             = bf_result.x;

  // lookup from calibvolume
  float d_idx = (depth - cv_min_d)/(cv_max_d - cv_min_d);

  to_geom_pos_cs        = texture3D(cv_xyz, vec3(gl_Vertex.xy, d_idx)).rgb;
  to_geom_pos_es        = (gl_ModelViewMatrix * vec4(to_geom_pos_cs, 1.0)).xyz;
  to_geom_texture_coord = texture3D(cv_uv,  vec3(gl_Vertex.xy, d_idx)).rg;
  to_geom_depth         = depth;
  to_geom_lateral_quality = bf_result.y;

  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(to_geom_pos_cs, 1.0);

}
