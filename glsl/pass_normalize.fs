#extension GL_EXT_gpu_shader4 : enable
#extension GL_EXT_texture_array : enable


uniform sampler2DArray color_map;
uniform sampler2DArray depth_map;
uniform sampler2DArray color_map_volviz;

uniform vec2 texSizeInv;
uniform vec2 offset;

uniform int viztype;
uniform int black;


void main(){

  vec3 coords = vec3((gl_FragCoord.xy - offset) * texSizeInv, 0.0 /*here layer is always 0*/);

  bool bg = false;
  vec4 final_col = vec4(0.0);
  vec4 col = texture2DArray(color_map, coords);
  float depth = texture2DArray(depth_map, coords).r;
  if(col.a > 0.0){
    final_col = col/col.a;
    bg = true;
  }
  else if(viztype == 0){
    discard;
  }


  if(viztype > 0){
    vec4 col_volviz  = vec4(texture2DArray(color_map_volviz, coords));
    if(bg){
      final_col.rgb = col_volviz.rgb +  (1.0 - col_volviz.a) * final_col.rgb;
    }
    else{
      if(black == 0)
	final_col.rgb = col_volviz.rgb +  (1.0 - col_volviz.a) * vec3(1.0);
      else
	final_col.rgb = col_volviz.rgb;// +  (1.0 - col_volviz.a) * vec3(1.0);
      depth = 0.5;
    }
  }


  gl_FragColor = final_col;
  gl_FragDepth = depth;

    
  //gl_FragColor = vec4(texture2DArray(depth_map, coords).r,0.0,0.0,0.0);
}
