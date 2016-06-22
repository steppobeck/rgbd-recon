#version 430
#extension GL_EXT_shader_texture_lod : enable
noperspective in vec2 pass_TexCoord;

uniform sampler2D texture_color;
uniform sampler2D texture_depth;
uniform uvec2 resolution_tex;
uniform int lod;
out vec4 out_FragColor;
out float gl_FragDepth;

const int kernel_size = 4;

const float gauss_weights[16] = {
  0.4, 0.9, 0.9, 0.4,
  0.9, 1.8, 1.8, 0.9,
  0.9, 1.8, 1.8, 0.9,
  0.4, 0.9, 0.9, 0.4
};

vec2 to_lod_pos(in vec2 pos, in int lod) {
  if(lod == 0) {
    pos.x *= 2.0f / 3.0f;
  } 
  else {
    float fraction = 1.0f / pow(2.0f, float(lod));
    pos.x = 2.0f / 3.0f + pos.x * fraction * 2.0f / 3.0f;
    pos.y = fraction + pos.y * fraction;
  }
  return pos;
}

void main() {
  ivec2 curr_res = textureSize(texture_color, 0);
  curr_res.x = int(curr_res.x * 2.0f / 3.0f);

  vec4 samples[kernel_size * kernel_size];
  for(int x = 0; x < kernel_size; ++x) {
    for(int y = 0; y < kernel_size; ++y) {
      const vec2 pos_tex = to_lod_pos(pass_TexCoord, lod) * vec2(curr_res) + vec2(x-4/2+1, y-4/2+1);
      vec4 color = texelFetch(texture_color, ivec2(pos_tex), 0);
      if (color == vec4(0.0f, 0.0f, 0.0f, 1.0f)) {
        color.r = -1.0f;
      }
      float depth = texelFetch(texture_depth, ivec2(pos_tex), 0).r;
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
  depth_av /= float(num_samples);
  
  vec4 total_color = vec4(0.0f);
  float total_depth = 0.0f;
  float total_weight = 0.0f;
  for(int i = 0; i < kernel_size * kernel_size; ++i) {
    if (samples[i].r >= 0.0f) {
      if (samples[i].a > depth_av) {
        float weight = 1.0f;
        // float weight = gauss_weights[i];
        total_color.rgb += samples[i].rgb * weight;
        total_depth += samples[i].a * weight;
        total_weight += weight;
      }
    }
  }
  out_FragColor = total_color / total_weight;
  out_FragColor.w = 1.0f;
  if(total_weight <= 0.0f) out_FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  // out_FragColor = vec4(to_lod_pos(pass_TexCoord, lod), float(lod) / 9.0f, 1.0f);
  gl_FragDepth = total_depth / total_weight;
}