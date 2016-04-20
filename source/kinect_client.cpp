#include <CMDParser.h>

/// c++ includes
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <stdlib.h>


#include <PerspectiveCamera.h>
#include <CameraNavigator.h>
#include <FourTiledWindow.h>
#include <KinectSurface.h>
#include <KinectSurface3.h>
#include <KinectSurfaceV2.h>
#include <CalibVolume.h>
#include <KinectSurfaceV3.h>
#include <NetKinectArray.h>
#include <KinectCalibrationFile.h>
#include <Statistics.h>
#include <gl_util.h>
#include <GlPrimitives.h>
#include <gloostMath.h>
#include <gloostHelper.h>

/// general setup
unsigned int g_screenWidth  = 1280;
unsigned int g_screenHeight = 720;
float        g_aspect       = g_screenWidth * 1.0/g_screenHeight;
unsigned int g_frameCounter = 0;
float        g_scale        = 1.0f;
bool         g_info         = false;
bool         g_play         = true;
bool         g_reference    = false;
bool         g_warpviz      = false;
bool         g_animate      = false;
bool         g_wire         = false;
bool         g_bfilter      = true;
bool         g_black        = false;
gloost::PerspectiveCamera*   g_camera = 0;
pmd::CameraNavigator* g_navi = 0;
mvt::FourTiledWindow* g_ftw = 0;
mvt::Statistics* g_stats = 0;

void init(std::vector<std::string>& args);
void draw3d(void);
void cleanup();
void resize(int width, int height);
void key(unsigned char key, int x, int y);
void motionFunc(int mouse_h, int mouse_v);
void mouseFunc(int button, int state, int mouse_h, int mouse_v);
void idle(void);

std::vector<kinect::KinectSurface* >  g_ks; // 0
std::vector<kinect::KinectSurface* > g_ks2;// 1
std::vector<kinect::KinectSurface3* > g_ks3;// 2
std::vector<kinect::KinectSurfaceV2* > g_ksV2;// 3
std::vector<kinect::KinectSurfaceV3* > g_ksV3;// 4
unsigned g_ks_mode = 0;



struct pickpos{
public:
  pickpos(unsigned a, unsigned b)
    : x(a),
      y(b)
  {}
  unsigned x;
  unsigned y;

};

gloost::Point3
multH(gloost::Matrix const& m,
      gloost::Point3 const& p )
{
  gloost::Point3 tmp(m[0]*p[0] + m[4]*p[1] + m[8] *p[2] + m[12] *p[3],
		     m[1]*p[0] + m[5]*p[1] + m[9] *p[2] + m[13] *p[3],
		     m[2]*p[0] + m[6]*p[1] + m[10]*p[2] + m[14] *p[3]);
  tmp[3] =           m[3]*p[0] + m[7]*p[1] + m[11]*p[2] + m[15] *p[3];
  return tmp;
}


class LineSegmentRenderer{
public:
  LineSegmentRenderer()
  {}
  ~LineSegmentRenderer()
  {}

  void draw(std::vector<gloost::Point3> p){
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glColor3f(0.0,1.0,0.0);
    glPointSize(3.0);

    glBegin(GL_POINTS);
    for(unsigned i = 0; i < p.size(); ++i){
      glVertex3fv(p[i].data());
    }
    glEnd();
    glLineWidth(2.0);
    glBegin(GL_LINE_STRIP);
    for(unsigned i = 0; i < p.size(); ++i){
      glVertex3fv(p[i].data());
    }
    glEnd();


    glPopAttrib();
  }

private:

};

LineSegmentRenderer* g_lsr = 0;

class ScreenSpaceMeasureTool{

public:

  ScreenSpaceMeasureTool(gloost::Camera* cam, unsigned w, unsigned h)
    : m_cam(cam),
      m_w(w),
      m_h(h),
      m_pp(),
      m_mp(),
      m_modelview()
  {}

  ~ScreenSpaceMeasureTool()
  {}

  void resize(unsigned w, unsigned h){
    m_w = w;
    m_h = h;
  }

  void mouse(int button, int state, int mouse_h, int mouse_v){
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
      m_pp.clear();
      pickpos pp(mouse_h, mouse_v);
      m_pp.push_back(pp);


      float d = pickDepth(m_pp[0]);

      // calculate img_to_eye for this view
      gloost::Matrix viewport_translate;
      viewport_translate.setIdentity();
      viewport_translate.setTranslate(1.0,1.0,1.0);
      gloost::Matrix viewport_scale;
      viewport_scale.setIdentity();
      viewport_scale.setScale(m_w * 0.5, m_h * 0.5, 0.5f);
      gloost::Matrix image_to_eye =  viewport_scale * viewport_translate * m_cam->getProjectionMatrix() * m_modelview;
      image_to_eye.invert();
      gloost::Point3 pos_img(m_pp[0].x + 0.5, m_pp[0].y + 0.5, d);
      gloost::Point3 pos_es = multH(image_to_eye, pos_img);
      std::cerr << pos_es[3] << std::endl;
      pos_es[0] /= pos_es[3];
      pos_es[1] /= pos_es[3];
      pos_es[2] /= pos_es[3];
      pos_es[3] /= pos_es[3];
#if 0      
      std::cerr << m_cam->getProjectionMatrix() << std::endl;
      std::cerr << m_pp[0].x << " " << m_pp[0].y << ": " << d << " 3D pos: " << pos_es << std::endl;
#endif

      m_mp.push_back(pos_es);

    }
    else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
      m_mp.pop_back();
    }
    else if(button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN){
      m_mp.clear();
    }
  }

  float measure(){
    const size_t mp_count = m_mp.size();
    if(mp_count > 1){
      return (m_mp[mp_count - 1] - m_mp[mp_count - 2]).length();
    }

    return -1.0;
  }

  std::vector<gloost::Point3>& getMeasurePoints(){
    return m_mp;
  }

  void setModelView(gloost::Matrix& mv){
    m_modelview = mv;
  }

private:

  float pickDepth(const pickpos& pp){
    float d;
    glReadPixels( pp.x, pp.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &d);
    return d;
  }

  gloost::Camera*      m_cam;
  unsigned             m_w;
  unsigned             m_h;

  std::vector<pickpos> m_pp;
  std::vector<gloost::Point3> m_mp;
  gloost::Matrix m_modelview;
};

bool g_picking = false;
ScreenSpaceMeasureTool* g_ssmt = 0;

////////////////////////////////////////////////////////////////////////////////


void init(std::vector<std::string> args){

  g_camera = new gloost::PerspectiveCamera(50.0,
					   g_aspect,
                                           0.1,
                                           200.0);

  g_navi = new pmd::CameraNavigator(0.1f);

  g_ftw = new mvt::FourTiledWindow(g_screenWidth, g_screenHeight);
  g_stats = new mvt::Statistics;
  g_ssmt = new ScreenSpaceMeasureTool(g_camera, g_screenWidth, g_screenHeight);

  g_lsr = new LineSegmentRenderer;

  for(unsigned i = 0; i < args.size(); ++i){
    const std::string ext(args[i].substr(args[i].find_last_of(".") + 1));
    std::cerr << ext << std::endl;
    if("ks" == ext){
      g_ks.push_back(new kinect::KinectSurface(args[i].c_str()));
      g_ks_mode = 0;
    }
    else if("ks2" == ext){
      g_ks2.push_back(new kinect::KinectSurface(args[i].c_str()));
      g_ks_mode = 1;
    }
    else if("ks3" == ext){
      g_ks3.push_back(new kinect::KinectSurface3(args[i].c_str()));
      g_ks_mode = 2;
    }
    else if("ksV2" == ext){
      g_ksV2.push_back(new kinect::KinectSurfaceV2(args[i].c_str()));
      g_ks_mode = 3;
    }
    else if("ksV3" == ext){
      g_ksV3.push_back(new kinect::KinectSurfaceV3(args[i].c_str()));
      g_ks_mode = 4;
    }
  }

}

//////////////////////////////////////////////////////////////////////////////////////////


  /// logic

void frameStep (){

  /// increment the frame counter
  ++g_frameCounter;

  
  glClearColor(g_black ? 0 : 1, g_black ? 0 : 1, g_black ? 0 : 1, 0);
  glViewport(0,0,g_screenWidth, g_screenHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  
  draw3d();
  

  g_ftw->endFrame();

  if(g_info)
    g_stats->draw(g_screenWidth, g_screenHeight);


  //std::cerr << g_frameCounter << std::endl;
  glutSwapBuffers();
}


//////////////////////////////////////////////////////////////////////////////////////////


  /// main loop function, render with 3D setup

void draw3d(void)
{

  g_camera->setAspect(g_aspect);
  g_camera->set();
  

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gloost::Point3 speed(0.0,0.0,0.0);

  gloost::vec2 speed_button1(g_ftw->getButtonSpeed(1));
  gloost::vec2 speed_button2(g_ftw->getButtonSpeed(2));
  float fac = 0.005;
  speed[0] = speed_button1.u * fac;
  speed[1] = speed_button1.v * - 1.0 * fac;
  speed[2] = speed_button2.v * fac;

  gloost::Matrix camview(g_navi->get(speed));
  camview.invert();

  gloostMultMatrix(camview.data());

  glScalef(g_scale, g_scale, g_scale);

  if(g_animate){
    static unsigned g_framecounta = 0;
    ++g_framecounta;
    glRotatef(g_framecounta * 1.0, 0.0,1.0,0.0);
  }

  if(g_wire){
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  }
  else{
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  }

  gloost::Matrix modelview;
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview.data());
  g_ssmt->setModelView(modelview);
  gloost::Vector3 test_vec(1.0,0.0,0.0);
  test_vec = modelview * test_vec;
  float scale = test_vec.length();

  g_stats->startGPU();

#if 0
  g_stats->setInfoSlot("comparison: [with calibration (above red x-axis)] vs. [without calibration]", 0);

  gloost::Matrix t;
  t.setIdentity();

  glPushMatrix();
  t.setTranslate(-0.5, 0.0,0.0);
  glMultMatrixf(t.data());
  g_ksV3[0]->lookup = 0;
  g_ksV3[0]->draw(g_play, scale);
  glPopMatrix();

  glPushMatrix();
  t.setTranslate(0.5, 0.0,0.0);
  glMultMatrixf(t.data());
  g_ksV3[0]->lookup = true;
  g_ksV3[0]->draw(false, scale);
  glPopMatrix();


  glPushAttrib(GL_ALL_ATTRIB_BITS);
  mvt::GlPrimitives::get()->drawCoords();
  glPopAttrib();

#endif

#if 1
  if(g_ks_mode == 1){
    gloost::Matrix t;
    t.setIdentity();
    g_stats->setInfoSlot("quality-based fusion", 0);
    for(unsigned i = 0; i < g_ks2.size(); ++i){
      glPushMatrix();
      t.setTranslate(i, 0.0,0.0);
      glMultMatrixf(t.data());
      
      g_ks2[i]->draw(g_play, scale);
      glPopMatrix();
    }
    for(unsigned i = 0; i < g_ks.size(); ++i){
      if(g_play)
        g_ks[i]->getNetKinectArray()->update();
    }
    for(unsigned i = 0; i < g_ks3.size(); ++i){
      if(g_play)
        g_ks3[i]->getNetKinectArray()->update();
    }
    for(unsigned i = 0; i < g_ksV2.size(); ++i){
      if(g_play)
        g_ksV2[i]->getNetKinectArray()->update();
    }
    for(unsigned i = 0; i < g_ksV3.size(); ++i){
      if(g_play)
        g_ksV3[i]->getNetKinectArray()->update();
    }
  }
  else if(g_ks_mode == 0){
    gloost::Matrix t;
    t.setIdentity();
    g_stats->setInfoSlot("z-buffer-based fusion", 0);
    for(unsigned i = 0; i < g_ks.size(); ++i){
      glPushMatrix();
      t.setTranslate(i, 0.0,0.0);
      glMultMatrixf(t.data());
      g_ks[i]->draw(g_play, scale, g_warpviz);
      glPopMatrix();
    }
    for(unsigned i = 0; i < g_ks2.size(); ++i){
      if(g_play)
        g_ks2[i]->getNetKinectArray()->update();
    }
    for(unsigned i = 0; i < g_ks3.size(); ++i){
      if(g_play)
        g_ks3[i]->getNetKinectArray()->update();
    }
    for(unsigned i = 0; i < g_ksV2.size(); ++i){
      if(g_play)
        g_ksV2[i]->getNetKinectArray()->update();
    }
    for(unsigned i = 0; i < g_ksV3.size(); ++i){
      if(g_play)
        g_ksV3[i]->getNetKinectArray()->update();
    }
  }
  else if(g_ks_mode == 2){
    gloost::Matrix t;
    t.setIdentity();
    g_stats->setInfoSlot("quality-based accumulation", 0);
    for(unsigned i = 0; i < g_ks3.size(); ++i){
      glPushMatrix();
      t.setTranslate(i, 0.0,0.0);
      glMultMatrixf(t.data());
      
      g_ks3[i]->draw(g_play, scale);
      glPopMatrix();
    }
    for(unsigned i = 0; i < g_ks.size(); ++i){
      if(g_play)
        g_ks[i]->getNetKinectArray()->update();
    }
    for(unsigned i = 0; i < g_ks2.size(); ++i){
      if(g_play)
        g_ks2[i]->getNetKinectArray()->update();
    }
    for(unsigned i = 0; i < g_ksV2.size(); ++i){
      if(g_play)
        g_ksV2[i]->getNetKinectArray()->update();
    }
    for(unsigned i = 0; i < g_ksV3.size(); ++i){
      if(g_play)
        g_ksV3[i]->getNetKinectArray()->update();
    }
  }
  else if(g_ks_mode == 3){
    gloost::Matrix t;
    t.setIdentity();
    g_ksV2[0]->lookup ? g_stats->setInfoSlot("Volume Based Mapping", 0) : g_stats->setInfoSlot("Explicit Mapping", 0);    
    for(unsigned i = 0; i < g_ksV2.size(); ++i){

      glPushMatrix();
      t.setTranslate(i, 0.0,0.0);
      glMultMatrixf(t.data());
      
      g_ksV2[i]->draw(g_play, scale);
      glPopMatrix();
    }
    for(unsigned i = 0; i < g_ks.size(); ++i){
      if(g_play)
        g_ks[i]->getNetKinectArray()->update();
    }
    for(unsigned i = 0; i < g_ks2.size(); ++i){
      if(g_play)
        g_ks2[i]->getNetKinectArray()->update();
    }
    for(unsigned i = 0; i < g_ks3.size(); ++i){
      if(g_play)
        g_ks3[i]->getNetKinectArray()->update();
    }
    for(unsigned i = 0; i < g_ksV3.size(); ++i){
      if(g_play)
        g_ksV3[i]->getNetKinectArray()->update();
    }
  }
  else if(g_ks_mode == 4){
    gloost::Matrix t;
    t.setIdentity();
    g_ksV3[0]->lookup ? g_stats->setInfoSlot("Volume Based Mapping", 0) : g_stats->setInfoSlot("Explicit Mapping", 0);
    //g_stats->setInfoSlot(("compression ratio " + gloost::toString(g_ksV3[0]->getNetKinectArray()->depth_compression_ratio)).c_str(), 1);
    for(unsigned i = 0; i < g_ksV3.size(); ++i){

      glPushMatrix();
      t.setTranslate(i, 0.0,0.0);
      glMultMatrixf(t.data());
      
      g_ksV3[i]->draw(g_play, scale);


      glPopMatrix();
    }
    for(unsigned i = 0; i < g_ks.size(); ++i){
      if(g_play)
        g_ks[i]->getNetKinectArray()->update();
    }
    for(unsigned i = 0; i < g_ks2.size(); ++i){
      if(g_play)
        g_ks2[i]->getNetKinectArray()->update();
    }
    for(unsigned i = 0; i < g_ks3.size(); ++i){
      if(g_play)
        g_ks3[i]->getNetKinectArray()->update();
    }
    for(unsigned i = 0; i < g_ksV2.size(); ++i){
      if(g_play)
        g_ksV2[i]->getNetKinectArray()->update();
    }
  }

#endif

  g_stats->stopGPU();
  //std::cerr << "after stopGPU" << std::endl; check_gl_errors("after stopGPU", false);


  if(g_picking){
    float dist = g_ssmt->measure();
    g_stats->setInfoSlot(("measuring: " + gloost::toString(dist)).c_str(), 1);
  }
  else{
    g_stats->setInfoSlot("navigation mode", 1);
  }
  g_lsr->draw(g_ssmt->getMeasurePoints());






  if(g_ksV2.size() == 1 && g_reference){
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    //glDisable(GL_DEPTH_TEST);
    mvt::GlPrimitives::get()->drawCoords();
    //glEnable(GL_DEPTH_TEST);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    
    mvt::CameraView* v = (mvt::CameraView*) g_ksV2[0]->getNetKinectArray()->getCalibs()[0];
    v->updateMatrices();
      
    glPushMatrix();
    gloostMultMatrix(v->eye_d_to_world.data());
    glColor4f(0.0,0.0,0.0,1.0);
    v->drawFrustum();
    glPopMatrix();
    /*  
    glPushMatrix();
    gloostMultMatrix(v->eye_rgb_to_world.data());
    glColor4f(0.0,1.0,0.0,0.0);
    v->drawFrustumColor();
    glPopMatrix();
    */
    
    glPopAttrib();
  }

  if(g_reference && g_ksV3.size()){
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    

    //glDisable(GL_DEPTH_TEST);
    mvt::GlPrimitives::get()->drawCoords();
    

#if 0
    for(unsigned idx = 0; idx  < g_ksV3[0]->getNetKinectArray()->getCalibs().size(); ++idx){
      
    
      glPushAttrib(GL_ALL_ATTRIB_BITS);
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      
      mvt::CameraView* v = (mvt::CameraView*) g_ksV3[0]->getNetKinectArray()->getCalibs()[idx];
      v->updateMatrices();
      
      glPushMatrix();
      gloostMultMatrix(v->eye_d_to_world.data());
      glColor4f(0.0,0.0,0.0,1.0);
      v->drawFrustum();
      glPopMatrix();
      /*  
	  glPushMatrix();
	  gloostMultMatrix(v->eye_rgb_to_world.data());
	  glColor4f(0.0,1.0,0.0,0.0);
	  v->drawFrustumColor();
	  glPopMatrix();
      */
      glPopAttrib();
    }
#endif
    glPopAttrib();




  }

  if(g_reference && g_ksV2.size()){
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    //glDisable(GL_DEPTH_TEST);
    mvt::GlPrimitives::get()->drawCoords();

    for(unsigned idx = 0; idx  < g_ksV2[0]->getNetKinectArray()->getCalibs().size(); ++idx){
      
    
      glPushAttrib(GL_ALL_ATTRIB_BITS);
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      
      mvt::CameraView* v = (mvt::CameraView*) g_ksV2[0]->getNetKinectArray()->getCalibs()[idx];
      v->updateMatrices();
      
      glPushMatrix();
      gloostMultMatrix(v->eye_d_to_world.data());
      glColor4f(1.0,0.0,0.0,1.0);
      v->drawFrustum();
      glPopMatrix();
      /*  
	  glPushMatrix();
	  gloostMultMatrix(v->eye_rgb_to_world.data());
	  glColor4f(0.0,1.0,0.0,0.0);
	  v->drawFrustumColor();
	  glPopMatrix();
      */
      glPopAttrib();
    }
    glPopAttrib();
  }

  { // draw black grid on floor for fancy look
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_LINES);
    const float lsize = 10.0f;
    const float gstep = 0.5f;
    for(float s = -lsize; s <= lsize; s += gstep){
      glVertex3f(s, 0.0, -lsize);
      glVertex3f(s, 0.0,  lsize);

      glVertex3f(-lsize, 0.0, s);
      glVertex3f( lsize, 0.0, s);
    }

    glEnd();
    glPopAttrib();
  }


}


////////////////////////////////////////////////////////////////////////////////


  /// free all resources we created

void cleanup(){

  delete g_camera;
  delete g_navi;
  delete g_ftw;
  delete g_ssmt;
  delete g_lsr;
}


////////////////////////////////////////////////////////////////////////////////


  /// this function is triggered when the screen is resized

void resize(int width, int height){

  g_screenWidth = width;
  g_screenHeight = height;
  g_aspect       = g_screenWidth * 1.0/g_screenHeight;
  g_camera->setAspect(g_aspect);

  g_navi->resize(width, height);
  g_ftw->resize(width, height);
  g_ssmt->resize(g_screenWidth, g_screenHeight);
  glutPostRedisplay();

}


//////////////////////////////////////////////////////////////////////////////////////////


  /// this function is called by glut when a key press occured

void key(unsigned char key, int x, int y)
{

  switch (key){

  case 'b':
    g_black = !g_black;
    for(unsigned i = 0; i < g_ksV3.size(); ++i){
      g_ksV3[i]->black = g_black;
    }
    break;
  case '0':
    for(unsigned i = 0; i < g_ksV3.size(); ++i){
      g_ksV3[i]->viztype = 0;
    }
    break;
  case '1':
    for(unsigned i = 0; i < g_ksV3.size(); ++i){
      g_ksV3[i]->viztype = 1;
    }
    break;
  case '2':
    for(unsigned i = 0; i < g_ksV3.size(); ++i){
      g_ksV3[i]->viztype = 2;
    }
    break;
  case '3':
    for(unsigned i = 0; i < g_ksV3.size(); ++i){
      g_ksV3[i]->viztype = 3;
    }
    break;

    /// press ESC or q to quit
  case 27 :
  case 'q':
    cleanup();
    exit(0);
    break;
  case 'r':
    g_reference = !g_reference;
    break;

  case 'a':
    g_animate = !g_animate;
    break;
#if 0
  case 'w':
    g_warpviz = !g_warpviz;
    break;
  case 'w':
    for(unsigned i = 0; i < g_ksV2.size(); ++i){
      g_ksV2[i]->saveCalibVolumes();
      g_ksV2[i]->saveSamplePoints("samplePoints", 35 /* 7 * 5 */);
    }
    break;
#endif
  case 'w':
    g_wire = !g_wire;
    break;
  case 'd':
    for(unsigned i = 0; i < g_ksV2.size(); ++i){
      g_ksV2[i]->dumpCalibVolumesSamplingPoints();
    }
    break;


  case '+':
    
    if(g_ksV3[0]->viztype_num < (g_ksV3[0]->getNetKinectArray()->getCalibs().size() - 1))
      g_ksV3[0]->viztype_num += 1;
    std::cout << "viztype_num: "  << g_ksV3[0]->viztype_num << std::endl;

    break;
  case '-':
    if(g_ksV3[0]->viztype_num > 0)
      g_ksV3[0]->viztype_num -= 1;
    std::cout << "viztype_num: "  << g_ksV3[0]->viztype_num << std::endl;

    break;
#if 0
  case ' ':
    g_ks_mode = (g_ks_mode + 1) % 3;
    std::cerr << "g_ks_mode: " << g_ks_mode << std::endl;
    break;
#endif
  case 's':
    for(unsigned i = 0; i < g_ks2.size(); ++i){
      g_ks2[i]->reloadShader();
    }
    for(unsigned i = 0; i < g_ks.size(); ++i){
      g_ks[i]->reloadShader();
    }
    for(unsigned i = 0; i < g_ks3.size(); ++i){
      g_ks3[i]->reloadShader();
    }
    for(unsigned i = 0; i < g_ksV2.size(); ++i){
      g_ksV2[i]->reloadShader();
    }
    for(unsigned i = 0; i < g_ksV3.size(); ++i){
      g_ksV3[i]->reloadShader();
    }
    break;
  case 'm':
    g_picking = !g_picking;
    break;
  case 'p':
    g_play = !g_play;
    break;

  case 't':
    for(unsigned i = 0; i < g_ksV2.size(); ++i){
      g_ksV2[i]->lookup = (int) !g_ksV2[i]->lookup;
    }
    for(unsigned i = 0; i < g_ksV3.size(); ++i){
      g_ksV3[i]->lookup = (int) !g_ksV3[i]->lookup;
    }
    break;

  case 'i':
    for(unsigned i = 0; i < g_ksV3.size(); ++i){
      g_ksV3[i]->switchCalibVolume();
    }
    break;

  case ' ':
    for(unsigned i = 0; i < g_ksV2.size(); ++i){
      g_ksV2[i]->do_capture = true;
    }
    break;
  case 'z':
    for(unsigned i = 0; i < g_ksV2.size(); ++i){
      g_ksV2[i]->do_apply = true;
    }
    break;
  case 'c':
    for(unsigned i = 0; i < g_ksV2.size(); ++i){
      g_ksV2[i]->do_clear = true;
    }
    break;
  case 'x':
    for(unsigned i = 0; i < g_ksV2.size(); ++i){
      g_ksV2[i]->toggleCalibMode();
    }
    break;
  case'f':
    g_bfilter = !g_bfilter;
    for(unsigned i = 0; i < g_ksV3[0]->getNetKinectArray()->getCalibs().size(); ++i){
      g_ksV3[0]->getNetKinectArray()->getCalibs()[i]->use_bf = g_bfilter;
    }
    break;

  case '#':
    for(unsigned i = 0; i < g_ksV3[0]->getNetKinectArray()->getCalibs().size(); ++i){
      g_ksV3[0]->getNetKinectArray()->depth_compression_lex = !g_ksV3[0]->getNetKinectArray()->depth_compression_lex;
    }
    break;

  default:
    break;
  }

  glutPostRedisplay();
}


//////////////////////////////////////////////////////////////////////////////////////////


void motionFunc(int mouse_h, int mouse_v){
  if(g_picking){

  }
  else{
    g_navi->motion(mouse_h, mouse_v);
    g_ftw->motion(mouse_h, mouse_v);
  }
  glutPostRedisplay();
}


//////////////////////////////////////////////////////////////////////////////////////////
void passiveFunc(int x, int y){
  if(g_picking){
    
  }
  else{
    g_ftw->passive(x,y);
  }
  glutPostRedisplay();
}


//////////////////////////////////////////////////////////////////////////////////////////


void mouseFunc(int button, int state, int mouse_h, int mouse_v){

  if(g_picking){
    g_ssmt->mouse(button, state, mouse_h, g_screenHeight - mouse_v);
  }
  else{
    g_navi->mouse(button, state, mouse_h, mouse_v);
    g_ftw->mouse(button, state, mouse_h, mouse_v);
  }
  glutPostRedisplay();
}




void specialKey(int key, int x, int y){
  g_ftw->specialKey(key, x, y);
  glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////////////////////


  /// this function is triggered by glut,
  /// when nothing is left to do for this frame

void idle(void){
    glutPostRedisplay();
}


////////////////////////////////////////////////////////////////////////////////


int main(int argc, char *argv[])
{

  std::string serverendpoint("tcp://141.54.147.32:7000");

  CMDParser p("kinect_surface ...");

  p.addOpt("r",2,"resolution", "set screen resolution");
  p.addOpt("i",-1,"info", "draw info");
  p.addOpt("s",1,"serverendpoint", "set the server endpoint for calibvolume based calibration");
  p.init(argc,argv);

  if(p.isOptSet("r")){
    g_screenWidth = p.getOptsInt("r")[0];
    g_screenHeight = p.getOptsInt("r")[1];
  }

  if(p.isOptSet("i")){
    g_info = true;
  }

  if(p.isOptSet("s")){
    kinect::CalibVolume::serverendpoint = std::string("tcp://") + p.getOptsString("s")[0];
  }

  glutInit(&argc, argv);
  glutInitWindowSize(g_screenWidth, g_screenHeight);
  glutInitWindowPosition(10,10);
  glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);


  /// open the window
  glutCreateWindow(argv[0]);


  /// set glut callbacks for resizing, frameloop, keyboard input and idle
  glutReshapeFunc(resize);
  glutDisplayFunc(frameStep);
  glutKeyboardFunc(key);
  glutIdleFunc(idle);
  glutMotionFunc(motionFunc);
  glutPassiveMotionFunc(passiveFunc);
  glutMouseFunc(mouseFunc);
  glutSpecialFunc(specialKey);
  glutSetCursor(GLUT_CURSOR_CROSSHAIR);

  // initialize GLEW
  if (GLEW_OK != glewInit()){
    /// ... or die trying
    std::cout << "'glewInit()' failed." << std::endl;
    exit(0);
  }


  // set some gl states
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  
  // load and intialize stuff for our demo
  init(p.getArgs());

  
  /// start the loop (this will call display() every frame)
  glutMainLoop();


  /// cleanup all resources we created for the demo
  cleanup();

  return EXIT_SUCCESS;
}


