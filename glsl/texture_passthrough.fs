#version 130

noperspective in vec2 pass_TexCoord;

uniform sampler2DArray texture_array;
uniform uint layer;

out vec4 out_FragColor;

void main() {

  vec3 coords = vec3(pass_TexCoord, layer);

  out_FragColor = vec4(texture(texture_array, coords).rgb, 1.0f);
}
