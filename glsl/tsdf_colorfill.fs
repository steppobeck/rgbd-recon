#version 130

noperspective in vec2 pass_TexCoord;

uniform sampler2D texture_color;
uniform sampler2D texture_depth;
uniform uvec2 resolution_tex;
uniform int num_lods;

out vec4 out_FragColor;
out float gl_FragDepth;

void main() {
  int level = 0;
  ivec2 curr_res = ivec2(0);
  for (; level < num_lods; ++level) {
    curr_res = textureSize(texture_color, level);
    out_FragColor = texelFetch(texture_color, ivec2((pass_TexCoord) * curr_res), level);
    if (out_FragColor != vec4(vec3(0.0f), 1.0f) || out_FragColor == vec4(0.0f)) break;
  }

  out_FragColor = textureLod(texture_color, pass_TexCoord, level > 0 ? level + 0.75f : level);
  gl_FragDepth = texelFetch(texture_depth, ivec2(pass_TexCoord * curr_res), level).r;
}
