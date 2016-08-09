#ifndef TIMER_DATABASE_HPP
#define TIMER_DATABASE_HPP

#include "timer_gpu.hpp"

#include <vector>
#include <map>

//singleton
class TimerDatabase {
 public:
  static TimerDatabase& instance();
    
  void begin(std::string const&);
  void end(std::string const&);
  void addTimer(std::string const&);

  double duration(std::string const&) const;
  double mean(std::string const&) const;

 private:
  // prevent construction by user
  TimerDatabase();
  TimerDatabase(TimerDatabase const&) = delete;
  TimerDatabase& operator=(TimerDatabase const&) = delete;

  TimerGPU& get(std::string const&);
  double getNum(std::string const&) const;

  std::map<std::string, TimerGPU> m_timers;
  std::map<std::string, std::size_t> m_nums;
  std::map<std::string, double> m_means;
};

#endif //TIMER_DATABASE_HPP