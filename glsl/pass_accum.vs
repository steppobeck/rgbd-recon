#extension GL_EXT_gpu_shader4 : enable
#extension GL_EXT_texture_array : enable



uniform sampler2DArray kinect_depths;
uniform vec2 texSizeInv;


uniform mat4 image_d_to_eye_d;
uniform mat4 eye_d_to_world;
uniform mat4 eye_d_to_eye_rgb;
uniform mat4 eye_rgb_to_image_rgb;


uniform int layer;

varying vec2 tex_coord;
varying vec3 pos_es;
varying vec3 pos_d;
varying vec3 pos_ws;

varying float depth;
varying vec2  ddepth;


float lookup(vec2 offset){
  return texture2DArray(kinect_depths, vec3(gl_Vertex.xy + offset, layer)).r;
}

vec2 calc_ddepth(){

  // left
  float dl = lookup(vec2(-texSizeInv.x, 0.0));

  // right
  float dr = lookup(vec2( texSizeInv.x, 0.0));

  // top
  float dt = lookup(vec2( 0.0, texSizeInv.y));

  // bottom
  float db = lookup(vec2( 0.0,-texSizeInv.y));



  return vec2(dr - dl, dt - db);

}




float avgdist(vec2 tex_c, float d){


  float tl = texture2DArray(kinect_depths, vec3(tex_c + vec2(-texSizeInv.s, -texSizeInv.t), layer)).r;   // top left
  float  l = texture2DArray(kinect_depths, vec3(tex_c + vec2(-texSizeInv.s,          0.0), layer)).r;   // left

  float bl = texture2DArray(kinect_depths, vec3(tex_c + vec2(-texSizeInv.s,  texSizeInv.t), layer)).r;   // bottom left
  float  t = texture2DArray(kinect_depths, vec3(tex_c + vec2(        0.0, -texSizeInv.t), layer)).r;   // top

  float  b = texture2DArray(kinect_depths, vec3(tex_c + vec2(        0.0,  texSizeInv.t), layer)).r;   // bottom
  float tr = texture2DArray(kinect_depths, vec3(tex_c + vec2( texSizeInv.s, -texSizeInv.t), layer)).r;   // top right

  float  r = texture2DArray(kinect_depths, vec3(tex_c + vec2( texSizeInv.s,          0.0), layer)).r;   // bottom
  float br = texture2DArray(kinect_depths, vec3(tex_c + vec2( texSizeInv.s,  texSizeInv.t), layer)).r;   // top right

  // Compute dx using Sobel:
  //           -1 0 1
  //           -2 0 2
  //           -1 0 1
  float dX = tr + 2.0*r + br -tl - 2.0*l - bl;

  // Compute dy using Sobel:
  //           -1 -2 -1
  //            0  0  0
  //            1  2  1
  float dY = bl + 2.0*b + br -tl - 2.0*t - tr;

  return (abs(d - tl) * 0.125 + 
	  abs(d -  l) * 0.125 + 
	  abs(d - bl) * 0.125 + 
	  abs(d -  t) * 0.125 + 
	  abs(d -  b) * 0.125 + 
          abs(d - tr) * 0.125 + 
          abs(d -  r) * 0.125 + 
          abs(d - br) * 0.125);

}


void main(){

  /*float*/ depth = texture2DArray(kinect_depths, vec3(gl_Vertex.xy, layer)).r;
  
  vec4 POS_d = depth * image_d_to_eye_d * vec4(gl_Vertex.xy, depth, 1.0);
  POS_d.z = depth;
  
  POS_d.w = 1.0;
  pos_d   = POS_d.xyz;
  
  vec4 POS_rgb = eye_d_to_eye_rgb * POS_d;		
  
  if(POS_rgb.z > 0.0)
    tex_coord = (eye_rgb_to_image_rgb * vec4( (POS_rgb.xy/POS_rgb.z) ,0.0,1.0)).xy;
  else
    tex_coord = vec2(0.0);
  
  
  vec4 POS_ws =  eye_d_to_world * POS_d;
  
#if 0
  if(POS_ws.y < 0.0)
    POS_ws.y = 0.0;
#endif
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * POS_ws;
  pos_es = (gl_ModelViewMatrix * POS_ws).xyz;
  pos_ws = POS_ws.xyz;
  ddepth = calc_ddepth();
    
}
