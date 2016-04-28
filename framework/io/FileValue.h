#ifndef SYS_FILEVALUE_H
#define SYS_FILEVALUE_H


#include <string>

namespace sys{

  class FileValue{

  public:
    FileValue(const char* path);
    ~FileValue();

    bool read(float& value);
    bool read(int& value);

  private:
    std::string m_path;
  };

}


#endif // #ifndef SYS_FILEVALUE_H
