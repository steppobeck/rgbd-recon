#version 130 
#extension GL_ARB_explicit_attrib_location : enable

layout(location = 0) in vec3 position;

noperspective out vec2 pass_TexCoord;

void main(void) {
   gl_Position = vec4(position, 1.0f);
   pass_TexCoord = position.xy * 0.5f + 0.5f;
}
