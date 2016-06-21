#version 130

noperspective in vec2 pass_TexCoord;

uniform sampler2D texture_color;
uniform sampler2D texture_depth;
uniform uvec2 resolution_tex;
uniform int num_lods;

out vec4 out_FragColor;
out float gl_FragDepth;

vec2 to_lod_pos(in vec2 pos, in int lod) {
  if(lod == 0) {
    pos.x / 3.0f * 2.0f;
  } 
  else {
    float fraction = 1.0f / pow(2.0f, float(lod));
    pos.x = 2.0f / 3.0f + pos.x * fraction;
    pos.y = fraction + pos.y * fraction;
  }
  return pos;
}

void main() {
  int level = 0;
  ivec2 curr_res = ivec2(0);
  ivec2 curr_pos = ivec2(0);
  for (; level < num_lods; ++level) {
    curr_res = textureSize(texture_color, level);
    curr_pos = ivec2(to_lod_pos(pass_TexCoord, level) * curr_res);
    out_FragColor = texelFetch(texture_color, curr_pos, 0);
    if (out_FragColor != vec4(vec3(0.0f), 1.0f) || out_FragColor == vec4(0.0f)) break;
  }

  out_FragColor = texture(texture_color, pass_TexCoord);
  gl_FragDepth = texelFetch(texture_depth, curr_pos, 0).r;
}
