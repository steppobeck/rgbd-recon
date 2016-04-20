#include "KinectSurface.h"


#include <Shader.h>
#include <UniformSet.h>
#include <Obj.h>

#include <NetKinectArray.h>
#include <ARTListener.h>
#include <ProxyMeshGrid.h>
#include <KinectCalibrationFile.h>

#include <FileValue.h>

#include <gl_util.h>

#include <algorithm>
#include <unistd.h>

namespace{
    std::string vshader;
    std::string gshader;
    std::string fshader;
}

namespace kinect{


  KinectSurface::KinectSurface(const char* config)
    : m_config(config),
      m_nka(0),
      m_shaderWarp(0),
      m_uniformsWarp(0),
      m_proxyMesh(0),
      m_proxyMesh_low(0),
      m_hostname(),
      m_shader(0),
      m_uniforms(0),
      m_obj(0)
  {
    init(config);
  }


  KinectSurface::~KinectSurface(){
    delete m_nka;
    if(m_shaderWarp){
      delete m_shaderWarp;
    }
    if(m_uniformsWarp){
      delete m_uniformsWarp;
    }
    delete m_proxyMesh;
    delete m_proxyMesh_low;
  }

  void
  KinectSurface::draw(bool update, float scale, bool warpvis){


    if(m_nka->isPhoto()){
      drawMesh(update,scale);
      return;
    }

    //std::cerr << this << " draw " << update << std::endl;

    if(0 == m_shaderWarp)
      return;


    if(update)
      m_nka->update();
    
#if 1
    if("kerberos" == m_hostname && std::string::npos != m_config.find("DLP") && scale > 0.99)
       return;
    if("pandora" == m_hostname  && std::string::npos != m_config.find("LCD") && scale > 0.99)
       return;
#endif


    float min_length = m_nka->getCalibs()[0]->min_length/*0.0125*/;
    min_length = scale * min_length;

    m_uniformsWarp->set_float("min_length", min_length);

    m_uniformsWarp->set_float("groundlevel", m_nka->getCalibs()[0]->s_groundlevel);

    for(unsigned layer = 0; layer < m_nka->getNumLayers(); ++layer){

      KinectCalibrationFile* calib = m_nka->getCalibs()[layer];
      calib->updateMatrices();


      // now we do first: frustum culling
      if(calib->frustCull())
	continue;

      m_uniformsWarp->set_mat4("image_d_to_eye_d", calib->image_d_to_eye_d);
      //m_uniformsWarp->set_mat4("eye_d_to_world", calib->eye_d_to_world);		


      if(calib->isSensored()){
	m_uniformsWarp->set_mat4("eye_d_to_world", m_nka->current_poses[layer]);
      }
      else{
	m_uniformsWarp->set_mat4("eye_d_to_world", calib->eye_d_to_world);	
      }


      m_uniformsWarp->set_mat4("eye_d_to_eye_rgb", calib->eye_d_to_eye_rgb);
      m_uniformsWarp->set_mat4("eye_rgb_to_image_rgb", calib->eye_rgb_to_image_rgb);
      m_uniformsWarp->set_int("layer",  layer);
      m_uniformsWarp->set_vec2("texSizeInv", gloost::vec2(1.0f/calib->getWidth(), 1.0f/calib->getHeight()));
 
      m_uniformsWarp->set_vec3(     "pos_min", calib->pos_min);
      m_uniformsWarp->set_vec3(     "pos_max", calib->pos_max);
      
      m_uniformsWarp->set_vec3(     "neg_min", calib->neg_min);
      m_uniformsWarp->set_vec3(     "neg_max", calib->neg_max);

      


      glPushAttrib(GL_ALL_ATTRIB_BITS);
      {

	glDisable(GL_CULL_FACE);

	if(warpvis){
	  glColor4f(0.0,0.0,0.0,1.0);
	  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	}

	glPushMatrix();
	{
	  
	  m_shaderWarp->set();

	  m_uniformsWarp->applyToShader(m_shaderWarp);
	  
	  m_nka->bindToTextureUnits(GL_TEXTURE0);
	  //std::cerr << "update " << (int) update << std::endl;
	  if(warpvis){
	    m_proxyMesh_low->draw(scale);
	  }
	  else{
	    m_proxyMesh->draw(scale);
	  }

	  m_shaderWarp->disable();
	}
	glPopMatrix();
      }
      glActiveTexture(GL_TEXTURE0);
      glPopAttrib();
      
    }

    m_nka->drawGeometry();

  }

  void
  KinectSurface::drawMesh(bool update, float scale){

    //std::cerr << this << " draw " << update << std::endl;


    if(0 == m_shader){
      // initialize the uniforms and the shader like in RgbDepthRemapping
      m_uniforms = new gloost::UniformSet;
      m_uniforms->set_int("kinect_colors",0);
      m_uniforms->set_int("kinect_depths",1);

      m_shader = new gloost::Shader("glsl/body_texture.vs",
				    "glsl/body_texture.fs");
      
      
      std::string obj_filename(m_config);
      obj_filename.replace( obj_filename.end() - 3, obj_filename.end(), "obj");
      obj_filename = std::string("recordings/") + obj_filename;
      std::cerr << obj_filename << std::endl;
      m_obj = new gloost::Obj(obj_filename.c_str());
    }

#if 0
    if("kerberos" == m_hostname && std::string::npos != m_config.find("DLP") && scale > 0.99)
       return;
    if("pandora" == m_hostname  && std::string::npos != m_config.find("LCD") && scale > 0.99)
       return;
#endif

#if 0
    if(update)
      m_nka->update();
#endif

    /*
      uniform mat4 v_world_to_eye_d[MAX_VIEWS];
      uniform mat4 v_eye_d_to_eye_rgb[MAX_VIEWS];
      uniform mat4 v_eye_rgb_to_image_rgb[MAX_VIEWS];
    */

    std::vector<gloost::mat4> v_world_to_eye_d;
    std::vector<gloost::mat4> v_eye_d_to_world;
    std::vector<gloost::mat4> v_eye_d_to_eye_rgb;
    std::vector<gloost::mat4> v_eye_rgb_to_image_rgb;
    std::vector<kinect::KinectCalibrationFile*>& calibs = m_nka->getCalibs();


    for(unsigned i = 0; i < calibs.size(); ++i){

      kinect::KinectCalibrationFile* v = calibs[i];
      v->updateMatrices();

      gloost::Matrix world_to_eye_d(v->eye_d_to_world);
      world_to_eye_d.invert();
      v_world_to_eye_d.push_back(world_to_eye_d);
      v_eye_d_to_world.push_back(v->eye_d_to_world);
      v_eye_d_to_eye_rgb.push_back(v->eye_d_to_eye_rgb);
      v_eye_rgb_to_image_rgb.push_back(v->eye_rgb_to_image_rgb);

    }

    m_uniforms->set_mat4v(     "v_world_to_eye_d", v_world_to_eye_d);
    m_uniforms->set_mat4v(     "v_eye_d_to_world", v_eye_d_to_world);
    m_uniforms->set_mat4v(     "v_eye_d_to_eye_rgb", v_eye_d_to_eye_rgb);
    m_uniforms->set_mat4v(     "v_eye_rgb_to_image_rgb", v_eye_rgb_to_image_rgb);

    m_uniforms->set_int("num_layers", (int) calibs.size());

    float min_length = 0.04;
    min_length = scale * min_length;
    m_uniforms->set_float("min_length", min_length);



    glPushAttrib(GL_ALL_ATTRIB_BITS);
    {
      m_nka->bindToTextureUnits(GL_TEXTURE0);

      m_shader->set();
      m_uniforms->applyToShader(m_shader);

      m_obj->drawFacesNormalsTexcoords();

      m_shader->disable();      
      
      glActiveTexture(GL_TEXTURE0);
    }
    glPopAttrib();

    m_nka->drawGeometry();

  }


  /*virtual*/ void
  KinectSurface::init(const char* config){

    KinectCalibrationFile::s_use_sensor = false;
    //KinectCalibrationFile::s_compress   = true;

    char hostname[1024];
    gethostname(hostname, 1024);
    m_hostname = hostname;
    
    // parse the file .ks and find the shader and hostname entries

    std::vector<std::string> hostnames;
    
    std::ifstream in(config);
    std::string token;
    while(in >> token){
      if(token == "vshader"){
	in >> vshader;
      }
      else if(token == "gshader"){
	in >> gshader;
      }
      else if(token == "fshader"){
	in >> fshader;
      }
      else if(token == "hostname"){
	in >> token;
	hostnames.push_back(token);
      }

    }
    in.close();

    if (std::find(hostnames.begin(), hostnames.end(), hostname) == hostnames.end())
    {
        return;
    }

    m_nka = new NetKinectArray(config);

    // initialize the uniforms and the shader like in RgbDepthRemapping
    m_uniformsWarp = new gloost::UniformSet;
    m_uniformsWarp->set_int("kinect_colors",0);
    m_uniformsWarp->set_int("kinect_depths",1);



    m_shaderWarp = new gloost::Shader(vshader,
				  fshader,
				  gshader);

    m_shaderWarp->setProgramParameter(GL_GEOMETRY_INPUT_TYPE_EXT ,GL_TRIANGLES);
    m_shaderWarp->setProgramParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT ,GL_TRIANGLE_STRIP);
    m_shaderWarp->setProgramParameter(GL_GEOMETRY_VERTICES_OUT_EXT ,3);

    m_proxyMesh = new mvt::ProxyMeshGrid(m_nka->getWidth(),
					 m_nka->getHeight());

    m_proxyMesh_low = new mvt::ProxyMeshGrid(48,
					     36);

    
  }

  void
  KinectSurface::reloadShader(){
    if(m_shaderWarp){
      delete m_shaderWarp;
      m_shaderWarp = new gloost::Shader(vshader,
				    fshader,
				    gshader);
      
      m_shaderWarp->setProgramParameter(GL_GEOMETRY_INPUT_TYPE_EXT ,GL_TRIANGLES);
      m_shaderWarp->setProgramParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT ,GL_TRIANGLE_STRIP);
      m_shaderWarp->setProgramParameter(GL_GEOMETRY_VERTICES_OUT_EXT ,3);
    }
    m_nka->reloadShader();

    for(unsigned layer = 0; layer < m_nka->getNumLayers(); ++layer){

      KinectCalibrationFile* calib = m_nka->getCalibs()[layer];
      calib->parse();
    }

  }

  NetKinectArray*
  KinectSurface::getNetKinectArray(){
    return m_nka;
  }

  bool
  KinectSurface::isPhoto(){
    return m_nka->isPhoto();
  }

}
