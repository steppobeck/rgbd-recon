#version 130
#extension GL_EXT_gpu_shader4 : enable

uniform float limit;

uniform uint layer;
uniform sampler3D[5] cv_xyz_inv;
uniform sampler3D[5] cv_uv_inv;

flat in vec3 geo_pos_view;
flat in vec3 geo_pos_world;
flat in vec3 geo_pos_volume;
flat in vec2 geo_texcoord;

out vec4 gl_FragColor;

bool clip(vec3 p){
  if(
  	 // p.x < 0.0f ||
     // p.y < 0.0f ||
     p.z < 0.0f
     // p.x > 1.0f ||
     // p.y > 1.0f ||
     // p.z > 1.0f
     ){
    return true;
  }
  return false;
}
void main() {
	float inverted_dist = abs(geo_texcoord.r) / limit;
	if (geo_texcoord.r > 0.0f) {
   	  gl_FragColor = vec4(1.0f -inverted_dist, 0.0f, 0.0f, 1.0f);	
	}
	else {
   	  gl_FragColor = vec4(0.0f, 1.0f - inverted_dist, 0.0f, 1.0f);	
	}
	vec3 color = texture(cv_xyz_inv[layer],geo_pos_volume).rgb;
   	 // if(!clip(color)) discard;
   	 // if(color.z <= 0.0f) discard;
   	  gl_FragColor = vec4(color, 1.0f);	
   // gl_FragColor = vec4(1.0f);
   // gl_FragColor = vec4(geo_pos_volume, 1.0f);
   // if (geo_texcoord.r < -10.0f) discard;
   // if (geo_texcoord.r > 9.0f) discard;
   // if (abs(geo_texcoord.r) >= limit) discard;
   // if (geo_texcoord.r > 0.0f) discard;
}