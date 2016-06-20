#version 430

in vec3 in_Position; 

uniform sampler2D texture_color;
uniform uvec2 resolution_tex;
uniform int num_lods;
layout(rgba32f) uniform image2D out_tex;

vec4 out_FragColor = vec4(0.0f);
// out float gl_FragDepth;

void main() {
  int level = 0;
  ivec2 curr_res = ivec2(0);
  for (; level < num_lods; ++level) {
    curr_res = textureSize(texture_color, level);
    // out_FragColor = textureLod(texture_color, in_Position.xy, level);
    out_FragColor = texelFetch(texture_color, ivec2(in_Position.xy * curr_res), level);
    if (out_FragColor != vec4(vec3(0.0f), 1.0f) || out_FragColor == vec4(0.0f)) break;
  }

  ivec2 ipos_tex = ivec2(in_Position.xy * resolution_tex);
  imageStore(out_tex, ipos_tex, vec4(level / float(num_lods), vec2(ipos_tex), 1.0f));
  imageStore(out_tex, ipos_tex, out_FragColor);
}
