#version 130

noperspective in vec2 pass_TexCoord;

uniform sampler2D texture_handle;
uniform sampler2DArray texture_array;
uniform uint layer;
uniform ivec2 resolution;
uniform uint mode;
uniform int lod;

out vec4 out_FragColor;


const float zNear = 0.1;
const float zFar = 200.0;
float linearize(in float depth) {
  // bring between -1 and 1
  depth = 2.0 * depth - 1.0;
  return 2.0 * zNear * zFar / (zFar + zNear - depth * (zFar - zNear));
}

vec3 heatmap(float norm_val) {
  vec3 color = vec3(0.0);
  const float third = 1 / 3.0;
  if(norm_val <= third) {
    color.g = 1.0;
    color.r = max(0.0, (third - norm_val) * 3.0);
  }
  else if(norm_val <= third * 2.0) { 
    norm_val -= third;
    color.b = max(0.0, norm_val * 3.0);
    color.g = max(0.0, (third - norm_val) * 3.0);
  }
  else {
    norm_val -= 2.0 * third;
    color.r = max(0.0, norm_val * 3.0);
    color.b = max(0.0, (third - norm_val) * 3.0);
  }

  return color;
}

void main() {

  if (mode == 0u) {
    out_FragColor = vec4(texture(texture_handle, pass_TexCoord).rgb, 1.0);
    // out_FragColor.r = linearize(out_FragColor.r);
    // out_FragColor.rgb = heatmap(out_FragColor.r );
  }
  else if (mode == 1u){
    vec3 coords = vec3(pass_TexCoord, layer);
    out_FragColor = vec4(texture(texture_array, coords).rgb, 1.0);
  }
  else {
    out_FragColor = vec4(texelFetch(texture_handle, ivec2(pass_TexCoord * resolution), lod).rgb, 1.0);
  }
}
