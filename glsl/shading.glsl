#ifndef SHADING_GLSL
#define SHADING_GLSL
#extension GL_ARB_shading_language_420pack : enable
// light
const vec3 LightPosition = vec3(1.5f, 1.0f, 1.0f);
const vec3 LightDiffuse = vec3(1.0f, 0.9f, 0.7f);
const vec3 LightAmbient = LightDiffuse * 0.2f;
const vec3 LightSpecular = vec3(1.0f);

// material
const float ks = 0.5f;            // specular intensity
const float n = 20.0f;            //specular exponent 

layout (std140, binding = 1) uniform Settings {
  uint g_shade_mode;
};
// 0 = color
// 1 = shaded
// 2 = normal
// 3 = camera influence

const vec3 solid_diffuse = vec3(0.5f);

const vec3 camera_colors[5] = vec3[5](
  vec3(228,26,28) / 255.0f,
  vec3(55,126,184) / 255.0f,
  vec3(77,175,74) / 255.0f,
  vec3(152,78,163) / 255.0f,
  vec3(255,127,0) / 255.0f
);
// phong diss and spec coefficient calculation in viewspace
vec2 phongDiffSpec(const vec3 position, const vec3 normal, const float n, const vec3 lightPos) {
  vec3 toLight = normalize(lightPos - position);
  float lightAngle = dot(normal, toLight);
  // if fragment is not directly lit, use only ambient light
  if (lightAngle <= 0.0f) {
    return vec2(0.0f);
  }

  float diffContribution = max(lightAngle, 0.0f);

  vec3 toViewer = normalize(-position);
  vec3 halfwayVector = normalize(toLight + toViewer);
  float reflectedAngle = dot(halfwayVector, normal);
  float specLight = pow(reflectedAngle, n);

  // fade out specular hightlights towards edge of lit region
  float a = (1.0f - lightAngle) * ( 1.0f - lightAngle);
  specLight *= 1.0f - a * a * a;

  return vec2(diffContribution, specLight);
}
// shade with included light and material info
vec3 shade(const in vec3 view_pos, const in vec3 view_normal, const in vec3 diffuseColor) {
  if (g_shade_mode == 0u) { 
    return diffuseColor;
  }
  else if(g_shade_mode == 1u){
    // shadow paramater
    vec2 diffSpec = phongDiffSpec(view_pos, view_normal, n, LightPosition);
    return vec3(LightAmbient * solid_diffuse 
              + LightDiffuse * solid_diffuse * diffSpec.x
              + LightSpecular * ks * diffSpec.y);
  }
  else if (g_shade_mode == 2u) {
    return (inverse(gl_NormalMatrix) * vec4(view_normal, 0.0f)).xyz;
  }
  else return vec3(1.0f);
}
#endif