
#include "Statistics.h"

#include <Smoother.h>
#include <Timer.h>
#include <GPUTimer.h>


namespace mvt{

  Statistics::Statistics(const char* text)
    : m_text(new gloost::TextureText(text)),
      m_timers(),
      m_gputimers(),
      m_gpuslots(),
      m_infoslots(),
      m_framenr(0)
  {
    m_timers.push_back(new sensor::Timer);
    setCPUSlot("frame time total",0);
    m_gputimers.push_back(new sensor::GPUTimer);
  }


  Statistics::~Statistics(){
    for (unsigned i = 0; i < m_timers.size(); ++i)
      delete m_timers[i];
    for (unsigned i = 0; i < m_gputimers.size(); ++i)
      delete m_gputimers[i];
    delete m_text;
  }

  void
  Statistics::draw(unsigned width, unsigned height){
    static sensor::Smoother s_smooth_dt(20);
    m_timers[0]->stop();
    
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    {
      
      glViewport(0, 0, width, height);

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0, (float) width, 0, (float) height, -1.0, 1.0);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();


      glDisable(GL_COLOR_MATERIAL);
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_LIGHTING);
      const float avg_dt(s_smooth_dt((int) (m_timers[0]->get().msec())));
      const float fps(1000.0 / avg_dt);
      m_text->begin();
      m_text->renderTextLine(15.0, height - 40.0, std::string("fps: ") + gloost::toString(fps));

      for(unsigned i = 0; i < m_gpuslots.size(); ++i){
	m_text->renderTextLine(m_gpuslots[i] + std::string(": ") + gloost::toString(m_gputimers[i]->get().usec() * 0.001) + gloost::toString(" ms"));
      }

      for(unsigned i = 0; i < m_timersslots.size(); ++i){
	m_text->renderTextLine(m_timersslots[i] + std::string(": ") + gloost::toString(m_timers[i]->get().usec() * 0.001) + gloost::toString(" ms"));
      }

      for(unsigned i = 0; i < m_infoslots.size(); ++i){
	m_text->renderTextLine(m_infoslots[i]);
      }

      m_text->end();
      
    }
    glPopAttrib();
    m_timers[0]->start();
    
  }

  void
  Statistics::print(unsigned framedrop){
    ++m_framenr;
    if(m_framenr%framedrop == 0){
      std::cerr << "Statistics -------------------------------------- " << m_framenr << std::endl;
      for(unsigned i = 0; i < m_gpuslots.size(); ++i){
	std::cerr << m_gpuslots[i] + std::string(": ") + gloost::toString(m_gputimers[i]->get().usec() * 0.001) + gloost::toString(" ms") << std::endl;
      }

    }
  }







  void
  Statistics::setInfoSlot(const char* text, unsigned slot){
    if(m_infoslots.size() < (slot + 1)){
      m_infoslots.push_back(text);
    }
    m_infoslots[slot] = text;
  }

  void
  Statistics::setGPUSlot(const char* text, unsigned slot){
    if(m_gpuslots.size() < (slot + 1)){
      m_gpuslots.push_back(text);
    }
    m_gpuslots[slot] = text;
  }

  void
  Statistics::startGPU(unsigned slot){
    
    if((m_gputimers.size() - 1) < slot){
      m_gputimers.push_back(new sensor::GPUTimer);
    }
    m_gputimers[slot]->start();
  }
  
  void
  Statistics::stopGPU(unsigned slot){
    
    if((m_gputimers.size() - 1) < slot){
      m_gputimers.push_back(new sensor::GPUTimer);
    }
    m_gputimers[slot]->stop();    
  }


  void
  Statistics::setCPUSlot(const char* text, unsigned slot){
    if(m_timersslots.size() < (slot + 1)){
      m_timersslots.push_back(text);
    }
    m_timersslots[slot] = text;
  }

  void
  Statistics::startCPU(unsigned slot){
    
    if((m_timers.size() - 1) < slot){
      m_timers.push_back(new sensor::Timer);
    }
    m_timers[slot]->start();
  }
  
  void
  Statistics::stopCPU(unsigned slot){
    
    if((m_timers.size() - 1) < slot){
      m_timers.push_back(new sensor::Timer);
    }
    m_timers[slot]->stop();    
  }


  void
  Statistics::sleepIfMoreThanMsecGPU(unsigned msec){
    const float dt(m_timers[0]->get().msec());
    if(dt < msec){
      double t((msec - dt) * 0.001);
      std::cerr << t << std::endl;
      sensor::timevalue ts(t);
      sleep(ts);
    }
  }

}
