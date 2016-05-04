#version 150
#extension GL_ARB_gpu_shader5 : enable
#extension GL_EXT_texture_array : enable

///////////////////////////////////////////////////////////////////////////////
// layout(pixel_center_integer) in vec4 gl_FragCoord;
flat in vec2  pass_texcoord;
flat in vec3  pass_pos_es;
flat in vec3  pass_pos_cs;
flat in float pass_depth;
flat in float pass_lateral_quality;
flat in vec3  normal_es;
// used by accumulation pass
uniform sampler2DArray kinect_colors;
uniform sampler2DArray kinect_qualities;
uniform int layer;
uniform mat4 gl_ProjectionMatrix;
uniform mat4 gl_ModelViewMatrix;

uniform mat4 img_to_eye_curr;
uniform mat4 projection_inv;
uniform mat4 modelview_inv;
uniform vec2 viewportSizeInv;
uniform vec2 offset;
uniform float epsilon;

out vec4 gl_FragColor;
///////////////////////////////////////////////////////////////////////////////
// main
///////////////////////////////////////////////////////////////////////////////
void main() {
  // to cull away borders of the rgb camera view
  if(pass_texcoord.s > 0.99 || pass_texcoord.s < 0.01 ||
    pass_texcoord.t > 0.99 || pass_texcoord.t < 0.01) {
      discard;
  }

  vec4 ndcPos;
  ndcPos.xy = ((2.0 * gl_FragCoord.xy)) / (viewportSizeInv) - 1.0f;
  // ndcPos.xy = ((2.0 * gl_FragCoord.xy) - (2.0 * viewport.xy)) / (viewport.zw) - 1;
  ndcPos.z = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) /
      (gl_DepthRange.far - gl_DepthRange.near);
  ndcPos.w = 1.0;

  vec4 clipPos = ndcPos / gl_FragCoord.w;
  vec4 eyePos = inverse(gl_ProjectionMatrix) * clipPos;
  // eyePos /= eyePos.w;
  vec4 worldPos =  inverse(gl_ModelViewMatrix) * eyePos;


  vec4 clipped = gl_ProjectionMatrix * vec4(pass_pos_es, 1.0f);
  vec3 ndc = clipped.xyz / clipped.w;
  vec3 tex = ndc * 0.5f + 0.5f;
  vec3 frag = vec3(tex.xy / viewportSizeInv, tex.z);
  uvec2 fragi = ivec2(frag.xy);

  uvec2 pos_frag = ivec2(gl_FragCoord.xy);

  float quality = pass_lateral_quality/pass_depth;
  // ndc coords are correct
  vec3  pos_tex = vec3(gl_FragCoord.xy * viewportSizeInv, gl_FragCoord.z);
  vec3  pos_ndc = pos_tex * 2.0f - 1.0f;
  vec3  pos_clip = pos_ndc / gl_FragCoord.q;
  vec4  pos_div = projection_inv * vec4(pos_clip, 1.0);
  vec4  pos_world = inverse(gl_ModelViewMatrix) * vec4(pos_div.xyz, 1.0f);

  vec4  position_curr = img_to_eye_curr * vec4(gl_FragCoord.xy + vec2(0.5,0.5), 0.0f, 1.0);
  vec3  position_curr_es = (position_curr / position_curr.w).xyz;

  vec4 color = texture2DArray(kinect_colors, vec3(pass_texcoord, float(layer)));
  gl_FragColor = vec4(color.rgb, quality);
  //ndc
  // gl_FragColor = vec4(ndc.xyz, 1.0f);
  // gl_FragColor = vec4(pos_ndc.xyz, 1.0f);
  //clip space
  // gl_FragColor = vec4(pos_clip.xyz, 1.0f);
  // gl_FragColor = vec4(clipped.xyz, 1.0f);
  // gl_FragColor = vec4(clipPos.xyz, 1.0f);
  // view space
  // gl_FragColor = vec4(pass_pos_es.xyz, 1.0f);
  // gl_FragColor = vec4(eyePos.xyz, 1.0f);
  gl_FragColor = vec4(pos_div.xyz, 1.0f);
  // gl_FragColor = vec4(position_curr_es.xyz, 1.0f);
  // gl_FragColor = vec4((gl_ModelViewMatrix * vec4(pass_pos_cs, 1.0f)).xyz, 1.0f);
  // world space
  // gl_FragColor = vec4(pass_pos_cs.xyz, 1.0f);
  // gl_FragColor = vec4(world_pos.xyz, 1.0f);
  // gl_FragColor = vec4(pos_world.xyz, 1.0f);
  // gl_FragColor = vec4((inverse(gl_ModelViewMatrix)* vec4(pass_pos_es, 1.0f)).xyz, 1.0f);
  vec4 p = vec4(0.5f, 0.25f, -0.15f, 1.0f);
  vec4 p2 = gl_ModelViewMatrix * p;
  // vec4 p3 = modelview_inv * p2;
  vec4 p3 = inverse(gl_ModelViewMatrix) * p2;
  if(length((frag.xy - (gl_FragCoord.xy))) < 0.04) {

  }
  else {
    // discard;
  }
}