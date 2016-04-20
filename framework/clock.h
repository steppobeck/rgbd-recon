#if !defined(SENSOR_CLOCK_H)

#define SENSOR_CLOCK_H

// includes, system

#include <boost/noncopyable.hpp> // boost::noncopyable
#include <string>                // std::string

// includes, project

#include <timevalue.h>

// exported namespace

namespace sensor {
  
  // types, exported (class, enum, struct, union, typedef)
  
  class clock : private boost::noncopyable {
    
  public:

    static const timevalue   resolution;
    static const std::string name;
    
    static timevalue time();        // wraps ::clock_gettime
    static timevalue time_of_day(); // wraps ::gettimeofday

  };
  
  // variables, exported (extern)
  
  // functions, inlined (inline)
  
  // functions, exported (extern)
  
  timevalue current_time();
  timevalue current_time_of_day();

} // namespace sensor {

#endif // #if !defined(SENSOR_CLOCK_H)

// $Id: clock.h,v 1.2 2006/11/16 01:46:05 springer Exp $
