#version 130

noperspective in vec2 pass_TexCoord;

uniform sampler2D texture_color;
uniform sampler2D texture_depth;

out vec4 out_FragColor;
out float gl_FragDepth;

void main() {
  out_FragColor = vec4(texture(texture_color, pass_TexCoord).rgb, 1.0f);
  gl_FragDepth = texture(texture_depth, pass_TexCoord).r;
}
