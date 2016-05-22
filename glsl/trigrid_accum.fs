#version 140
#extension GL_ARB_shading_language_include : require

uniform uint stage;
// used by accumulation pass
uniform sampler2DArray kinect_colors;
uniform sampler2DArray kinect_normals;
uniform sampler2DArray depth_map_curr;
uniform uint layer;

uniform mat4 img_to_eye_curr;
uniform vec2 viewportSizeInv;
uniform float epsilon;

uniform mat4 gl_NormalMatrix;

uniform vec3 bbox_min;
uniform vec3 bbox_max;

#include </shading.glsl>
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
  if(p.x < bbox_min.x ||
     p.y < bbox_min.y ||
     p.z < bbox_min.z ||
     p.x > bbox_max.x ||
     p.y > bbox_max.y ||
     p.z > bbox_max.z){
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

   vec3 normal = -normalize(pass_normal_es);
#if 1
// backface culling
   if ( dot ( normal, normalize(pass_pos_es) ) > 0.0 ) {
     discard;
   }
#else
   if ( dot ( normal, -normalize(pass_pos_es) ) < 0.0 ) {
     normal = -normal;
   }
#endif
    // non-normalized depth was between 0.5 and 4.5
   float quality = pass_lateral_quality/(pass_depth * 4.0f+ 0.5f);

   if(stage > 0u){ // accumulation pass write color and quality if within epsilon
     vec3  coords = vec3(gl_FragCoord.xy * viewportSizeInv, 0.0 /*here layer is always 0*/);
     float depth_curr = texture(depth_map_curr, coords).r;
     vec4  position_curr = img_to_eye_curr * vec4(gl_FragCoord.xy + vec2(0.5,0.5),depth_curr,1.0);
     vec3  position_curr_es = position_curr.xyz / position_curr.w;
     // discard if occluded by triangles in front
     if(epsilon < length(position_curr_es - pass_pos_es)) {
       discard;
       gl_FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
       return;
     }

     vec3 color = texture(kinect_colors, vec3(pass_texcoord, float(layer))).rgb;

    #ifdef NORMAL
      vec3 world_normal = (inverse(gl_NormalMatrix) * vec4(normal, 0.0f)).xyz;
      gl_FragColor = vec4(world_normal * quality, quality);
    #else
      gl_FragColor = vec4(shade(pass_pos_es, normal, color) * quality, quality);
    #endif
   }
}