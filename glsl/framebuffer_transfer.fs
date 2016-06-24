#version 130

noperspective in vec2 pass_TexCoord;

uniform sampler2D texture_color;
uniform sampler2D texture_depth;
uniform uvec2 resolution_tex;
uniform int lod;

out vec4 out_FragColor;
out float gl_FragDepth;

void main() {
  out_FragColor = texelFetch(texture_color, ivec2(pass_TexCoord * resolution_tex), lod);
  gl_FragDepth = texelFetch(texture_depth, ivec2(pass_TexCoord * resolution_tex), lod).r;
}
