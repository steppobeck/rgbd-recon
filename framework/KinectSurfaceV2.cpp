#include "KinectSurfaceV2.h"


#include <Shader.h>
#include <UniformSet.h>
#include <Obj.h>

#include <NetKinectArray.h>
#include <ARTListener.h>
#include <ProxyMeshGridV2.h>
#include <KinectCalibrationFile.h>
#include <CalibVolume.h>

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


  KinectSurfaceV2::KinectSurfaceV2(const char* config)
    : m_config(config),
      m_nka(0),
      m_shaderWarp(0),
      m_uniformsWarp(0),
      m_proxyMesh(0),
      m_proxyMesh_low(0),
      m_hostname(),
      m_shader(0),
      m_uniforms(0),
      m_obj(0),
      m_cv(0),
      lookup(true),
      do_capture(false),
      do_apply(false),
      do_clear(false)
  {
    init(config);
  }


  KinectSurfaceV2::~KinectSurfaceV2(){
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
  KinectSurfaceV2::draw(bool update, float scale, bool warpvis){

   

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

      m_uniformsWarp->set_int("cv_xyz",2);
      m_uniformsWarp->set_int("cv_uv",3);


      m_uniformsWarp->set_float("cv_min_d",m_cv->m_cv_min_ds[layer]);
      m_uniformsWarp->set_float("cv_max_d",m_cv->m_cv_max_ds[layer]);


      m_uniformsWarp->set_float("cx_d",calib->getDepthPrincipalPoint().u);
      m_uniformsWarp->set_float("cy_d",calib->getDepthPrincipalPoint().v);
      
      m_uniformsWarp->set_float("fx_d",calib->getDepthFocalLength().u);
      m_uniformsWarp->set_float("fy_d",calib->getDepthFocalLength().v);
      
      m_uniformsWarp->set_float("cx_rgb",calib->getColorPrincipalPoint().u);
      m_uniformsWarp->set_float("cy_rgb",calib->getColorPrincipalPoint().v);
      
      m_uniformsWarp->set_float("fx_rgb",calib->getColorFocalLength().u);
      m_uniformsWarp->set_float("fy_rgb",calib->getColorFocalLength().v);
      m_uniformsWarp->set_int("widthc",calib->getWidthC());
      m_uniformsWarp->set_int("heightc",calib->getHeightC());
      m_uniformsWarp->set_int("widthd",calib->getWidth());
      m_uniformsWarp->set_int("heightd",calib->getHeight());
      
      m_uniformsWarp->set_int("dolookup",lookup);
      

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

	  glActiveTexture(GL_TEXTURE0 + 2);
	  glBindTexture(GL_TEXTURE_3D,m_cv->m_cv_xyz_ids[layer]);
	  glActiveTexture(GL_TEXTURE0 + 3);
	  glBindTexture(GL_TEXTURE_3D,m_cv->m_cv_uv_ids[layer]);
	  

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


    m_cv->drawChessboardPoints(do_capture);
    do_capture = false;

    if(do_apply)
      m_cv->applySamples();
    do_apply = false;

    if(do_clear)
      m_cv->clearSamples();
    do_clear = false;

  }

  void
  KinectSurfaceV2::drawMesh(bool update, float scale){

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
  KinectSurfaceV2::init(const char* config){

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

    m_proxyMesh = new mvt::ProxyMeshGridV2(m_nka->getWidth(),
					 m_nka->getHeight());

    m_proxyMesh_low = new mvt::ProxyMeshGridV2(48,
					       36);

    m_cv = new CalibVolume(m_nka->getCalibs(), 0/*m_nka->getARTL()*/);
    m_cv->reload();
    m_cv->findSamples();
    
#if 0
    for(unsigned layer = 0; layer < m_nka->getNumLayers(); ++layer){
      // load m_cv_xyz_id
      unsigned cv_width  = 128;
      unsigned cv_height = 128;
      unsigned cv_depth  = 128;
      
      float    cv_min_d  = 0.5;
      float    cv_max_d  = 4.5;
      
      
      KinectCalibrationFile* calib = m_nka->getCalibs()[layer];
      std::string basefile = calib->_filePath;
      basefile.replace( basefile.end() - 3, basefile.end(), "");
      
      FILE* f_xyz = fopen( (basefile + "cv_xyz").c_str(), "rb");
      fread(&cv_width, sizeof(unsigned), 1, f_xyz);
      fread(&cv_height, sizeof(unsigned), 1, f_xyz);
      fread(&cv_depth, sizeof(unsigned), 1, f_xyz);
      fread(&cv_min_d, sizeof(float), 1, f_xyz);
      fread(&cv_max_d, sizeof(float), 1, f_xyz);
      xyz* cv_xyz = new xyz[cv_width * cv_height * cv_depth];
      fread(cv_xyz, sizeof(xyz), cv_width * cv_height * cv_depth, f_xyz);
      fclose(f_xyz);
      
      FILE* f = fopen( (basefile + "cv_uv").c_str(), "rb");
      fread(&cv_width, sizeof(unsigned), 1, f);
      fread(&cv_height, sizeof(unsigned), 1, f);
      fread(&cv_depth, sizeof(unsigned), 1, f);
      fread(&cv_min_d, sizeof(float), 1, f);
      fread(&cv_max_d, sizeof(float), 1, f);
      uv*  cv_uv  = new uv [cv_width * cv_height * cv_depth];
      fread(cv_uv, sizeof(uv), cv_width * cv_height * cv_depth, f);
      fclose(f);

      {
	unsigned int texname;
	glGenTextures(1, &texname);
	glBindTexture(GL_TEXTURE_3D, texname);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB32F, cv_width, cv_height, cv_depth, 0, GL_RGB, GL_FLOAT, (unsigned char*) cv_xyz);
	m_cv_xyz_ids.push_back(texname);
      }
      {
	unsigned int texname;
	glGenTextures(1, &texname);
	glBindTexture(GL_TEXTURE_3D, texname);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RG32F, cv_width, cv_height, cv_depth, 0, GL_RG, GL_FLOAT, (unsigned char*) cv_uv);
	m_cv_uv_ids.push_back(texname);
      }    


      delete [] cv_xyz;
      delete [] cv_uv;

    }
#endif
  }

  void
  KinectSurfaceV2::reloadShader(){
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

    m_cv->reload();

  }

  NetKinectArray*
  KinectSurfaceV2::getNetKinectArray(){
    return m_nka;
  }



  bool
  KinectSurfaceV2::isPhoto(){
    return m_nka->isPhoto();
  }


  void
  KinectSurfaceV2::toggleCalibMode(){
    m_cv->calib_mode = !m_cv->calib_mode;
    std::cerr << "calib_mode = " << m_cv->calib_mode << std::endl;
  }

  void
  KinectSurfaceV2::saveCalibVolumes(){
    m_cv->save();
  }

  void
  KinectSurfaceV2::saveSamplePoints(const char* bfname, unsigned stride){
    m_cv->saveSamplePoints(bfname, stride);
  }


  void
  KinectSurfaceV2::dumpCalibVolumesSamplingPoints(){
    m_cv->dumpSamplePoints();
  }

}
