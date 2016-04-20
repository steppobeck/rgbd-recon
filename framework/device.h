#if !defined(SENSOR_DEVICE_H)

#define SENSOR_DEVICE_H

// includes, system

#include <Matrix.h>
#include <vector>

// includes, project

#include <timevalue.h>

namespace boost{
  class thread;
  class mutex;
}

// types, exported (class, enum, struct, union, typedef)

namespace sensor {

class device {

public:

  struct station {

    static const unsigned short max_buttons = 32;
    
    size_t          frame;
    timevalue timestamp;
    gloost::Matrix     matrix;
    bool            button[max_buttons];
    boost::mutex*   mutex;
    
    station();
    ~station();
    
  };
  
  device();
  virtual ~device();

  bool start();
  bool stop();

  const station& getStation(size_t /* station_id */) const;
  
protected:

  virtual bool startDevice() =0;
  virtual bool stopDevice() =0;
  virtual void readLoop() =0;

  typedef std::vector<station> station_list_type;

  station_list_type _station_list;
  bool              _running;
  boost::thread*    _readthread;
  
};

} // end namespace sensor

// variables, exported (extern)

// functions, inlined (inline)
  
// functions, exported (extern)
  
#endif // #if !defined(SENSOR_DEVICE_H)


