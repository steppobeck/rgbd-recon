#ifndef FRAMETIMEADJUSTER_H
#define FRAMETIMEADJUSTER_H


#include <timevalue.h>


namespace kinect{


  class FrameTimeAdjuster{

  public:

    FrameTimeAdjuster();
    ~FrameTimeAdjuster();

    bool putTime(const sensor::timevalue& ts);

  private:

    sensor::timevalue m_ts_old;
    sensor::timevalue m_ts_old_frame;
    unsigned m_framenr;

  };

}


#endif // #ifndef FRAMETIMEADJUSTER_H

