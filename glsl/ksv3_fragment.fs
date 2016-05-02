#version 130
#extension GL_EXT_gpu_shader4 : enable
#extension GL_EXT_texture_array : enable

uniform int stage;
// used by accumulation pass
uniform sampler2DArray kinect_colors;
uniform sampler2DArray depth_map_curr;
uniform int layer;

uniform mat4 img_to_eye_curr;
uniform vec2 viewportSizeInv;
uniform float epsilon;

const vec3 bbx_min = vec3(-1.,0.0, -1.);
const vec3 bbx_max = vec3( 1.,2.2,   1.);

///////////////////////////////////////////////////////////////////////////////
// input
///////////////////////////////////////////////////////////////////////////////
in vec2  pass_texcoord;
in vec3  pass_pos_es;
in vec3  pass_pos_cs;

in float pass_depth;
in float pass_lateral_quality;
in vec3  pass_normal_es;

out vec4 gl_FragColor;
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

  if(clip(pass_pos_cs)) {
    discard;
  }

#if 1
   // to cull away borders of the rgb camera view
   if(pass_texcoord.s > 0.99 || pass_texcoord.s < 0.01 ||
      pass_texcoord.t > 0.99 || pass_texcoord.t < 0.01) {
        discard;
   }
#endif

   vec3 normal = normalize(pass_normal_es);
#if 1
// backface culling
   if ( dot ( normal, -normalize(pass_pos_es) ) > 0.0 ) {
     discard;
   }
#else
   if ( dot ( normal, -normalize(pass_pos_es) ) < 0.0 ) {
     normal = -normal;
   }
#endif
   float quality = pass_lateral_quality/pass_depth;

   if(stage > 0){ // accumulation pass write color and quality if within epsilon
     vec3  coords = vec3(gl_FragCoord.xy * viewportSizeInv, 0.0 /*here layer is always 0*/);
     float depth_curr = texture2DArray(depth_map_curr, coords).r;
     vec4  position_curr = img_to_eye_curr * vec4(gl_FragCoord.xy + vec2(0.5,0.5),depth_curr,1.0);
     vec3  position_curr_es = position_curr.xyz / position_curr.w;
     // discard if occluded by triangles in front
     if(epsilon < length(position_curr_es - pass_pos_es)) {
       // discard;
       gl_FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
       return;
     }

     vec4 color = texture2DArray(kinect_colors, vec3(pass_texcoord, float(layer)));
     gl_FragColor = vec4(color.rgb * quality, quality);
   }
}