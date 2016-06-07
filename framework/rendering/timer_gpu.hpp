#ifndef TIMER_GPU_HPP
#define TIMER_GPU_HPP

#include <globjects/base/ref_ptr.h>
namespace globjects {
  class Query;
}

#include <cstdint>

class TimerGPU {
 public:
  TimerGPU();
  
  void begin();
  void end();
  // in ns
  std::uint64_t duration();
  
 private:
  globjects::ref_ptr<globjects::Query> m_query;
  std::uint64_t m_start;
  std::uint64_t m_end;
};

#endif