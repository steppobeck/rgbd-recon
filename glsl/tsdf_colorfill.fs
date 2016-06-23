#version 130

noperspective in vec2 pass_TexCoord;

uniform sampler2D texture_color;
uniform sampler2D texture_depth;
uniform uvec2 resolution_tex;
uniform int num_lods;

uniform uvec2[20] texture_offsets;
uniform uvec2[20] texture_resolutions;

out vec4 out_FragColor;
out float gl_FragDepth;

ivec2 to_lod_pos(in vec2 pos, in int lod) {
  return ivec2(ivec2(texture_offsets[lod]) + vec2(ivec2(texture_resolutions[lod]) - 1) * pos); 
}
vec2 to_lod_pos2(in vec2 pos, in int lod) {
  return clamp(vec2(texture_offsets[lod] ) + vec2(ivec2(texture_resolutions[lod]) - 1) * pos, vec2(texture_offsets[lod]), vec2(ivec2(texture_offsets[lod] + texture_resolutions[lod]) - 1)); 
}

void main() {
  int level = 0;
  ivec2 curr_res = textureSize(texture_color, level);

  ivec2 curr_pos = ivec2(0);
  for (; level < num_lods; ++level) {
    curr_pos = to_lod_pos(pass_TexCoord, level);
    curr_pos = curr_pos;
    out_FragColor = texelFetch(texture_color, curr_pos, 0);
    if ((out_FragColor != vec4(vec3(0.0f), -1.0f) && out_FragColor != vec4(0.0f, 0.0f, 0.0f, 1.0f)) || out_FragColor == vec4(0.0f) ) break;
  }
  if (level == 0) {
    out_FragColor = texture(texture_color, to_lod_pos2(pass_TexCoord, level) / vec2(curr_res));
  }
  else {
    vec2 p2 =to_lod_pos2(pass_TexCoord, level + 2) / vec2(curr_res);
    vec2 p1 =to_lod_pos2(pass_TexCoord, level + 1) / vec2(curr_res);
    vec4 color_1 = texture(texture_color, p1);
    vec4 color_2 = texture(texture_color, p2);
    float weight_1 = distance(pass_TexCoord, floor(pass_TexCoord));
    float weight_2 = 1.0f - weight_1;
    // float weight_1 = color_1.a +1.0f;
    // float weight_2 = color_2.a +1.0f;
    out_FragColor = (color_1 * weight_1 + color_2 * weight_2) / (weight_1 + weight_2);
  }
    // out_FragColor = vec4(float(level) / 8.0f);
  gl_FragDepth = texelFetch(texture_depth, curr_pos, 0).r;
}
