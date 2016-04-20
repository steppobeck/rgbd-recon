#extension GL_EXT_gpu_shader4 : enable
#extension GL_EXT_texture_array : enable

uniform sampler3D cv_xyz;
uniform sampler3D cv_uv;


uniform sampler3D cv_error3D;
uniform sampler3D cv_error2D;
uniform sampler3D cv_nnistats;

uniform int viztype;

uniform float d_coord;

varying vec3 pos_es;
varying vec3 pos_cs;
varying float vizvalue;

void main(void){

  pos_cs = texture3D(cv_xyz, vec3(gl_Vertex.xy, d_coord)).rgb;
  pos_es      = (gl_ModelViewMatrix * vec4(pos_cs,1.0)).xyz;

  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(pos_cs, 1.0);


  
  switch(viztype){

  case 1: // error3D
    vizvalue = texture3D(cv_error3D,  vec3(gl_Vertex.xy, d_coord)).r;
    break;

  case 2: // error2D
    vizvalue = texture3D(cv_error2D,  vec3(gl_Vertex.xy, d_coord)).r;
    break;

  case 3: // nnistats
    vizvalue = texture3D(cv_nnistats,  vec3(gl_Vertex.xy, d_coord)).r;
    break;

  default:
    break;
  }


}
