#extension GL_EXT_gpu_shader4 : enable
#extension GL_EXT_texture_array : enable

varying vec3 pos_es;
varying vec3 pos_ws;
varying vec3 normal_ws;

void main(){

  gl_Position = ftransform();
  pos_ws = gl_Vertex.xyz;
  pos_es = (gl_ModelViewMatrix * gl_Vertex).xyz;
  normal_ws = gl_Normal.xyz;

}
