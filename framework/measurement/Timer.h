#ifndef TIMER_H
#define TIMER_H


#include <timevalue.h>

/*
    sensor::timevalue ts_now(sensor::clock::time());
    m_loopControl.draw_time = (ts_now - ts).msec();
    m_loopControl.avg_fps = std::floor(1000.0/ smoother((int) (m_loopControl.draw_time)));

    ts = ts_now;



*/

namespace sensor{

  class Timer{

  public:
    Timer();
    virtual ~Timer();

    virtual void start(bool do_reset = true);
    virtual void stop();

    virtual void reset();


    const timevalue& get();


  protected:
    timevalue m_time;
    timevalue m_start;

  };


}



#endif // #ifndef TIMER_H
