#ifndef GL_UTIL_H
#define GL_UTIL_H

#include <glbinding/gl/gl.h>
using namespace gl;
#include <stdexcept>
#include <iostream>
#include <GL/glu.h>

inline
void check_gl_errors(const char* msg, bool th = true){
  bool error = false;
  GLenum errnum;
  const char *errstr;
  while ((errnum = glGetError()) != GL_NONE) {
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
