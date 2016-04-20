#include "clock.h"

// includes, system

#include <boost/cast.hpp> // boost::numeric_cast
#include <cerrno>         // errno
#include <ctime>          // clock_*
#include <iostream>       // std::cerr, std::endl
#include <sys/time.h>     // ::gettimeofday
#include <string.h>       // std::strerror

// includes, project

//#include <>

// internal unnamed namespace

namespace {

  // types, internal (class, enum, struct, union, typedef)

  // variables, internal

  const int clock_type_list[] = {
    //CLOCK_REALTIME,
    CLOCK_MONOTONIC,
    //CLOCK_PROCESS_CPUTIME_ID,
    //CLOCK_THREAD_CPUTIME_ID
  };
  
  const std::string clock_type_name_list[] = {
    //"CLOCK_REALTIME",
    "CLOCK_MONOTONIC",
    //"CLOCK_PROCESS_CPUTIME_ID",
    //"CLOCK_THREAD_CPUTIME_ID"
  };
  
  const std::size_t clock_type_max = sizeof(clock_type_list)/sizeof(clock_type_list[0]);
  
  std::string       clock_type_name = "CLOCK_REALTIME";
  ::clockid_t       clock_type = CLOCK_REALTIME;
  struct ::timespec clock_process_initial = {0, 0};
  
  // functions, internal

  std::size_t
  get_clock_resolution()
  {
    struct ::timespec min_resolution = {0, 999999999};
    
    for (std::size_t i = 0; i < clock_type_max; ++i) {
      struct ::timespec resolution = {0, 0};
      
      if (0 != ::clock_getres(clock_type_list[i], &resolution)) {
        std::cerr << "<unnamed>::get_clock_resolution: "
                  << "unable to determine clock resolution for '"
                  << clock_type_name_list[i]
                  << "' (errno: '" << strerror(errno) << "' " << errno << ")"
                  << std::endl;
        
        resolution.tv_nsec = 999999999;
      } else if (resolution.tv_nsec < min_resolution.tv_nsec) {
        min_resolution.tv_nsec = resolution.tv_nsec;
        clock_type = clock_type_list[i];
        clock_type_name = clock_type_name_list[i];

        ::clock_gettime(clock_type, &clock_process_initial);
      }
    }
    
    return boost::numeric_cast<int>(min_resolution.tv_nsec);
  }
  
} // namespace {

// exported namespace

namespace sensor {
  
  // variables, exported
  
  const timevalue   clock::resolution(0, get_clock_resolution());
  const std::string clock::name(clock_type_name);
  
  // functions, exported
  
  timevalue
  clock::time()
  {
    struct ::timespec now = {0, 0};
    
    ::clock_gettime(clock_type, &now);
    
    return timevalue(now.tv_sec - clock_process_initial.tv_sec, now.tv_nsec);
  }
  
  timevalue
  clock::time_of_day()
  {
    struct ::timeval now = {0, 0};
    
    ::gettimeofday(&now, 0);
    
    return timevalue(now.tv_sec, now.tv_usec * 1000);
  }

  timevalue
  current_time()
  { 
    return clock::time();
  }
  
  timevalue
  current_time_of_day()
  {
    return clock::time_of_day();
  }
  
} // namespace sensor {

