#extension GL_EXT_gpu_shader4 : enable
#extension GL_EXT_texture_array : enable


uniform sampler2DArray kinect_colors;
uniform sampler2DArray depth_map_curr;
uniform int layer;

uniform mat4 img_to_eye_curr;
uniform vec2 viewportSizeInv;
uniform vec2 offset;

uniform float epsilon;

uniform float groundlevel; // depr

varying vec2 tex_coord_int;
varying vec3 pos_es_int;
varying float depth_int;
varying vec2 ddepth_int;
varying vec3 pos_d_int;
varying vec3 pos_ws_int;
///////////////////////////////////////////////////////////////////////////////

vec3 bbx_min = vec3(-1.0,-0.1, -1.0);
vec3 bbx_max = vec3( 1.0,2.0,   1.0);

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
#if 1
  if(clip(pos_ws_int))
    discard;
#endif

  vec3  coords = vec3(gl_FragCoord.xy * viewportSizeInv, 0.0 /*here layer is always 0*/);
  float depth_curr = texture2DArray(depth_map_curr, coords).r;
  vec4  position_curr = img_to_eye_curr * vec4(gl_FragCoord.xy + vec2(0.5,0.5),depth_curr,1.0);
  vec3  position_curr_es = (position_curr / position_curr.w).xyz;
  if(epsilon < length(position_curr_es - pos_es_int))
    discard;



  // caclulate adhoc normal from ddepth
  vec3 a_d = dFdx(pos_d_int);
  vec3 b_d = -dFdy(pos_d_int);
  vec3 normal_d = normalize(cross(b_d,a_d));
  float d_angle = dot(normalize(-pos_d_int),normal_d);

#if 1
  // back face culling
  if(d_angle < 0.075)
     discard;
#endif

  // to cull away borders of the rgb camera view
#if 1
   if(tex_coord_int.s > 0.975 || tex_coord_int.s < 0.025 ||
      tex_coord_int.t > 0.975 || tex_coord_int.t < 0.025)
        discard;
#endif





  vec3 pos_d_to_depth_camera = normalize(pos_d_int);
  pos_d_to_depth_camera.x = pos_d_to_depth_camera.x;
  pos_d_to_depth_camera.y = pos_d_to_depth_camera.y;
  pos_d_to_depth_camera.z = -pos_d_to_depth_camera.z;

  float cosphi = dot(normal_d, pos_d_to_depth_camera);
  float depth_int2 = (depth_int * depth_int);
  float quality = 1000.0;

  if(depth_int2 > 0.0)
    quality = cosphi / depth_int2;
  else
    quality = 0.0;
  quality = quality * quality;


  vec4 color = texture2DArray(kinect_colors, vec3(tex_coord_int, float(layer)));
  gl_FragColor = vec4(color.rgb * quality, quality);



#if 0
   // to visualize the results of bilateral filtering
   vec3 a_es = dFdx(pos_es_int);
   vec3 b_es = -dFdy(pos_es_int);
   vec3 normal_es = normalize(cross(b_es,a_es));
   gl_FragColor = vec4(normal_es.xyz * quality, quality);
#endif

#if 0
   // to visualize the quality and other stuff
   gl_FragColor = vec4(normal_d,1.0);
#endif
}


///////////////////////////////////////////////////////////////////////////////

