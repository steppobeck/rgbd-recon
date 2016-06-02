#version 130 
#extension GL_ARB_explicit_attrib_location : enable

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texcoord;

noperspective out vec2 pass_TexCoord;

void main(void) {
   gl_Position = vec4(position, 0.0f, 1.0f);
   pass_TexCoord = texcoord;
}
