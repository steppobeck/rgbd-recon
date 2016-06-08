#version 130
#extension GL_EXT_gpu_shader4 : enable

uniform float limit;

uniform uint layer;
uniform sampler3D[5] cv_xyz_inv;
uniform sampler3D[5] cv_uv_inv;

flat in vec3 geo_pos_view;
flat in vec3 geo_pos_world;
flat in vec3 geo_pos_volume;
flat in float geo_distance;

out vec4 gl_FragColor;

void main() {
	float inverted_dist = abs(geo_distance) / limit;
	if (geo_distance > 0.0f) {
   	  gl_FragColor = vec4(1.0f -inverted_dist, 0.0f, 0.0f, 1.0f);	
	}
	else {
        gl_FragColor = vec4(0.0f, 1.0f - inverted_dist, 0.0f, 1.0f); 
   }
   vec3 color = texture(cv_xyz_inv[layer],geo_pos_volume).rgb;
   if(geo_distance >= limit) {
	  gl_FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);	   
   }

   if (geo_distance <= -limit) discard;
}