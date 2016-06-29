#version 130

out vec4 out_Color; 

void main() {
  out_Color = vec4(gl_FragCoord.z, -gl_FragCoord.z, 0.0, 1.0);
}