#ifndef CONFIGURATOR_HPP
#define CONFIGURATOR_HPP

#include <map>
#include <vector>
#include <string>

// config singleton
class Configurator {
 public:
  // access the singleton instance
  static Configurator& inst();
  // load variables from file, can be called mutliple times
  void read(std::string const& filename);
  // print out all loaded variables
  void print() const;

  // return variable in given format, throws outofrange
  bool getBool(std::string const& name) const;
  float getFloat(std::string const& name) const;
  unsigned getUint(std::string const& name) const;
  std::vector<unsigned> getList(std::string const& name) const;

 private:
  // prevent construction by user
  Configurator(){};
  ~Configurator(){};
  Configurator(Configurator const&) = delete;
  Configurator& operator=(Configurator const&) = delete;
  // loaded variables
  std::map<std::string, bool> m_bools;
  std::map<std::string, float> m_floats;
  std::map<std::string, unsigned> m_uints;
  std::map<std::string, std::vector<unsigned>> m_lists; 
};
// get singleton sinstance
Configurator& configurator();

#endif //CONFIGURATOR_HPP