#include "KinectSurface3.h"


#include <Shader.h>
#include <UniformSet.h>
#include <Obj.h>

#include <NetKinectArray.h>
#include <ARTListener.h>
#include <ProxyMeshGrid.h>
#include <KinectCalibrationFile.h>
#include <ViewArray.h>

#include <FileValue.h>

#include <gl_util.h>

#include <algorithm>
#include <unistd.h>

namespace kinect{


  KinectSurface3::KinectSurface3(const char* config)
    : m_config(config),
      m_nka(0),
      m_shader_pass_depth(0),
      m_shader_pass_accum(0),
      m_shader_pass_normalize(0),
      m_uniforms_pass_depth(0),
      m_uniforms_pass_accum(0),
      m_uniforms_pass_normalize(0),
      m_proxyMesh(0),
      m_va_pass_depth(0),
      m_va_pass_accum(0),
      m_hostname(),
      m_shader(0),
      m_uniforms(0),
      m_obj(0)
  {
    init(config);
  }


  KinectSurface3::~KinectSurface3(){
    delete m_nka;
    delete m_shader_pass_depth;
    delete m_shader_pass_accum;
    delete m_shader_pass_normalize;
    delete m_uniforms_pass_depth;
    delete m_uniforms_pass_accum;
    delete m_uniforms_pass_normalize;

    delete m_proxyMesh;
    delete m_va_pass_depth;
    delete m_va_pass_accum;
  }

  void
  KinectSurface3::draw(bool update, float scale){

    if(m_nka->isPhoto()){
      drawMesh(update,scale);
      return;
    }


    if(0 == m_nka)
	return;

    if(update)
      m_nka->update();

#if 0
    if("kerberos" == m_hostname && std::string::npos != m_config.find("DLP") && scale > 0.99)
       return;
    if("pandora" == m_hostname  && std::string::npos != m_config.find("LCD") && scale > 0.99)
       return;
#endif


    // calculate img_to_eye for this view
    gloost::Matrix projection_matrix;
    glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix.data());
    gloost::Matrix viewport_translate;
    viewport_translate.setIdentity();
    viewport_translate.setTranslate(1.0,1.0,1.0);
    gloost::Matrix viewport_scale;
    viewport_scale.setIdentity();
    unsigned width  = 0;
    unsigned height = 0;
    getWidthHeight(width, height);
    viewport_scale.setScale(width * 0.5, height * 0.5, 0.5f);
    gloost::Matrix image_to_eye =  viewport_scale * viewport_translate * projection_matrix;
    image_to_eye.invert();


    const float min_length = m_nka->getCalibs()[0]->min_length/*0.0125*/ * scale;

    unsigned ox;
    unsigned oy;


    // pass 1 goes to depth buffer only
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    m_va_pass_depth->enable(0, false, &ox, &oy, false);
    m_uniforms_pass_depth->set_float("min_length", min_length);
    m_nka->bindToTextureUnits(GL_TEXTURE0);
    for(unsigned layer = 0; layer < m_nka->getNumLayers(); ++layer){

      KinectCalibrationFile* calib = m_nka->getCalibs()[layer];
      calib->updateMatrices();


      // now we do first: frustum culling
      if(calib->frustCull())
	continue;

      m_uniforms_pass_depth->set_mat4("image_d_to_eye_d", calib->image_d_to_eye_d);
      if(calib->isSensored()){
	m_uniforms_pass_depth->set_mat4("eye_d_to_world", m_nka->current_poses[layer]);
      }
      else{
	m_uniforms_pass_depth->set_mat4("eye_d_to_world", calib->eye_d_to_world);	
      }

      m_uniforms_pass_depth->set_mat4("eye_d_to_eye_rgb", calib->eye_d_to_eye_rgb);
      m_uniforms_pass_depth->set_mat4("eye_rgb_to_image_rgb", calib->eye_rgb_to_image_rgb);
      m_uniforms_pass_depth->set_int("layer",  layer);
      m_uniforms_pass_depth->set_vec2("texSizeInv", gloost::vec2(1.0f/calib->getWidth(), 1.0f/calib->getHeight()));
 

      m_uniforms_pass_depth->set_float("groundlevel", calib->s_groundlevel);

      //glPushAttrib(GL_ALL_ATTRIB_BITS);
      {
	glDisable(GL_CULL_FACE);
	glPushMatrix();
	{
	  m_shader_pass_depth->set();
	  m_uniforms_pass_depth->applyToShader(m_shader_pass_depth);
	  
	  
	  m_proxyMesh->draw(scale);
	  m_shader_pass_depth->disable();
	}
	glPopMatrix();
      }
      glActiveTexture(GL_TEXTURE0);
      //glPopAttrib();
      
    }
    m_va_pass_depth->disable(false);
    glPopAttrib();
    // end pass 1
#if 1
    // pass 2 goes to accumulation buffer
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND); // glEnablei(GL_BLEND, GL_COLOR_ATTACHMENT0_EXT);
    glBlendFuncSeparateEXT(GL_ONE,GL_ONE,GL_ONE,GL_ONE);
    glBlendEquationSeparateEXT(GL_FUNC_ADD, GL_FUNC_ADD);
    m_va_pass_accum->enable(0, false, &ox, &oy);
    m_uniforms_pass_accum->set_float("min_length", min_length);
    m_uniforms_pass_accum->set_vec2("viewportSizeInv", gloost::vec2(1.0f/m_va_pass_depth->getWidth(), 1.0f/m_va_pass_depth->getHeight()));
    m_uniforms_pass_accum->set_vec2("offset"         , gloost::vec2(1.0f*ox,                          1.0f*oy));
    m_uniforms_pass_accum->set_mat4("img_to_eye_curr", image_to_eye);
    m_uniforms_pass_accum->set_float("epsilon"    , scale * 0.075);
    m_nka->bindToTextureUnits(GL_TEXTURE0);
    m_va_pass_depth->bindToTextureUnitsDepth(GL_TEXTURE0 + 2);

    for(unsigned layer = 0; layer < m_nka->getNumLayers(); ++layer){

      KinectCalibrationFile* calib = m_nka->getCalibs()[layer];
      calib->updateMatrices();


      // now we do first: frustum culling
      if(calib->frustCull())
	continue;

      m_uniforms_pass_accum->set_mat4("image_d_to_eye_d", calib->image_d_to_eye_d);
      if(calib->isSensored()){
	m_uniforms_pass_accum->set_mat4("eye_d_to_world", m_nka->current_poses[layer]);
      }
      else{
	m_uniforms_pass_accum->set_mat4("eye_d_to_world", calib->eye_d_to_world);	
      }

      m_uniforms_pass_accum->set_mat4("eye_d_to_eye_rgb", calib->eye_d_to_eye_rgb);
      m_uniforms_pass_accum->set_mat4("eye_rgb_to_image_rgb", calib->eye_rgb_to_image_rgb);
      m_uniforms_pass_accum->set_int("layer",  layer);
      m_uniforms_pass_accum->set_vec2("texSizeInv", gloost::vec2(1.0f/calib->getWidth(), 1.0f/calib->getHeight()));
 
      m_uniforms_pass_accum->set_float("groundlevel", calib->s_groundlevel);

      //glPushAttrib(GL_ALL_ATTRIB_BITS);
      {
	glDisable(GL_CULL_FACE);
	glPushMatrix();
	{
	  m_shader_pass_accum->set();
	  m_uniforms_pass_accum->applyToShader(m_shader_pass_accum);
	  

	  m_proxyMesh->draw(scale);
	  m_shader_pass_accum->disable();
	}
	glPopMatrix();
      }
      
      //glPopAttrib();
      
    }
    m_va_pass_accum->disable(false);
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_BLEND);
    glPopAttrib();
    // end pass 2
#endif


#if 1
    // normalize pass outputs best quality color and depth to framebuffer of parent renderstage
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0,1.0,0.0,1.0,1.0,-1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    


    m_shader_pass_normalize->set();

    
    m_uniforms_pass_normalize->set_vec2("texSizeInv", gloost::vec2(1.0f/m_va_pass_depth->getWidth(), 1.0f/m_va_pass_depth->getHeight()));
    m_uniforms_pass_normalize->set_vec2("offset"    , gloost::vec2(1.0f*ox,                          1.0f*oy));
    
    m_uniforms_pass_normalize->applyToShader(m_shader_pass_normalize);

    m_va_pass_accum->bindToTextureUnitsRGBA(GL_TEXTURE0);
    m_va_pass_depth->bindToTextureUnitsDepth(GL_TEXTURE0 + 1);

    glBegin(GL_QUADS);
    {
      glVertex3f  (0.0f, 0.0f, 0.0f);
      glVertex3f  (1.0f, 0.0f, 0.0f);
      glVertex3f  (1.0f, 1.0f, 0.0f);
      glVertex3f  (0.0f, 1.0f, 0.0f);
    }
    glEnd();

    m_shader_pass_normalize->disable();
    
    glActiveTexture(GL_TEXTURE0);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glPopAttrib();
    // end pass normalize
#endif


    m_nka->drawGeometry();

  }



  void
  KinectSurface3::drawMesh(bool update, float scale){

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
  KinectSurface3::init(const char* config){

    KinectCalibrationFile::s_use_sensor = false;
    //KinectCalibrationFile::s_compress   = true;

    char hostname[1024];
    gethostname(hostname, 1024);
    m_hostname = hostname;

    // parse the file .ks and find the shader entry to get the names.
    std::vector<std::string> hostnames;
    std::ifstream in(config);
    std::string token;
    while(in >> token){
      if(token == "hostname"){
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
    m_proxyMesh = new mvt::ProxyMeshGrid(m_nka->getWidth(),
					 m_nka->getHeight());

    m_uniforms_pass_depth = new gloost::UniformSet;
    m_uniforms_pass_depth->set_int("kinect_colors",0);
    m_uniforms_pass_depth->set_int("kinect_depths",1);


    m_uniforms_pass_accum = new gloost::UniformSet;
    m_uniforms_pass_accum->set_int("kinect_colors",0);
    m_uniforms_pass_accum->set_int("kinect_depths",1);
    m_uniforms_pass_accum->set_int("depth_map_curr",2);

    m_uniforms_pass_normalize = new gloost::UniformSet;
    m_uniforms_pass_normalize->set_int("color_map",0);
    m_uniforms_pass_normalize->set_int("depth_map",1);

    reloadShader();
    
    m_va_pass_depth = new mvt::ViewArray(1920,1200, 1);
    m_va_pass_depth->init();

    m_va_pass_accum = new mvt::ViewArray(1920,1200, 1);
    m_va_pass_accum->init();

  }

  void
  KinectSurface3::reloadShader(){
    if(m_shader_pass_depth)
      delete m_shader_pass_depth;

    m_shader_pass_depth = new gloost::Shader("glsl/pass_depth.vs",
					     "glsl/pass_depth.fs",
					     "glsl/pass_depth.gs");
    m_shader_pass_depth->setProgramParameter(GL_GEOMETRY_INPUT_TYPE_EXT ,GL_TRIANGLES);
    m_shader_pass_depth->setProgramParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT ,GL_TRIANGLE_STRIP);
    m_shader_pass_depth->setProgramParameter(GL_GEOMETRY_VERTICES_OUT_EXT ,3);

    if(m_shader_pass_accum)
      delete m_shader_pass_accum;

    m_shader_pass_accum = new gloost::Shader("glsl/pass_accum.vs",
					     "glsl/pass_accum.fs",
					     "glsl/pass_accum.gs");
    m_shader_pass_accum->setProgramParameter(GL_GEOMETRY_INPUT_TYPE_EXT ,GL_TRIANGLES);
    m_shader_pass_accum->setProgramParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT ,GL_TRIANGLE_STRIP);
    m_shader_pass_accum->setProgramParameter(GL_GEOMETRY_VERTICES_OUT_EXT ,3);


    if(m_shader_pass_normalize)
      delete m_shader_pass_normalize;
    m_shader_pass_normalize = new gloost::Shader("glsl/pass_normalize.vs",
						 "glsl/pass_normalize.fs");


    m_nka->reloadShader();


    std::vector<kinect::KinectCalibrationFile*>& calibs = m_nka->getCalibs();
    for(unsigned i = 0; i < calibs.size(); ++i){
      kinect::KinectCalibrationFile* v = calibs[i];
      v->parse();
    }

  }

  NetKinectArray*
  KinectSurface3::getNetKinectArray(){
    return m_nka;
  }
  void

  KinectSurface3::getWidthHeight(unsigned& width, unsigned& height){
    GLsizei vp_params[4];
    glGetIntegerv(GL_VIEWPORT,vp_params);
    width  = vp_params[2];
    height = vp_params[3];
  }



  bool
  KinectSurface3::isPhoto(){
    return m_nka->isPhoto();
  }

}
