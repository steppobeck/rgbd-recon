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



  CandidateSample::CandidateSample(const float w, const xyz& p_off, const uv& t_off):
    weight(w),
    pos_off(),
    tex_off()
  {
    pos_off = p_off;
    tex_off = t_off;
  }

  CandidateSample::~CandidateSample()
  {}


  /*extern*/
  bool
  operator < (const CandidateSample& a, const CandidateSample& b){
    return a.weight > b.weight;
  }






  /* extern */
  std::ostream&
  operator<< (std::ostream& os, const samplePoint& a)
  {
    os << "samplePoint( " << std::fixed 
       << " depth " << a.depth
       << " tex_color.u " << a.tex_color.u
       << " tex_color.v " << a.tex_color.v
       << " tex_depth.u " << a.tex_depth.u
       << " tex_depth.v " << a.tex_depth.v
       << " pos_offset.x " << a.pos_offset.x
       << " pos_offset.y " << a.pos_offset.y
       << " pos_offset.z " << a.pos_offset.z
       << " tex_offset.u " << a.tex_offset.u
       << " tex_offset.v " << a.tex_offset.v
       << " pos_real " << a.pos_real
       << " quality "  << a.quality
       << ")";


    return os;
  }

#if 0
   float depth;
   uv tex_color;
   uv tex_depth;
   
   xyz pos_offset;
   uv tex_offset;
   gloost::Point3 pos_real;
#endif


}
