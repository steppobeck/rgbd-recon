
// include i/f header

#include "device_dtrack.h"
#include <ChronoMeter.h>
// includes, system

#include <iostream>
#include <boost/thread/mutex.hpp>

// includes, project
#include <clock.h>
#include <dtrack.h>

#include <fstream>
#include <iomanip>
// internal unnamed namespace

namespace {
  
  // types, internal (class, enum, struct, union, typedef)

  // variables, internal
  
  const size_t dfault_udp_bufsize = 10000;
    
  // functions, internal

} // namespace {

namespace sensor {

// variables, exported
  /*static*/ const char* device_dtrack::filename = 0;
  /*static*/ unsigned*   device_dtrack::id = 0;
  /*static*/ kinect::ChronoMeter* device_dtrack::cm = 0;
// functions, exported
device_dtrack::device_dtrack(size_t a, timevalue b)
  : device(),
    _dtrack(new DTrack),
    _port(a),
    _timeout(b)
{}

device_dtrack::~device_dtrack()
{
  delete _dtrack;
}


/* virtual */ bool
device_dtrack::startDevice()
{
  dtrack_init_type ini;

  ini.udpport = _port;
  ini.udpbufsize = dfault_udp_bufsize;
  ini.udptimeout_us = _timeout.usec();
  ini.remote_ip[0] = '\0';
  ini.remote_port = 0;
  
  if (int err = _dtrack->init(&ini)) {
    std::cerr << "device_dtrack::startDevice:"
              << " unable to initialize DTrack; error code = " << err
              << std::endl;
    
    stopDevice();

    return !err;
  } else {
    _dtrack->send_udp_command(DTRACK_CMD_CAMERAS_AND_CALC_ON, 1);
  }

  
  return true;
}

/* virtual */ bool
device_dtrack::stopDevice()
{
   _dtrack->exit();
   
   return true;
}


/* virtual */ void
device_dtrack::readLoop()
{
  unsigned long     framecount(0);
  unsigned long     framenr;
  double            timestamp;
  int               nbodycal;
  int               nbody;
  dtrack_body_type* body = 0;
  unsigned int      max_bodies = 0;
  int               dummy;
  unsigned int      max_station_id = 0;
  std::ofstream* sweep_poses = 0;        
  if(filename != 0){
    sweep_poses = new std::ofstream(filename, std::ofstream::binary);
  }
  while (_running) {

    ++framecount;
    
    max_station_id = _station_list.size() - 1;
    
    if (max_bodies < max_station_id) {
      if (body)
        ::free(body);
      
      max_bodies = max_station_id + 1;
      
      body = (dtrack_body_type*) ::malloc(sizeof(dtrack_body_type) * max_bodies);
    }
    
    const int err = _dtrack->receive_udp_ascii(&framenr, &timestamp, &nbodycal,
                                               &nbody, body, max_bodies,
                                               &dummy, 0,    0,
                                               &dummy, 0,    0,
                                               &dummy, 0,    0);
    
    if (DTRACK_ERR_NONE != err) {
      std::string msg;
      
      switch (err) {
      case DTRACK_ERR_UDP:     msg = "error handling the udp socket"; break;
      case DTRACK_ERR_MEM:     msg = "error handling the udp buffer"; break;
      case DTRACK_ERR_TIMEOUT: msg = "timeout while receiving data"; break;
      case DTRACK_ERR_CMD:     msg = "error while sending remote command"; break;
      case DTRACK_ERR_PCK:     msg = "error in udp packet"; break;
      }
      
      if(framecount%30 == 0){
	std::cerr << "device_dtrack::readLoop: "
		  << "dtrack: '" <<  msg << "'"
		  << std::endl;
      }
      
      continue;
    } else {
      const unsigned int body_limit(std::min(static_cast<unsigned>(nbody), max_bodies));
      
      for (unsigned int i = 0; i < body_limit; ++i) {
        const int               idx = body[i].id;
        const dtrack_body_type& b(body[i]);
        
        {
          boost::mutex::scoped_lock lock(*_station_list[idx].mutex);
          
          gloost::Matrix& m(_station_list[idx].matrix);
          
          m.setIdentity();
          
	  gloost::mathType* m_data = m.data();

	  /*

	  0  4  8 12

	  1  5  9 13

	  2  6 10 14

	  3  7 11 15
	  

	  */


	  m_data[0] = b.rot[0]; m_data[4] = b.rot[3]; m_data[8]  = b.rot[6];  m_data[12] = b.loc[0] / 1000.0;
          m_data[1] = b.rot[1]; m_data[5] = b.rot[4]; m_data[9]  = b.rot[7];  m_data[13] = b.loc[1] / 1000.0;
          m_data[2] = b.rot[2]; m_data[6] = b.rot[5]; m_data[10] = b.rot[8];  m_data[14] = b.loc[2] / 1000.0;

	  
	  

          // m[0][3] = 0;        m[1][3] = 0;        m[2][3] = 0;      m[3][3] = 1;



	  
          if(sweep_poses != 0){
	    if( unsigned(idx) == *id){
	      const double tick = cm->getTick();

	      static bool firsttime = true;
	      static double timeoffset = 0.0;
	      
	      if(firsttime){
		firsttime = false;
		timeoffset = tick - timestamp;
	      }
	      const double art_tick = timestamp + timeoffset;

	      //std::cerr << "device_dtrack::readLoop(): writing to " << filename << std::endl;
	      //std::cerr << "------------------------------------------" << std::endl;
	      //std::cerr << "system_clock; " << std::setprecision(20) << tick << std::endl;
	      //std::cerr << "ART_clock; " << std::setprecision(20) << timestamp << std::endl;
	      //std::cerr << "ART_system_clock; " << std::setprecision(20) << art_tick << std::endl;

	      //sweep_poses->write((const char*) &tick, sizeof(double));
	      sweep_poses->write((const char*) &art_tick, sizeof(double));
	      sweep_poses->write((const char*) m.data(), sizeof(gloost::Matrix));

	    }
	  }
          


        }
        
        _station_list[idx].timestamp = (clock::time() - _station_list[idx].timestamp);
        
        ++_station_list[idx].frame;
      } // ~lock
    }
  }

  if(sweep_poses != 0){
    sweep_poses->close();
    delete sweep_poses;
  }


}

} // end namespace sensor
