#if !defined(SENSOR_DEVICEMANAGER_H)

#define SENSOR_DEVICEMANAGER_H

// includes, system

#include <map>
#include <string>

// includes, project

#include <device.h>

namespace kinect{
  class ChronoMeter;
}

namespace sensor {

  class device_dtrack;


// types, exported (class, enum, struct, union, typedef)

class devicemanager {

protected:
  
  devicemanager();
  ~devicemanager();

public:

  static devicemanager* const the();
  static void shutdown();


  device* get_dtrack(int /*port*/, timevalue /* timeout */, const char* filename = 0, unsigned* id = 0, kinect::ChronoMeter* cm = 0);


private:

  static devicemanager* _me;

  std::map<int, device* > _dtracks;


};

} // end namespace sensor
// variables, exported (extern)

// functions, inlined (inline)
  
// functions, exported (extern)
  
#endif // #if !defined(SENSOR_DEVICEMANAGER_H)

// $Id: devicemanager.h,v 1.3 2008/02/04 16:00:16 beck3 Exp $
