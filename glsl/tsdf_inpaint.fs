#version 430

noperspective in vec2 pass_TexCoord;
layout(pixel_center_integer) in vec4 gl_FragCoord;

uniform sampler2D texture_color;
uniform sampler2D texture_depth;
uniform vec2 resolution_inv;
uniform int lod;

uniform uvec2[20] texture_offsets;
uniform uvec2[20] texture_resolutions;

out vec4 out_FragColor;
out float gl_FragDepth;

const int kernel_size = 4;

const float gauss_weights[16] = {
  0.4, 0.9, 0.9, 0.4,
  0.9, 1.8, 1.8, 0.9,
  0.9, 1.8, 1.8, 0.9,
  0.4, 0.9, 0.9, 0.4
};

ivec2 to_lod_pos(in vec2 pos, in int lod) {
  return ivec2(vec2(texture_offsets[lod]) + vec2(texture_resolutions[lod]) * pos); 
}

void main() {
  gl_FragDepth = 1.0f;
  // get normalized coordinates from integer fragcoord
  vec2 tex_coord = (gl_FragCoord.xy - vec2(texture_offsets[lod + 1])) / vec2(texture_resolutions[lod + 1]);
  ivec2 pos_int = ivec2(vec2(to_lod_pos(tex_coord, lod)) * vec2(2.0f / 3.0f , 1.0f));

  // out_FragColor = texelFetch(texture_color, pos_int, 0);
  // gl_FragDepth = texelFetch(texture_depth, pos_int, 0).r;
  // if (out_FragColor == vec4(0.0f, 0.0f, 0.0f, 1.0f)) {
  //   out_FragColor = vec4(1.0f);
  //   return;
  // }

  vec4 samples[kernel_size * kernel_size];
  for(int x = 0; x < kernel_size; ++x) {
    for(int y = 0; y < kernel_size; ++y) {
      const ivec2 pos_tex = pos_int + ivec2(x- kernel_size * 0.5 + 1, y- kernel_size * 0.5 + 1);
      vec4 color = texelFetch(texture_color, pos_tex, 0);
      float depth = texelFetch(texture_depth, pos_tex, 0).r;
      if (color == vec4(0.0f, 0.0f, 0.0f, -1.0f) || color == vec4(0.0f, 0.0f, 0.0f, 1.0f) || depth == 1.0) {
        color.r = -1.0f;
      }
      samples[x + y * kernel_size] = vec4(color.rgb, depth);
    }
  }

  float depth_av = 0.0f;
  int num_samples = 0;
  for(int i = 0; i < kernel_size * kernel_size; ++i) {
    if (samples[i].r >= 0.0f) {
      depth_av += samples[i].a;
        ++num_samples;
    }
  }
  if (num_samples == 0) {
    // gl_FragDepth = 1.0;
    gl_FragDepth = texelFetch(texture_depth, pos_int, 0).r;
    if (gl_FragDepth < 1.0) {
      out_FragColor = vec4(0.0f, 0.0f, 0.0f, -1.0f);
    }
    else {
      out_FragColor = vec4(0.0f, 1.0f, 0.0f, 0.0f);
    }
    return;
  }
  depth_av /= float(num_samples);
  
  vec4 total_color = vec4(0.0f);
  float total_depth = 0.0f;
  float total_weight = 0.0f;
  for(int i = 0; i < kernel_size * kernel_size; ++i) {
    if (samples[i].r >= 0.0f) {
      // if (samples[i].a >= depth_av) {
        float weight = 1.0f;
        // float weight = gauss_weights[i];
        total_color.rgb += samples[i].rgb * weight;
        total_depth += samples[i].a * weight;
        total_weight += weight;
      // }
    }
  }

  out_FragColor = total_color / total_weight;
  out_FragColor.w = 1.0f;
  gl_FragDepth = total_depth / total_weight;
  // out_FragColor = vec4(to_lod_pos(pass_TexCoord, lod), float(lod) / 9.0f, 1.0f);
}