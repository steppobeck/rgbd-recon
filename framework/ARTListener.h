#ifndef KINECT_ARTLISTENER_H
#define KINECT_ARTLISTENER_H

#define ARTLISTENERNUMSENSORS 50

#include <Matrix.h>

#include <timevalue.h>

#include <vector>
#include <map>
#include <string>

namespace sensor{
  class sensor;
  class device;
}

namespace gloost{
  class Shader;
  class UniformSet;
  class Obj;
}

namespace kinect{


  class ARTListener{


  public:
    ARTListener(const char* artconfig);
    ~ARTListener();

    bool isOpen();
    bool open(unsigned port);
    void listen();

    void fill(void* destination);
    std::vector<gloost::Matrix>& get(const void* source);


    void draw();

    const sensor::timevalue& getTimeStamp();

    const std::vector<gloost::Matrix>& getMatrices() const;

    bool isGlass(unsigned index);

  private:
    void initGL();

    std::vector<gloost::Matrix> m_matrices;
    std::vector<sensor::sensor*> m_sensors;
    sensor::device* m_dev;

    std::string m_artconfig;
    std::map<unsigned, gloost::Obj*> m_objs;
    std::map<unsigned, std::string> m_objs_filename;
    gloost::Shader* m_shader;
    gloost::UniformSet* m_uniforms;
    std::vector<int> m_geomapping;
    sensor::timevalue m_ts;
    unsigned m_framenr;
    static bool s_glewInit;


  };

}



#endif  // #ifndef KINECT_ARTLISTENER_H
