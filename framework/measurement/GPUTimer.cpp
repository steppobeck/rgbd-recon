#include "GPUTimer.h"



#include <GL/glew.h>

#include <iostream>
#include <cstdlib>

namespace sensor{


  GPUTimer::GPUTimer()
    : m_time(),
      m_id(0)
  {}

  GPUTimer::~GPUTimer()
  {}



  /*virtual*/ void
  GPUTimer::start(){
    if(0 == m_id){
      glGenQueries(1, &m_id);
      if(0 == m_id){
	std::cerr << "ERROR: GPUTimer::start() ...could not request a query id" << std::endl;
	exit(0);
      }
    }

    glBeginQuery(GL_TIME_ELAPSED_EXT, m_id);

  }


  /*virtual*/ void
  GPUTimer::stop(){
    glEndQuery(GL_TIME_ELAPSED_EXT);
    GLuint available = 0;
    while (!available) {
      glGetQueryObjectuiv(m_id, GL_QUERY_RESULT_AVAILABLE, &available);
    }
    unsigned int timeElapsed;
    glGetQueryObjectuiv(m_id, GL_QUERY_RESULT, &timeElapsed);
    m_time = sensor::timevalue(0,timeElapsed);
  }




  const timevalue&
  GPUTimer::get(){
    return m_time;
  }


}
