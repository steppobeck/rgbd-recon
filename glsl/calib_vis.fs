#version 130
#extension GL_EXT_gpu_shader4 : enable

uniform float limit;

flat in vec3 geo_pos_view;
flat in vec3 geo_pos_world;
flat in vec3 geo_pos_volume;
flat in vec2 geo_texcoord;

out vec4 gl_FragColor;

void main() {
	float inverted_dist = 1.0f - abs(geo_texcoord.r) / limit;
   gl_FragColor = vec4(inverted_dist, 0.0f, 0.0f, 1.0f);
   // gl_FragColor = vec4(1.0f);
   // gl_FragColor = vec4(geo_pos_volume, 1.0f);
   // if (geo_texcoord.r < -10.0f) discard;
   // if (geo_texcoord.r > 9.0f) discard;
   if (distance(geo_texcoord.r, 0.0f) >= limit) discard;
}