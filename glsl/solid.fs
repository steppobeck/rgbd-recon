#version 130

out vec4 out_Color; 
uniform vec3 Color;

void main() {
  out_Color = vec4(Color, 1.0);
}