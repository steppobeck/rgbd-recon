#extension GL_EXT_gpu_shader4 : enable
#extension GL_EXT_texture_array : enable

uniform sampler2DArray kinect_colors;
uniform sampler2DArray kinect_depths;

#define MAX_VIEWS 8


uniform mat4 v_world_to_eye_d[MAX_VIEWS];
uniform mat4 v_eye_d_to_world[MAX_VIEWS];
uniform mat4 v_eye_d_to_eye_rgb[MAX_VIEWS];
uniform mat4 v_eye_rgb_to_image_rgb[MAX_VIEWS];

uniform int num_layers;

varying vec3 pos_es;
varying vec3 pos_ws;
varying vec3 normal_ws;

///////////////////////////////////////////////////////////////////////////////

vec3 bbx_min = vec3(-1.0,0.0,-1.0);
vec3 bbx_max = vec3( 1.0,2.0, 1.0);

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


void main(){

  if(clip(pos_ws))
    discard;

  vec4 final_col = vec4(0.0,0.0,0.0,0.0);
  
  for(int i = 0; i < num_layers; i++){

    

    // calc the view dir in ws
    vec3 view_ws = (v_eye_d_to_world[i] * vec4(0.0,0.0,-1.0,0.0)).xyz;

    float w = max(0.0, dot(normalize(view_ws), normalize(normal_ws)));

    vec4 pos_d = v_world_to_eye_d[i] * vec4(pos_ws,1.0);

    float dist = length(pos_d.xyz);
    w /= (dist * dist * dist * dist);
    

    vec4 pos_rgb = v_eye_d_to_eye_rgb[i] * pos_d;
    vec2 tex_rgb = (v_eye_rgb_to_image_rgb[i] * vec4( (pos_rgb.xy/pos_rgb.z) ,0.0,1.0)).xy;

    vec4 color = texture2DArray( kinect_colors, vec3(tex_rgb, float(i)));

    final_col.rgb += w * color.rgb;
    final_col.w += w;

  }
  final_col /= final_col.w;
  gl_FragColor = final_col;

  //gl_FragColor = vec4(normal_ws,0.0);

}


///////////////////////////////////////////////////////////////////////////////

