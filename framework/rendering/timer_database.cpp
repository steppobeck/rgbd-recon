#include "timer_database.hpp"
#include <limits>
#include <fstream>

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
  m_extrema.emplace(name, std::make_pair(std::numeric_limits<double>::infinity(), 0));
}

TimerGPU& TimerDatabase::get(std::string const& name) {
  return m_timers.at(name);
}

void TimerDatabase::begin(std::string const& name)  {
  auto& timer{m_timers.at(name)};
  if(timer.outdated()) {
    auto& num{m_nums.at(name)};
    m_means.at(name) = (m_means.at(name) * num + timer.duration()) / (num + 1); 
    auto& extremum{m_extrema.at(name)};
    if(extremum.first > timer.duration()) {
      extremum.first = timer.duration();
    }
    else if (extremum.second < timer.duration()){
      extremum.second = timer.duration();
    }
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

void TimerDatabase::write(std::string const& file_name) const {
  std::ofstream file{file_name};
  std::string line;
  file << "timer,mean,min,max" << std::endl;
  auto iter_mean{m_means.begin()};
  for(auto const& pair : m_means) {
    file << "\"" << pair.first << "\"" << "," << pair.second / 1000000.0f << ",";
    auto const& extremum = m_extrema.at(pair.first);
    file << extremum.first / 1000000.0f << "," << extremum.second / 1000000.0f << std::endl;
  }
}
