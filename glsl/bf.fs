#extension GL_EXT_gpu_shader4 : enable
#extension GL_EXT_texture_array : enable



uniform sampler2D gauss;
uniform sampler2DArray kinect_depths;
//uniform sampler2DArray kinect_colors;
uniform vec2 texSizeInv;

uniform int layer;
uniform bool compress;
uniform float scale;
uniform float near;
uniform float scaled_near;



int kernel_size = 5; // in pixel
int kernel_end = kernel_size + 1;


float dist_space_max_inv = 1.0/float(kernel_size);
float computeGaussSpace(float dist_space){
  float gauss_coord = dist_space * dist_space_max_inv;
  return texture2D(gauss,vec2(gauss_coord,0.5)).r;
}

float dist_range_max = 0.05; // in meter
float dist_range_max_inv = 1.0/dist_range_max;
float computeGaussRange(float dist_range){
  float gauss_coord = min(dist_range, dist_range_max) * dist_range_max_inv;

#if 1
  return texture2D(gauss,vec2(gauss_coord,0.5)).r;
#else
  return 1.0;
#endif
}



float uncompress(float d_c){
    if(d_c < scaled_near){
      return 0.0;
    }
    else{
      // linear mapping
      //return (d_c + 0.15 *scaled_near) * scale + near;
      // sqrt-mapping
      return (d_c * d_c + 0.15 *scaled_near) * scale + near;
    }
}


void main( void )
{

  vec3 coords = vec3(gl_TexCoord[0].xy,layer);

  float depth = 0.0;
  if(compress){
    depth = uncompress(texture2DArray(kinect_depths, coords).r);
  }
  else{
    depth = texture2DArray(kinect_depths, coords).r;
  }
  

#if 1
  // weiter hinten liegende samples haben einen groeßeren range
  float max_depth = 7.0;
  float d_dmax = depth/max_depth;

  dist_range_max = 0.5 * d_dmax;
  if(dist_range_max > 0.0)
    dist_range_max_inv = 1.0/dist_range_max;
  else
    dist_range_max_inv = 20.0;
#endif


  float depth_bf = 0.0;

  float w = 0.0;
  float w_range = 0.0;
  for(int y = -kernel_size; y < kernel_end; ++y){
    for(int x = -kernel_size; x < kernel_end; ++x){
      vec3 coords_s = vec3(coords.s + float(x) * texSizeInv.x, coords.t + float(y) * texSizeInv.y, float(layer));


      
      float depth_s = 0.0;
      if(compress){
	depth_s = uncompress(texture2DArray(kinect_depths, coords_s).r);
      }
      else{
	depth_s = texture2DArray(kinect_depths, coords_s).r;
      }
      

      float gauss_space = computeGaussSpace(length(vec2(x,y)));
      float gauss_range = computeGaussRange(abs(depth_s - depth));
      float w_s = gauss_space * gauss_range;
      depth_bf += w_s * depth_s;
      w += w_s;
      w_range += gauss_range;
    }
  }

  if(w > 0.0)
    gl_FragData[0] = vec4(depth_bf/w);
  else
    gl_FragData[0] = vec4(0.0);

#if 1
  if(w_range < 70.1)
    gl_FragData[0] = vec4(0.0);
#endif
}
