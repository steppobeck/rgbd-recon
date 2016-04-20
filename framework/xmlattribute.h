#ifndef XMLATTRIBUTE_H
#define XMLATTRIBUTE_H


#include <vector>
#include <string>

namespace minixml{

class xmlattribute{

 public:
    
  xmlattribute(const char* /*name*/, const char* /*value*/);
  xmlattribute(const char* /*name*/, int /*value*/);
  xmlattribute(const char* /*name*/, float /*value*/);
  virtual ~xmlattribute();

  void setValue(const char* v);
  void setValue(int v);
  void setValue(float v);

  const char* getName() const;
  const char* getValueString() const;

  unsigned int getValueIntX() const;
  int getValueInt() const;
  float getValueFloat() const;
  std::vector<int> getValuesInt(const char* /*seperator*/) const;
  std::vector<float> getValuesFloat(const char* /*seperator*/) const;

    
  static std::string int2str(int a);
  static std::string float2str(float a);
  
 private:

  static const unsigned int _max_value_size;
  std::string _name;
  std::string _value;
  
};

}

#endif // #ifndef XMLATTRIBUTE_H
