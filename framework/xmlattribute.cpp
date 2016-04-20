
#include "xmlattribute.h"
#include <string.h>
#include <stdio.h>


namespace minixml{


/*static*/ const unsigned int xmlattribute::_max_value_size = 2000;


xmlattribute::xmlattribute(const char* a, const char* b)
  : _name(a),
    _value(b)
{}

xmlattribute::xmlattribute(const char* a, int b)
  : _name(a),
    _value()
{
  _value = int2str(b);
}



xmlattribute::xmlattribute(const char* a, float b)
  : _name(a),
    _value()
{
  _value = float2str(b);
}


/*virtual*/
xmlattribute::~xmlattribute()
{}


void
xmlattribute::setValue(const char* v){
  _value = v;
}


void
xmlattribute::setValue(int v){
  _value = int2str(v);
}

void
xmlattribute::setValue(float v){
  _value = float2str(v);
}


const char*
xmlattribute::getName() const{
  return _name.c_str();
}


const char*
xmlattribute::getValueString() const{
  return _value.c_str();
}

unsigned int
xmlattribute::getValueIntX() const{
  unsigned int v;
  sscanf(_value.c_str(),"%i",&v);
  return v;
}

int
xmlattribute::getValueInt() const{
  int v;
  sscanf(_value.c_str(),"%d",&v);
  return v;
}


float
xmlattribute::getValueFloat() const{
  float v;
  sscanf(_value.c_str(),"%f",&v);
  return v;
}


std::vector<int>
xmlattribute::getValuesInt(const char* a) const{
  std::vector<int> vecv;
#ifndef WIN32
  char *running;
  char *token;
  running = strdup (_value.c_str());

  token = strsep (&running, a);
  while(token != NULL){
    int v;
    sscanf(token,"%d",&v);
    vecv.push_back(v);
    token = strsep (&running, a);
  }
#endif
#ifdef WIN32
  std::cerr << "WARNING xmlattribute ...need to fix strsep()" << std::endl;
#endif
  return vecv;
}


std::vector<float>
xmlattribute::getValuesFloat(const char* a) const{
  std::vector<float> vecv;
#ifndef WIN32
  char *running;
  char *token;
  running = strdup (_value.c_str());

  token = strsep (&running, a);
  while(token != NULL){
    float v;
    sscanf(token,"%f",&v);
    vecv.push_back(v);
    token = strsep (&running, a);
  }
#endif
#ifdef WIN32
  std::cerr << "WARNING xmlattribute ...need to fix strsep()" << std::endl;
#endif
  return vecv;
}


/*static*/ std::string
xmlattribute::int2str(int a){
  char s[200];
  sprintf(s, "%d", a);
  return std::string(s);
}


/*static*/ std::string
xmlattribute::float2str(float a){
  char s[200];
  sprintf(s, "%f", a);
  return std::string(s);
}
}
