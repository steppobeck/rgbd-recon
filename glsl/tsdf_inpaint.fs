#version 130

noperspective in vec2 pass_TexCoord;

uniform sampler2DArray texture_color;
uniform sampler2DArray texture_depth;

out vec4 out_FragColor;
out float gl_FragDepth;

void main() {
  out_FragColor = vec4(texture(texture_color, vec3(pass_TexCoord, 0.0f)).rgb, 1.0f);
  gl_FragDepth = texture(texture_depth, vec3(pass_TexCoord, 0.0f)).r;
}
