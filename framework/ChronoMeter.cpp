#include "ChronoMeter.h"

#include <ratio>

namespace kinect{

  ChronoMeter::ChronoMeter()
    : m_ts(std::chrono::system_clock::now())
  {}

  ChronoMeter::~ChronoMeter()
  {}

  double
  ChronoMeter::getTick() const{
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    std::chrono::duration<double> dur = std::chrono::duration_cast<std::chrono::duration<double>>(tp - m_ts);
    return dur.count();
  }

}
