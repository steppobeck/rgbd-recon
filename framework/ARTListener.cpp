#include "ARTListener.h"

#include <sensor.h>
#include <clock.h>
#include <devicemanager.h>
#include <device.h>

#include <Shader.h>
#include <UniformSet.h>
#include <Obj.h>

#include <string.h> // memcpy

#include <fstream>



namespace kinect{

  /*static*/ bool ARTListener::s_glewInit = false;

  ARTListener::ARTListener(const char* artconfig)
    : m_matrices(),
      m_sensors(),
      m_dev(0),
      m_artconfig(artconfig),
      m_objs(),
      m_objs_filename(),
      m_shader(0),
      m_uniforms(0),
      m_geomapping(),
      m_framenr(0),
      m_ts(sensor::clock::time())
  {
    gloost::Matrix m;
    m.setIdentity();
    for(unsigned i = 0; i < ARTLISTENERNUMSENSORS; ++i){
      m_matrices.push_back(m);
      m_geomapping.push_back(-1);
    }
  }


  ARTListener::~ARTListener(){
    // dont know how to handle this right now
  }

  bool
  ARTListener::isOpen(){
    return m_dev != 0;
  }

  bool
  ARTListener::open(unsigned port){
    m_dev = sensor::devicemanager::the()->get_dtrack(port, sensor::timevalue::const_050_ms);
    gloost::Matrix m;
    m.setIdentity();
    for(unsigned i = 0; i < ARTLISTENERNUMSENSORS; ++i){
      sensor::sensor* s = new sensor::sensor(m_dev,i);
      s->setTransmitterOffset(m);
      s->setReceiverOffset(m);
      m_sensors.push_back(s);
    }
    return true;
  }


  void
  ARTListener::listen(){
    for(unsigned i = 0; i < ARTLISTENERNUMSENSORS; ++i){
      m_matrices[i] = m_sensors[i]->getMatrix();
    }
    m_ts = sensor::clock::time();
    ++m_framenr;
  }



  void
  ARTListener::fill(void* destination){
    // fill matrices into source
    const unsigned ts_address = ARTLISTENERNUMSENSORS * sizeof(gloost::Matrix);
    memcpy(destination, (const char*) &(m_matrices.front()), /*ARTLISTENERNUMSENSORS * sizeof(gloost::Matrix)*/ ts_address);

    memcpy(destination + ts_address, &m_ts, sizeof(sensor::timevalue));
    const unsigned framenr_address = ts_address + sizeof(sensor::timevalue);
    memcpy(destination + framenr_address, &m_framenr, sizeof(m_framenr));

  }


  std::vector<gloost::Matrix>&
  ARTListener::get(const void* source){
    // apply matrices from source
    const unsigned ts_address = ARTLISTENERNUMSENSORS * sizeof(gloost::Matrix);
    memcpy((char*) &(m_matrices.front()), source, ts_address /*ARTLISTENERNUMSENSORS * sizeof(gloost::Matrix)*/);
    //memcpy(&m_ts,                         source + ts_address, sizeof(sensor::timevalue));
    return m_matrices;
  }


  void
  ARTListener::draw(){
    if(m_shader == 0){
      initGL();
    }

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    m_shader->set();
    m_uniforms->applyToShader(m_shader);

    for(unsigned i = 0; i < ARTLISTENERNUMSENSORS; ++i){
      int gm(m_geomapping[i]);
      if(gm >= 0){

	glPushMatrix();
	gloostMultMatrix(m_matrices[i].data());

	const GLfloat darkgrey[4] = { .1f, .1f, 0.1f, 1.f };
	glMaterialfv( GL_FRONT, GL_DIFFUSE, darkgrey );
	glMaterialfv( GL_FRONT, GL_SPECULAR, darkgrey );

	if(0 == gm){
	  glScalef(0.001,0.001,0.001);
	  glRotatef(90,1.0,0.0,0.0);
	  glRotatef(90,0.0,0.0,1.0);
	  m_objs[gm]->drawFacesNormalsTexcoords();
	}
	else{
	  m_objs[gm]->drawFacesNormalsTexcoords();
	}

	glPopMatrix();

      }
    }

    m_shader->disable();

    glPopAttrib();

  }

  void
  ARTListener::initGL(){


    if(!s_glewInit){
      // initialize GLEW
      if (GLEW_OK != glewInit()){
	/// ... or die trying
	std::cout << "'glewInit()' failed." << std::endl;
	exit(0);
      }
      else{
	s_glewInit = true;
      }
    }


    m_uniforms = new gloost::UniformSet;
    m_shader   = new gloost::Shader("glsl/material_phong.vs","glsl/material_phong.fs");
    //m_objs[0]  = new gloost::Obj("glsl/glasses.obj");


    // parse if available m_artconfig
    if(m_artconfig != ""){

      std::string filename = "glsl/" + m_artconfig;
      std::string::size_type idx = filename.rfind('.');
      
      if(idx != std::string::npos){
	filename.replace( filename.begin() + idx, filename.end(), ".art");
	std::cerr << filename << std::endl;

	std::ifstream file(filename.c_str());
	if (file.good()){
	  unsigned ids = 0;
	  std::string token;
	  while(file >> token){
	    //std::cerr << token << std::endl;
	    if("obj" == token){
	      file >> token;
	      //std::cerr << token << std::endl;
	      m_objs[ids]  = new gloost::Obj(token.c_str());
	      m_objs_filename[ids]  = token;
	      ++ids;
	    }
	    else if("sensor" == token){
	      unsigned sensorid   = 0;
	      unsigned geometryid = 0;
	      file >> sensorid;
	      file >> geometryid;
	      if( (m_geomapping.size() > sensorid) && m_objs[geometryid]){
		m_geomapping[sensorid] = geometryid;
	      }
	      else{
		std::cerr << "ARTListener::initGL() ERROR -> invalid mapping" << std::endl;
	      }
	    }

	  }

	  
	}
	file.close();
      }

    }

  }

  const sensor::timevalue&
  ARTListener::getTimeStamp(){
    return m_ts;
  }


  const std::vector<gloost::Matrix>&
  ARTListener::getMatrices() const{
    return m_matrices;
  }


  bool
  ARTListener::isGlass(unsigned index){
    int gm(m_geomapping[index]);
    if(gm >= 0){
      return m_objs_filename[gm] == "glsl/glasses.obj";
    }
    return false;
  }

}
