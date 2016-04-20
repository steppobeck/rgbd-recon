#if !defined(SENSOR_DEVICE_DTRACK_H)

#define SENSOR_DEVICE_DTRACK_H

// includes, system
// #include <>

// includes, project
#include <device.h>
#include <timevalue.h>

namespace kinect{
  class ChronoMeter;
}
class DTrack;

namespace sensor {

// types, exported (class, enum, struct, union, typedef)
class device_dtrack : public device {
  
public:
    static const char* filename;
    static unsigned*   id;
    static kinect::ChronoMeter* cm;
  device_dtrack(size_t /* port */, timevalue /* timeout */ = timevalue::const_999_ms);
  /*virtual*/ ~device_dtrack();

protected:
  
  /*virtual*/ bool startDevice();
  /*virtual*/ bool stopDevice();
  /*virtual*/ void readLoop();

  
  DTrack*         _dtrack;
  size_t          _port;
  timevalue       _timeout;
  
};

} // end namespace sensor
// variables, exported (extern)

// functions, inlined (inline)
  
// functions, exported (extern)
  
#endif // #if !defined(SENSOR_DEVICE_DTRACK_H)

// $Id: device_dtrack.h,v 1.3 2006/10/27 17:15:08 beck3 Exp $
