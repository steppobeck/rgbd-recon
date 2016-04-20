#ifndef GPUTIMER_H
#define GPUTIMER_H


#include <timevalue.h>

/*
    sensor::timevalue ts_now(sensor::clock::time());
    m_loopControl.draw_time = (ts_now - ts).msec();
    m_loopControl.avg_fps = std::floor(1000.0/ smoother((int) (m_loopControl.draw_time)));

    ts = ts_now;



*/

namespace sensor{

  class GPUTimer{

  public:
    GPUTimer();
    virtual ~GPUTimer();

    virtual void start();
    virtual void stop();



    const timevalue& get();


  protected:
    timevalue m_time;
    unsigned int m_id;

  };


}



#endif // #ifndef GPUTIMER_H
