#include "Timer.h"


#include <clock.h>

namespace sensor{


  Timer::Timer()
    : m_time(clock::time()),
      m_start(clock::time())
  {}

  Timer::~Timer()
  {}



  /*virtual*/ void
  Timer::start(bool do_reset){
    if(do_reset)
      reset();
  }


  /*virtual*/ void
  Timer::stop(){
    m_time = clock::time() - m_start;
  }


  /*virtual*/ void
  Timer::reset(){
    m_start = clock::time();
  }


  const timevalue&
  Timer::get(){
    return m_time;
  }


}
