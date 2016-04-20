#include "device.h"

// includes, system

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <iostream>

// includes, project

// #include <>

// internal unnamed namespace

namespace {
  
  // types, internal (class, enum, struct, union, typedef)
  
  // variables, internal

  const size_t const_default_station_size(100);
  const size_t const_default_button_size(32);
  
  // functions, internal

} // namespace {


namespace sensor {

// variables, exported

/* static */ const unsigned short device::station::max_buttons;

// functions, exported

device::station::station()
  : frame(0),
    timestamp(),
    matrix(),
    mutex(new boost::mutex)
{

  matrix.setIdentity();
}

device::station::~station()
{
  // delete mutex;
}

device::device()
  : _station_list(),
    _running(false),
    _readthread(0)
{
  for (unsigned i = 0; i < const_default_station_size; ++i){
    _station_list.push_back(station());
    
    bool* b(_station_list[i].button);
    
    for (int j = 0; j < station::max_buttons; ++j) {
      b[j] = false;
    }
  }
}

/* virtual */
device::~device()
{
  if (_running)
    stop();
}

bool
device::start()
{
  if(startDevice()){
    _running = true;
    
    _readthread = new boost::thread(boost::bind(&device::readLoop, this));
  }
  
  return _running;
}

bool
device::stop()
{
  _running = false;
  
  if(_readthread)
    _readthread->join();
  
  delete _readthread;
  
  return stopDevice();
}

const device::station&
device::getStation(size_t a) const
{
  return _station_list[a];
}

} // end namespace sensor

// $Id: device.c++,v 1.9 2006/11/14 20:10:37 beck3 Exp $
