#include "timer_gpu.hpp"

#include <glbinding/gl/enum.h>
using namespace gl;
#include <globjects/Query.h>

TimerGPU::TimerGPU()
 :m_query{new globjects::Query}
 ,m_start{0}
 ,m_end{0}
{}

void TimerGPU::begin() {
  m_query->counter();
}

void TimerGPU::end() {
  // get result from start
  m_start = m_query->get64(GL_QUERY_RESULT);
  m_query->counter();
}

std::uint64_t TimerGPU::duration() {
  m_end = m_query->waitAndGet64(GL_QUERY_RESULT);
  return m_end - m_start;
}