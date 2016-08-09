#include "timer_database.hpp"

TimerDatabase::TimerDatabase()
 :m_timers{}
{}

TimerDatabase& TimerDatabase::instance() {
  static TimerDatabase instance{};
  return instance;
}

void TimerDatabase::addTimer(std::string const& name) {
  m_timers.emplace(name, TimerGPU{});
  m_means.emplace(name, 0.0);
  m_nums.emplace(name, 0);
}

TimerGPU& TimerDatabase::get(std::string const& name) {
  return m_timers.at(name);
}

void TimerDatabase::begin(std::string const& name)  {
  auto& timer{m_timers.at(name)};
  if(timer.outdated()) {
    auto& num{m_nums.at(name)};
    m_means.at(name) = (m_means.at(name) * num + timer.duration()) / (num + 1); 
    num += 1;
  }
  m_timers.at(name).begin();
}

void TimerDatabase::end(std::string const& name)  {
  m_timers.at(name).end();
}

double TimerDatabase::duration(std::string const& name) const {
  return m_timers.at(name).duration();
}

double TimerDatabase::getNum(std::string const& name) const {
  return m_nums.at(name);
}

double TimerDatabase::mean(std::string const& name) const {
  return m_means.at(name);
}