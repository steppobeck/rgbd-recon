#version 130

noperspective in vec2 pass_TexCoord;

uniform sampler2D texture_handle;
uniform sampler2DArray texture_array;
uniform uint layer;
uniform ivec2 resolution;
uniform uint mode;
uniform int lod;

out vec4 out_FragColor;

void main() {

  if (mode == 0u) {
    out_FragColor = vec4(texture(texture_handle, pass_TexCoord).rgb, 1.0f);
    // out_FragColor = vec4(0.5f);
  }
  else if (mode == 1u){
    vec3 coords = vec3(pass_TexCoord, layer);
    out_FragColor = vec4(texture(texture_array, coords).rgb, 1.0f);
  }
  else {
    out_FragColor = vec4(texelFetch(texture_handle, ivec2(pass_TexCoord * resolution), lod).rgb, 1.0f);
  }
}
