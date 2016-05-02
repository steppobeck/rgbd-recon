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
varying vec2  texture_coord;
varying vec3  pos_es;
varying vec3  pos_cs;
varying float sq_area_cs;

varying float depth;
varying float lateral_quality;
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

  if(clip(pos_cs) && bbxclip > 0)
    discard;

#if 1
   // to cull away borders of the rgb camera view
   if(texture_coord.s > 0.99 || texture_coord.s < 0.01 ||
      texture_coord.t > 0.99 || texture_coord.t < 0.01) {
        discard;
   }
#endif

   vec3 normal = normalize(normal_es);
#if 1
// backface culling
   if ( dot ( normal, -normalize(pos_es) ) > 0.0 ) {
     discard;
   }
#else
   if ( dot ( normal, -normalize(pos_es) ) < 0.0 ) {
     normal = -normal;
   }
#endif


   //float quality = 1.0/(depth * depth);
   float quality = lateral_quality/depth;
   //float packed_normal = pack_vec3(normal);


   if(stage > 0){ // accumulation pass write color and quality if within epsilon
     vec3  coords = vec3(gl_FragCoord.xy * viewportSizeInv, 0.0 /*here layer is always 0*/);
     float depth_curr = texture2DArray(depth_map_curr, coords).r;
     vec4  position_curr = img_to_eye_curr * vec4(gl_FragCoord.xy + vec2(0.5,0.5),depth_curr,1.0);
     vec3  position_curr_es = (position_curr / position_curr.w).xyz;
     if(epsilon < length(position_curr_es - pos_es)){
       discard;
     }

     vec4 color = texture2DArray(kinect_colors, vec3(texture_coord, float(layer)));
     gl_FragColor = vec4(color.rgb * quality, quality);
     //gl_FragColor = vec4(1.5*normal.bgr * quality, quality);
     // gl_FragColor = vec4(quality);
   }
}