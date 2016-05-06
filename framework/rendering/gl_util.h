#ifndef GL_UTIL_H
#define GL_UTIL_H

#include <GL/glew.h>
#include <stdexcept>
#include <iostream>


inline
void check_gl_errors(const char* msg, bool th = true){
  bool error = false;
  GLuint errnum;
  const char *errstr;
  while ((errnum = glGetError())) {
    errstr = reinterpret_cast<const char *>(gluErrorString(errnum));
    
    std::cout << "GLError: " << errstr << std::endl;
    std::cout << "   at: " << msg << std::endl;
    std::cout.flush();
    error = true;
  }
  
  if (th && error) {
    throw std::runtime_error(
			     std::string("OpenGL Error at ") + std::string(msg)
			     );
  }

}


#endif // #ifndef GL_UTIL_H
