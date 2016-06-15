#version 130
#extension GL_EXT_shader_texture_lod : enable
noperspective in vec2 pass_TexCoord;

uniform sampler2D texture_color;
uniform sampler2D texture_depth;
uniform uvec2 resolution_tex;
out vec4 out_FragColor;
out float gl_FragDepth;

void main() {
  // out_FragColor = vec4(texture(texture_color, pass_TexCoord).rgb, 1.0f);
  // gl_FragDepth = texture(texture_depth, pass_TexCoord).r;
  out_FragColor = vec4(texelFetch(texture_color, ivec2(pass_TexCoord * resolution_tex), 1).rgb, 1.0f);
  gl_FragDepth = texelFetch(texture_depth, ivec2(pass_TexCoord * resolution_tex), 1).r;
  // out_FragColor = vec4(textureLod(texture_color, pass_TexCoord, 0.0f).rgb, 1.0f);
  // gl_FragDepth = textureLod(texture_depth, pass_TexCoord, 0.0f).r;
}
