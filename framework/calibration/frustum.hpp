#ifndef FRUSTUM_HPP
#define FRUSTUM_HPP

#include <glm/gtc/type_precision.hpp>
#include <array>

namespace kinect{

class Frustum{

public:
  Frustum(std::array<glm::fvec3, 8> const& corners);
  
  void draw() const;

  glm::fvec3 getCameraPos() const;
  bool inside(glm::fvec3 const& point) const;

private:
  std::array<glm::fvec3, 8> m_corners;
  std::array<glm::fvec4, 6> m_planes;
};

}

#endif // #ifndef FRUSTUM_HPP