#include "FileValue.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>

namespace sys{

  FileValue::FileValue(const char* path)
    : m_path(path)
  {}

  FileValue::~FileValue()
  {}


  bool
  FileValue::read(float& value){
    struct stat st;
    if(stat(m_path.c_str(),&st) == 0){
      std::ifstream in(m_path.c_str());
      in >> value;
      in.close();
      return true;
    }
    return false;
  }

  bool
  FileValue::read(int& value){
    struct stat st;
    if(stat(m_path.c_str(),&st) == 0){
      std::ifstream in(m_path.c_str());
      in >> value;
      in.close();
      return true;
    }
    return false;
  }

}
