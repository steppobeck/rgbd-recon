#include "configurator.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

void Configurator::read(std::string const& filename) {
  std::ifstream file{filename};
  std::string line;
  while (std::getline(file, line)) {
    line.erase(remove_if(line.begin(), line.end(), isspace),line.end());
    std::istringstream line_stream{line};
    std::string name;
    std::getline(line_stream, name, ':');
    // skip empty lines
    if (name.size() < 2) continue;
    // skip comments
    if (name.at(0) == '#') continue;

    std::size_t count = std::count(line.begin(), line.end(), ',');
    if (count == 0) {
      std::string val;
      std::getline(line_stream, val);
      bool is_uint = std::all_of(val.begin(), val.end(), isdigit);
      if (is_uint) {
        m_uints[name] = atoi(val.c_str());
      }
      else {
        bool is_text = std::all_of(val.begin(), val.end(), isalpha);
        if (is_text) {
          m_bools[name] = (val == "true") ? true : false;
        }
        else {
          m_floats[name] = atof(val.c_str());
        }
      }
    }
    // else if(count == 1) {
    //   std::string a, b;
    //   std::getline(line_stream, a, ',');
    //   std::getline(line_stream, b, ',');
    //   m_bools[name] = glm::vec2{atof(a.c_str()), atof(b.c_str())};
    // }
    else {
      std::string element;
      std::vector<unsigned> list{};
      while(std::getline(line_stream, element, ',')) {
        list.push_back(atoi(element.c_str()));
      }

      m_lists[name] = list;
    }
  }
}

void Configurator::print() const{
  std::cout << "floats" << std::endl;
  for (auto const& pair : m_floats) {
    std::cout << pair.first << ": " << pair.second << std::endl;
  }
  std::cout << "uints" << std::endl;
  for (auto const& pair : m_uints) {
    std::cout << pair.first << ": " << pair.second << std::endl;
  }
  std::cout << "bools" << std::endl;
  for (auto const& pair : m_bools) {
    std::cout << pair.first << ": " << pair.second << std::endl;
  }
  std::cout << "lists" << std::endl;
  for (auto const& pair : m_lists) {
    std::cout << pair.first << ": ";
    for( auto const& e : pair.second) {
      std::cout << e << ", ";
    }
    std::cout << std::endl;
  }

}

bool Configurator::getBool(std::string const& name) const {
  if (m_bools.find(name) != m_bools.end()) {
    return m_bools.at(name);
  }
  else throw std::out_of_range("key \'" + name + "\'' not found");
 
  return false;
}
float Configurator::getFloat(std::string const& name) const {
  if (m_floats.find(name) != m_floats.end()) {
    return m_floats.at(name);
  }
  else throw std::out_of_range("key \'" + name + "\'' not found");
 
  return 0.0f;
}
unsigned Configurator::getUint(std::string const& name) const {
  if (m_uints.find(name) != m_uints.end()) {
    return m_uints.at(name);
  }
  else throw std::out_of_range("key \'" + name + "\'' not found");
 
  return 0;
}
std::vector<unsigned> Configurator::getList(std::string const& name) const {
  if (m_lists.find(name) != m_lists.end()) {
    return m_lists.at(name);
  }
  else throw std::out_of_range("key \'" + name + "\'' not found");
 
  return std::vector<unsigned>{};
}

Configurator& Configurator::inst() {
  static Configurator instance{};

  return instance;
}

Configurator& configurator() {
  return Configurator::inst();
}
