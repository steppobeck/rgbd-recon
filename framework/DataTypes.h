#ifndef KINECT_DATATYPES_H
#define KINECT_DATATYPES_H

#include <glm/gtc/type_precision.hpp>
#include <iostream>

// non-numeric byte from Microsoft GSL implementation
enum class byte : std::uint8_t{};

namespace kinect{

  class xyz{
  public:
    float x;
    float y;
    float z;
    
    operator glm::fvec3() const {
      return glm::fvec3{x,y,z};
    }
  };

  extern std::ostream& operator << (std::ostream& o, const xyz& v);
  extern xyz interpolate(const xyz& a, const xyz& b, float t);

  class uv{
  public:
    float u;
    float v;
    
    operator glm::fvec2() const {
      return glm::fvec2{u,v};
    }
  };

  extern std::ostream& operator << (std::ostream& o, const uv& v);
  extern uv interpolate(const uv& a, const uv& b, float t);

  class xyz_d{
  public:
    double x;
    double y;
    double z;
    
    operator glm::fvec3() const {
      return glm::fvec3{x,y,z};
    }
  };

  class uv_d{
  public:
    double u;
    double v;
    
    operator glm::fvec2() const {
      return glm::fvec2{u,v};
    }
  };

extern xyz operator* (const float, const xyz&);
extern uv operator* (const float, const uv&);

extern xyz operator+ (const xyz&, const xyz&);
extern uv operator+ (const uv&, const uv&);
extern uv operator- (const uv&, const uv&);

extern xyz_d operator* (const float, const xyz_d&);
extern uv_d operator* (const float, const uv_d&);

extern xyz_d operator+ (const xyz_d&, const xyz&);
extern uv_d operator+ (const uv_d&, const uv&);

xyz getTrilinear(xyz* data, unsigned width, unsigned height, unsigned depth, float x, float y, float z);
}

#endif // #ifndef KINECT_DATATYPES_H