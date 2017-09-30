#version 430

noperspective in vec2 pass_TexCoord;
layout(pixel_center_integer) in vec4 gl_FragCoord;

uniform sampler2D texture_color;
uniform sampler2D texture_depth;
uniform vec2 resolution_inv;
uniform int num_lods;

uniform uvec2[20] texture_offsets;
uniform uvec2[20] texture_resolutions;

// currently does no apply here and does not work for SIDE by SIDE stereo
uniform vec2 viewport_offset;


out vec4 out_FragColor;
out float gl_FragDepth;
// receives fragcoord from 0,0 to width-1, heigh-1 -> dont reduce resolution
ivec2 to_lod_pos(in vec2 pos, in int lod) {
  return ivec2(ivec2(texture_offsets[lod]) + vec2(ivec2(texture_resolutions[lod])) * pos); 
}
// receives texcoord -> dont reduce resolution
vec2 to_lod_pos2(in vec2 pos, in int lod) {
  // clamp to prevent interpolating beyond mipmap border
  return clamp(vec2(texture_offsets[lod]) + vec2(texture_resolutions[lod]) * pos, vec2(texture_offsets[lod]) + 0.5, vec2(texture_offsets[lod] + texture_resolutions[lod]) - 0.5); 
}

void main() {
  // get normalized coordinates from integer fragcoord
  vec2 tex_coord = gl_FragCoord.xy / vec2(texture_resolutions[0]);

  int level = 0;
  ivec2 curr_pos = ivec2(0);
  for (; level < num_lods; ++level) {
    curr_pos = to_lod_pos(tex_coord, level);
    out_FragColor = texelFetch(texture_color, curr_pos, 0);
    if (out_FragColor.a > 0.0) break;
  }

  if (level > 0) {
    vec2 p2 =to_lod_pos2(pass_TexCoord, level + 2) * resolution_inv;
    vec2 p1 =to_lod_pos2(pass_TexCoord, level + 1) * resolution_inv;
    vec4 color_1 = texture(texture_color, p1);
    vec4 color_2 = texture(texture_color, p2);
    float weight_1 = distance(pass_TexCoord, floor(pass_TexCoord));
    float weight_2 = 1.0f - weight_1;
    // float weight_1 = color_1.a +1.0f;
    // float weight_2 = color_2.a +1.0f;
    out_FragColor = (color_1 * weight_1 + color_2 * weight_2) / (weight_1 + weight_2);
  }
    // out_FragColor = vec4(float(level) / 8.0f);
  gl_FragDepth = texelFetch(texture_depth, to_lod_pos(tex_coord, 0), 0).r;
}
