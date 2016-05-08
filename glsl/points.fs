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
flat in vec4  pass_glpos;
// used by accumulation pass
uniform sampler2DArray kinect_colors;
uniform sampler2DArray kinect_qualities;
uniform uint layer;
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

  highp vec4 ndcPos;
  ndcPos.xy = ((2.0 * gl_FragCoord.xy)) / (viewportSizeInv) - 1.0f;
  // ndcPos.xy = ((2.0 * gl_FragCoord.xy) - (2.0 * viewport.xy)) / (viewport.zw) - 1;
  ndcPos.z = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) /
      (gl_DepthRange.far - gl_DepthRange.near);
  ndcPos.w = 1.0;

  highp vec4 clipPos = ndcPos / gl_FragCoord.w;
  highp vec4 eyePos = inverse(gl_ProjectionMatrix) * clipPos;
  // eyePos /= eyePos.w;
  highp vec4 worldPos =  inverse(gl_ModelViewMatrix) * eyePos;
// manual
  highp vec3 view =   (gl_ModelViewMatrix * vec4(pass_pos_cs, 1.0f)).xyz;
  highp vec4 clipped = gl_ProjectionMatrix * vec4(pass_pos_es, 1.0f);
  highp vec3 ndc = clipped.xyz / clipped.w;
  highp vec3 tex = ndc * 0.5f + 0.5f;
  highp vec3 frag = vec3(tex.xy / viewportSizeInv, tex.z);
  highp uvec2 fragi = ivec2(frag.xy);

  highp uvec2 pos_frag = ivec2(gl_FragCoord.xy);

  highp float quality = pass_lateral_quality/pass_depth;
// reverse
  // pointcoord origin is upper left
  highp vec2 pos_win = gl_FragCoord.xy - vec2(gl_PointCoord.x - 0.5f, -gl_PointCoord.y + 0.5f);
  highp vec3 pos_tex = vec3(pos_win * viewportSizeInv, gl_FragCoord.z);
  highp vec3 pos_ndc = pos_tex * 2.0f - 1.0f;
  highp vec4 pos_clip = vec4(pos_ndc / gl_FragCoord.w, 1.0f / gl_FragCoord.w);
  highp vec4 pos_div = projection_inv * vec4(pos_clip);
  highp vec3 pos_view = pos_div.xyz / pos_div.w;
  highp vec3 pos_world = (modelview_inv * vec4(pos_div.xyz, 1.0f)).xyz;
 
  highp vec4 position_curr = img_to_eye_curr * vec4(gl_FragCoord.xy + vec2(0.5,0.5), 0.0f, 1.0);
  highp vec3 position_curr_es = (position_curr / position_curr.w).xyz;

  highp vec4 color = texture2DArray(kinect_colors, vec3(pass_texcoord, float(layer)));
  gl_FragColor = vec4(color.rgb, quality);
  // gl_FragColor = vec4(gl_PointCoord, 0.0f, 1.0f);
  //ndc
  gl_FragColor = vec4(ndc.xyz, 1.0f);
  // gl_FragColor = vec4(pos_ndc.xyz, 1.0f);
  //clip space
  // gl_FragColor = vec4(pos_clip.xyz, 1.0f);
  // gl_FragColor = vec4(clipped.xyz, 1.0f);
  // gl_FragColor = vec4(clipPos.xyz, 1.0f);
  // view space
  // gl_FragColor = vec4(pass_pos_es.xyz, 1.0f);
  // gl_FragColor = vec4(eyePos.xyz, 1.0f);
  // gl_FragColor = vec4(pos_div.xyz, 1.0f);
  // gl_FragColor = vec4(position_curr_es.xyz, 1.0f);
  // gl_FragColor = vec4(view, 1.0f);
  // world space
  // gl_FragColor = vec4(pass_pos_cs.xyz, 1.0f);
  // gl_FragColor = vec4(worldPos.xyz, 1.0f);
  // gl_FragColor = vec4(pos_world.xyz, 1.0f);
  // gl_FragColor = vec4((inverse(gl_ModelViewMatrix)* vec4(pass_pos_es, 1.0f)).xyz, 1.0f);
  highp vec4 p = vec4(0.5f, 0.25f, -0.15f, 1.0f);
  highp vec4 p2 = gl_ModelViewMatrix * p;
  // vec4 p3 = modelview_inv * p2;
  highp vec4 p3 = inverse(gl_ModelViewMatrix) * p2;
  if(distance(pass_pos_es, pos_view) < 0.0001) {
  // if(distance(vec4(pass_pos_es, 1.0f), pos_div2) < 0.0001) {
  }
  else {
    discard;
  }
}
// distances
// automatic vs manual clip space pos: 0.000001  clipped - pass_glpos
// automatic vs manual ndc pos:        0.0000001  ndc - pass_gpos / pass_glpos.w
// manual vs reverse view pos:        6    pass_pos_es.xyz - pos_div.xyz
// manual vs reverse clip pos:        0.005    clipped.xyz - pos_clip
// manual vs reverse ndc pos:        0.0009    ndc - pos_ndc
// manual vs reverse frag pos:       0.8        frag.xy - gl_FragCoord.xy

// with correcrt pointcoord
//                                   0.0001   pass_pos_es, pos_world
//                                   0.000001  pos_view.w, 1.0f
//                                   0.00001   pos_div.w, 1.0f
// manual vs reverse view pos:       0.0001    pass_pos_es.xyz, pos_view
// manual vs reverse clip pos:       0.0001    clipped, pos_clip
// manual vs reverse ndc pos:        0.00001    ndc, pos_ndc
// manual vs reverse frag pos:       0.01        frag.xy, pos_win