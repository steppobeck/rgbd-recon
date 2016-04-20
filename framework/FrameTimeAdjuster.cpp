#include "FrameTimeAdjuster.h"

#include <clock.h>

#include <iostream>

namespace kinect{


  FrameTimeAdjuster::FrameTimeAdjuster()
    : m_ts_old(sensor::clock::time()),
      m_ts_old_frame(sensor::clock::time()),
      m_framenr(0)
  {}
  
  
  FrameTimeAdjuster::~FrameTimeAdjuster()
  {}
  
  bool
  FrameTimeAdjuster::putTime(const sensor::timevalue& ts){
    ++m_framenr;
    //return false;
    //std::cerr  << "received frame: " << ts << std::endl;  
    sensor::timevalue ts_now(sensor::clock::time());

    sensor::timevalue ts_elapsed(ts_now - m_ts_old);
    m_ts_old = ts_now;
    long long elapsed = ts_elapsed.nsec();
    //std::cerr  << "elapsed nsec: " << elapsed << std::endl;

    sensor::timevalue ts_frame(ts - m_ts_old_frame);
    m_ts_old_frame = ts;
    long long frame = ts_frame.nsec();
    //std::cerr  << "frame nsec: " << frame << std::endl;


    double thresh( 0.2 * frame);
    if( (frame - elapsed) > thresh){ // we are too fast so we wait
      //std::cerr  << "sleeping frame ---------------------- " << std::endl;
      long long rest_sleep_ns = (frame - elapsed);
      if(0 < rest_sleep_ns && m_framenr > 2){
	sensor::timevalue rest_sleep(0,rest_sleep_ns);
	nanosleep(rest_sleep);
      }


    }
    else if( (elapsed - frame) > thresh){ // rendering or whatever is slower than frame
      //std::cerr  << "dropping frame!!!!!!!!!!!!!!! " << std::endl;
      return true; // drop frame

    }

    return false;
  }
  
}

