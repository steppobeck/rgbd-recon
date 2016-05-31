#ifndef COLOR_GLSL
#define COLOR_GLSL
const vec3 white_reference = vec3(95.047, 100.000, 108.883);
const float epsilon = 0.008856f;
const float kappa   = 903.3f;

float pivot_RGB(float n) {
  return (n > 0.04045 ? pow((n + 0.055) / 1.055, 2.4) : n / 12.92) * 100.0;
}

vec3 rgb_to_xyz(vec3 rgbCol) {

  float r = pivot_RGB(rgbCol[0] / 255.0f);
  float g = pivot_RGB(rgbCol[1] / 255.0f);
  float b = pivot_RGB(rgbCol[2] / 255.0f);

  vec3 xyz_col;

  xyz_col[0] = r * 0.4124 + g * 0.3576 + b * 0.1805;
  xyz_col[1] = r * 0.2126 + g * 0.7152 + b * 0.0722;
  xyz_col[2] = r * 0.0193 + g * 0.1192 + b * 0.9505;
  
  return xyz_col; 
}

float pivot_XYZ(float n) {
        return n > epsilon ? pow(n, 1.0/3.0) : (kappa * n + 16) / 116;
}

vec3 xyz_to_lab(vec3 xyzCol) {
  float x = pivot_XYZ(xyzCol[0] / white_reference[0]);
  float y = pivot_XYZ(xyzCol[1] / white_reference[1]);
  float z = pivot_XYZ(xyzCol[2] / white_reference[2]);

  vec3 lab_col;
  
  lab_col[0] = max(0.0, 116*y -16);
  lab_col[1] = 500 * (x - y);
  lab_col[2] = 200 * (y - z);

  return lab_col;
}

vec3 rgb_to_lab(vec3 rgb) {
  return xyz_to_lab(rgb_to_xyz(rgb));
}

#endif