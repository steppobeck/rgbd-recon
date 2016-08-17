#include "timer_database.hpp"
#include <limits>
#include <iostream>
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

void TimerDatabase::writeMean(std::string const& file_name) const {
  std::size_t pos = file_name.find_last_of('/');
  std::string filename{file_name.substr(pos + 1)};
  std::size_t pos2 = filename.find_first_of(',');
  std::string name{filename.substr(0, pos2)};
  // std::string ext{filename.substr(pos2 + 1)};
  std::string path{file_name.substr(0, pos + 1)};
  std::cout << "path " << path << ", file " << filename << ", name " << name << std::endl;

  std::ofstream file{path + "mean_" + filename};

  file << "timer";
  for(auto const& pair : m_means) {
    file << ",\"" << pair.first << "\"";
  }
  file << std::endl << name;
  for(auto const& pair : m_means) {
    file << "," << pair.second / 1000000.0f;
  }
  file << std::endl;
}
void TimerDatabase::writeMin(std::string const& file_name) const {
  std::size_t pos = file_name.find_last_of('/');
  std::string filename{file_name.substr(pos + 1)};
  std::size_t pos2 = filename.find_first_of(',');
  std::string name{filename.substr(0, pos2)};
  // std::string ext{filename.substr(pos2 + 1)};
  std::string path{file_name.substr(0, pos + 1)};
  std::cout << "path " << path << ", file " << filename << ", name " << name << std::endl;

  std::ofstream file{path + "min_" + filename};

  file << "timer";
  for(auto const& pair : m_extrema) {
    file << ",\"" << pair.first << "\"";
  }
  file << std::endl << name;
  for(auto const& pair : m_extrema) {
    file << "," << pair.second.first / 1000000.0f;
  }
  file << std::endl;
}
void TimerDatabase::writeMax(std::string const& file_name) const {
  std::size_t pos = file_name.find_last_of('/');
  std::string filename{file_name.substr(pos + 1)};
  std::size_t pos2 = filename.find_first_of(',');
  std::string name{filename.substr(0, pos2)};
  // std::string ext{filename.substr(pos2 + 1)};
  std::string path{file_name.substr(0, pos + 1)};
  std::cout << "path " << path << ", file " << filename << ", name " << name << std::endl;

  std::ofstream file{path + "max_" + filename};

  file << "timer";
  for(auto const& pair : m_extrema) {
    file << ",\"" << pair.first << "\"";
  }
  file << std::endl << name;
  for(auto const& pair : m_extrema) {
    file << "," << pair.second.second / 1000000.0f;
  }
  file << std::endl;
}
