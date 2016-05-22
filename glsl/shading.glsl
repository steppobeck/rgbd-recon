#ifndef SHADING_GLSL
#define SHADING_GLSL

// light
const vec3 LightPosition = vec3(1.5f, 1.0f, 1.0f);
const vec3 LightDiffuse = vec3(1.0f, 0.9f, 0.7f);
const vec3 LightAmbient = LightDiffuse * 0.2f;
const vec3 LightSpecular = vec3(1.0f);

// material
const float ks = 0.5f;            // specular intensity
const float n = 20.0f;            //specular exponent 

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
    vec2 diffSpec = phongDiffSpec(view_pos, view_normal, n, LightPosition);
    return vec3(LightAmbient * diffuseColor 
              + LightDiffuse * diffuseColor * diffSpec.x
              + LightSpecular * ks * diffSpec.y);
}
#endif