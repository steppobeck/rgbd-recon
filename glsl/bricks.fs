#version 130

out vec4 out_Color; 

void main() {
  out_Color = vec4(gl_FragCoord.z, -gl_FragCoord.z, gl_FrontFacing ? 1.0 : gl_FragCoord.z, 1.0);
}