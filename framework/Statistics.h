#ifndef MVT_STATISTICS_H
#define MVT_STATISTICS_H


#include <TextureText.h>



namespace sensor{
  class Timer;
  class GPUTimer;
}


#include <vector>
#include <string>

namespace mvt{

  class Statistics{

  public:
    Statistics(const char* text = "../../data/fonts/gloost_CourierNew_16.png");
    ~Statistics();

    void draw(unsigned width, unsigned height);
    void print(unsigned framedrops = 20);

    void setInfoSlot(const char* text, unsigned slot = 0);
    void setGPUSlot(const char* text, unsigned slot = 0);

    void startGPU(unsigned slot = 0);
    void stopGPU(unsigned slot = 0);

    void setCPUSlot(const char* text, unsigned slot);

    void startCPU(unsigned slot);
    void stopCPU(unsigned slot);

    void sleepIfMoreThanMsecGPU(unsigned msec);


  private:
    gloost::TextureText* m_text;
    std::vector<sensor::Timer*> m_timers;
    std::vector<sensor::GPUTimer*> m_gputimers;
    std::vector<std::string> m_gpuslots;
    std::vector<std::string> m_timersslots;
    std::vector<std::string> m_infoslots;
    unsigned                 m_framenr;
  };

}


#endif // #ifndef  MVT_STATISTICS_H
