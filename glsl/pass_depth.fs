#extension GL_EXT_gpu_shader4 : enable
#extension GL_EXT_texture_array : enable


uniform sampler2DArray kinect_colors;
//uniform sampler2DArray kinect_depths_video;
uniform int layer;

uniform vec3 pos_min;
uniform vec3 pos_max;

uniform vec3 neg_min;
uniform vec3 neg_max;

uniform float groundlevel; // depr

#if 0
uniform int direct;
#endif

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
#if 0
  if(clip(pos_ws_int))
    discard;
#endif

  gl_FragColor = vec4(1.0);

#if 0
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

  if(depth_int2 > 0)
    quality = cosphi / depth_int2;
  else
    quality = 0.0;

  quality = quality * quality;

  float dist_es = length(pos_es_int);



#if 0
  if(direct){
    gl_FragColor = texture2DArray( kinect_colors, vec3(tex_coord_int.xy,float(layer)));
  }
  else{
    gl_FragColor = vec4(tex_coord_int, dist_es, quality);
  }
#else
  gl_FragColor = vec4(tex_coord_int, dist_es, quality);
#endif


#if 0
   // to visualize the results of bilateral filtering
   vec3 a_es = dFdx(pos_es_int);
   vec3 b_es = -dFdy(pos_es_int);
   vec3 normal_es = normalize(cross(b_es,a_es));
   gl_FragColor = vec4(normal_es.xyz, 1.0);
#endif

#if 0
   // to visualize the quality and other stuff
   gl_FragColor = vec4(normal_d,1.0);
#endif
#endif
}


///////////////////////////////////////////////////////////////////////////////

