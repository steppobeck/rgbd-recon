#version 130
#extension GL_EXT_gpu_shader4 : enable

flat in vec3 geo_pos_view;
flat in vec3 geo_pos_world;
flat in vec3 geo_pos_volume;
flat in vec2 geo_texcoord;

out vec4 gl_FragColor;

void main() {
   gl_FragColor = vec4(geo_pos_volume, 1.0f);
}