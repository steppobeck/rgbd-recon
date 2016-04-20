#ifndef RGBD_CALIB_CHRONOMETER_H
#define RGBD_CALIB_CHRONOMETER_H

#include <chrono>

namespace kinect{

  class ChronoMeter{

  public:

    ChronoMeter();
    ~ChronoMeter();

    double getTick() const;

  private:
    std::chrono::system_clock::time_point m_ts;

  };

}

#endif // #ifndef RGBD_CALIB_CHRONOMETER_H
