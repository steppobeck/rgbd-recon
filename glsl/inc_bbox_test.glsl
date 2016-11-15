#ifndef BBOX_GLSL
#define BBOX_GLSL
#extension GL_ARB_shading_language_420pack : enable
#extension GL_ARB_uniform_buffer_object : enable

layout (std140, binding = 2) uniform BBox {
  vec3 bbox_min;
  vec3 bbox_max;
};

bool in_bbox(vec3 p){
  if(p.x >= bbox_min.x &&
     p.y >= bbox_min.y &&
     p.z >= bbox_min.z &&
     p.x <= bbox_max.x &&
     p.y <= bbox_max.y &&
     p.z <= bbox_max.z){
    return true;
  }
  return false;
}

#endif