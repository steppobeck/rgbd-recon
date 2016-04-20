
#include <devicemanager.h>
#include <device_dtrack.h>
#include <ChronoMeter.h>

// includes, system

#include <iostream>

// includes, project

// #include <>

// internal unnamed namespace

namespace {
  
  // types, internal (class, enum, struct, union, typedef)
  
  // variables, internal
  
  // functions, internal

} // namespace {

// variables, exported


namespace sensor {

/*static*/ devicemanager* devicemanager::_me = NULL;

// functions, exported
//   std::map<std::string, device* > _zmq_matrices;
devicemanager::devicemanager()
  : _dtracks()
{}



devicemanager::~devicemanager()
{
  for(std::map<int, device* >::iterator i = _dtracks.begin(); i != _dtracks.end(); ++i){
    i->second->stop();
    delete i->second;
  }
}


/*static*/ devicemanager* const
devicemanager::the(){
  if(!_me)
    _me = new devicemanager;
  return _me;
}

/*static*/ void
devicemanager::shutdown(){
  delete _me;
  _me = NULL;
}



device*
devicemanager::get_dtrack(int a, timevalue b, const char* filename, unsigned* id, kinect::ChronoMeter* cm){

  if(!_dtracks[a]){
    _dtracks[a] = new device_dtrack(a, b);

    if(filename != 0){
      device_dtrack::filename = filename;
      device_dtrack::id = id;
      device_dtrack::cm = cm;
    }

    _dtracks[a]->start();
  }

  return _dtracks[a];
}


} // end namespace sensor

