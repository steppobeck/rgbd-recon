#version 130
#extension GL_ARB_explicit_attrib_location : enable

noperspective in vec2 pass_TexCoord;

uniform uint layer;
uniform sampler2DArray kinect_depths;
uniform sampler2DArray kinect_colors_lab;

layout(location = 0) out vec2 out_Depth;

// average depth with new value
void main(void) {
  out_Depth = texture(kinect_depths, vec3(pass_TexCoord, layer)).rg;
}