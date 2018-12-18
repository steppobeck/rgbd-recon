#include "DataTypes.h"


namespace kinect{

  uv interpolate(const uv& a, const uv& b, float t){
    uv r;
    r.u = (1.0f - t)*a.u + t*b.u;
    r.v = (1.0f - t)*a.v + t*b.v;
    return r;
  }

  xyz interpolate(const xyz& a, const xyz& b, float t){
    xyz r;
    r.x = (1.0f - t)*a.x + t*b.x;
    r.y = (1.0f - t)*a.y + t*b.y;
    r.z = (1.0f - t)*a.z + t*b.z;
    return r;
  }
  /*extern*/
  xyz
  operator* (const float v, const xyz& b){
    xyz res;
    res.x = v * b.x;
    res.y = v * b.y;
    res.z = v * b.z;
    return res;
  }
  /*extern*/
  uv
  operator* (const float v, const uv& b){
    uv res;
    res.u = v * b.u;
    res.v = v * b.v;
    return res;
  }

  /*extern*/
  xyz
  operator+ (const xyz& a, const xyz& b){
    xyz res;
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    res.z = a.z + b.z;
    return res;
  }

  std::ostream& operator << (std::ostream& o, const xyz& v){
    o << "(" << v.x << "," << v.y << "," << v.z << ")";
    return o;
  }

  /*extern*/
  uv
  operator+ (const uv& a, const uv& b){
    uv res;
    res.u = a.u + b.u;
    res.v = a.v + b.v;
    return res;
  }

  /*extern*/
  uv
  operator- (const uv& a, const uv& b){
    uv res;
    res.u = a.u - b.u;
    res.v = a.v - b.v;
    return res;
  }

  std::ostream& operator << (std::ostream& o, const uv& v){
    o << "(" << v.u << "," << v.v << ")";
    return o;
  }

  /*extern*/
  xyz_d
  operator* (const float v, const xyz_d& b){
    xyz_d res;
    res.x = v * b.x;
    res.y = v * b.y;
    res.z = v * b.z;
    return res;
  }

  /*extern*/
  uv_d
  operator* (const float v, const uv_d& b){
    uv_d res;
    res.u = v * b.u;
    res.v = v * b.v;
    return res;
  }

  /*extern*/
  xyz_d
  operator+ (const xyz_d& a, const xyz& b){
    xyz_d res;
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    res.z = a.z + b.z;
    return res;
  }

  /*extern*/
  uv_d
  operator+ (const uv_d& a, const uv& b){
    uv_d res;
    res.u = a.u + b.u;
    res.v = a.v + b.v;
    return res;
  }


  xyz getTrilinear(xyz* data, unsigned width, unsigned height, unsigned depth, float x, float y, float z){

    // calculate weights and boundaries along x direction
    unsigned xa = std::floor(x);
    unsigned xb = std::ceil(x);
    float w_xb = x - xa;
    float w_xa = 1.0 - w_xb;

    // calculate weights and boundaries along y direction
    unsigned ya = std::floor(y);
    unsigned yb = std::ceil(y);
    float w_yb = y - ya;
    float w_ya = 1.0 - w_yb;

    // calculate weights and boundaries along z direction
    unsigned za = std::floor(z);
    unsigned zb = std::ceil(z);
    float w_zb = z - za;
    float w_za = 1.0 - w_zb;

    // calculate indices to access data
    const unsigned idmax = width * height * depth;
    unsigned id000 = std::min( za * width * height + ya * width + xa  , idmax);
    unsigned id100 = std::min( za * width * height + ya * width + xb  , idmax);
    unsigned id110 = std::min( za * width * height + yb * width + xb  , idmax);
    unsigned id010 = std::min( za * width * height + yb * width + xa  , idmax);

    unsigned id001 = std::min( zb * width * height + ya * width + xa  , idmax);
    unsigned id101 = std::min( zb * width * height + ya * width + xb  , idmax);
    unsigned id111 = std::min( zb * width * height + yb * width + xb  , idmax);
    unsigned id011 = std::min( zb * width * height + yb * width + xa  , idmax);



    // 1. interpolate between x direction: 4 times;
    xyz   tmp_000_100 = w_xa * data[id000] + w_xb * data[id100];
    xyz   tmp_010_110 = w_xa * data[id010] + w_xb * data[id110];
    xyz   tmp_001_101 = w_xa * data[id001] + w_xb * data[id101];
    xyz   tmp_011_111 = w_xa * data[id011] + w_xb * data[id111];

    // 2. interpolate between y direction: 2 times;

    xyz   tmp_A = w_ya * tmp_000_100 + w_yb * tmp_010_110;
    xyz   tmp_B = w_ya * tmp_001_101 + w_yb * tmp_011_111;

    xyz result = w_za * tmp_A + w_zb * tmp_B;

    return result;
  }

}