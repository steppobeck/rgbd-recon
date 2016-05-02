#extension GL_EXT_gpu_shader4 : enable
#extension GL_EXT_texture_array : enable

uniform int stage;

// used by accumulation pass
uniform sampler2DArray kinect_colors;
uniform sampler2DArray depth_map_curr;
uniform int layer;

uniform mat4 img_to_eye_curr;
uniform vec2 viewportSizeInv;
uniform vec2 offset;
uniform float epsilon;

/*uniform*/ int bbxclip = 1;
//uniform vec3 bbx_min;
//uniform vec3 bbx_max;
vec3 bbx_min = vec3(-1.,0.0, -1.);
vec3 bbx_max = vec3( 1.,2.2,   1.);

///////////////////////////////////////////////////////////////////////////////
// input
///////////////////////////////////////////////////////////////////////////////
varying vec2  pass_texcoord;
varying vec3  pass_pos_es;
varying vec3  pass_pos_cs;

varying float pass_depth;
varying float pass_lateral_quality;
varying vec3  normal_es;

// methods 

bool clip(vec3 p){
  if(p.x < bbx_min.x ||
     p.y < bbx_min.y ||
     p.z < bbx_min.z ||
     p.x > bbx_max.x ||
     p.y > bbx_max.y ||
     p.z > bbx_max.z){
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// main
///////////////////////////////////////////////////////////////////////////////
void main() {

  if(clip(pass_pos_cs) && bbxclip > 0)
    discard;

#if 1
   // to cull away borders of the rgb camera view
   if(pass_texcoord.s > 0.99 || pass_texcoord.s < 0.01 ||
      pass_texcoord.t > 0.99 || pass_texcoord.t < 0.01) {
        discard;
   }
#endif

   //float quality = 1.0/(pass_depth * pass_depth);
   float quality = pass_lateral_quality/pass_depth;
   //float packed_normal = pack_vec3(normal);


   if(stage > 0){ // accumulation pass write color and quality if within epsilon
     vec3  coords = vec3(gl_FragCoord.xy * viewportSizeInv, 0.0 /*here layer is always 0*/);
     float depth_curr = texture2DArray(depth_map_curr, coords).r;
     vec4  position_curr = img_to_eye_curr * vec4(gl_FragCoord.xy + vec2(0.5,0.5),depth_curr,1.0);
     vec3  position_curr_es = (position_curr / position_curr.w).xyz;
     if(epsilon < length(position_curr_es - pass_pos_es)){
       discard;
     }

     vec4 color = texture2DArray(kinect_colors, vec3(pass_texcoord, float(layer)));
     gl_FragColor = vec4(color.rgb, quality);
     gl_FragColor = vec4(1.0f);
     //gl_FragColor = vec4(1.5*normal.bgr * quality, quality);
   }
}