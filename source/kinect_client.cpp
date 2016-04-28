#include <CMDParser.h>
#include <screen_space_measurement_tool.hpp>

/// c++ includes
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <memory>


#include <PerspectiveCamera.h>
#include <CameraNavigator.h>
#include <FourTiledWindow.h>
#include <CalibVolume.h>
#include <KinectSurfaceV3.h>
#include <NetKinectArray.h>
#include <KinectCalibrationFile.h>
#include <Statistics.h>
#include <GlPrimitives.h>


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

std::unique_ptr<gloost::PerspectiveCamera>   g_camera{};
std::unique_ptr<pmd::CameraNavigator> g_navi{};
std::unique_ptr<mvt::FourTiledWindow> g_ftw{};
std::unique_ptr<mvt::Statistics> g_stats{};
std::unique_ptr<ScreenSpaceMeasureTool> g_ssmt{};

void init(std::vector<std::string>& args);
void draw3d(void);
void resize(int width, int height);
void key(unsigned char key, int x, int y);
void motionFunc(int mouse_h, int mouse_v);
void mouseFunc(int button, int state, int mouse_h, int mouse_v);
void idle(void);

std::vector<kinect::KinectSurfaceV3* > g_ksV3;// 4
unsigned g_ks_mode = 0;

bool g_picking = false;

void init(std::vector<std::string> args){

  g_camera = std::unique_ptr<gloost::PerspectiveCamera>{new gloost::PerspectiveCamera(50.0,
					   g_aspect,
                                           0.1,
                                           200.0)};

  g_navi = std::unique_ptr<pmd::CameraNavigator>{new pmd::CameraNavigator(0.1f)};

  g_ftw = std::unique_ptr<mvt::FourTiledWindow>{new mvt::FourTiledWindow(g_screenWidth, g_screenHeight)};
  g_stats = std::unique_ptr<mvt::Statistics>{new mvt::Statistics};
  g_ssmt = std::unique_ptr<ScreenSpaceMeasureTool>{new ScreenSpaceMeasureTool(g_camera.get(), g_screenWidth, g_screenHeight)};

  for(unsigned i = 0; i < args.size(); ++i){
    const std::string ext(args[i].substr(args[i].find_last_of(".") + 1));
    std::cerr << ext << std::endl;
   if("ksV3" == ext){
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
    for(unsigned i = 0; i < g_ksV3.size(); ++i){
      if(g_play)
        g_ksV3[i]->getNetKinectArray()->update();
    }
  }
  else if(g_ks_mode == 0){
    gloost::Matrix t;
    t.setIdentity();
    g_stats->setInfoSlot("z-buffer-based fusion", 0);
    for(unsigned i = 0; i < g_ksV3.size(); ++i){
      if(g_play)
        g_ksV3[i]->getNetKinectArray()->update();
    }
  }
  else if(g_ks_mode == 2){
    gloost::Matrix t;
    t.setIdentity();
    g_stats->setInfoSlot("quality-based accumulation", 0);
    for(unsigned i = 0; i < g_ksV3.size(); ++i){
      if(g_play)
        g_ksV3[i]->getNetKinectArray()->update();
    }
  }
  else if(g_ks_mode == 3){
    gloost::Matrix t;
    t.setIdentity();
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
  mvt::GlPrimitives::get()->drawLineSegments(g_ssmt->getMeasurePoints());

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
    for(unsigned i = 0; i < g_ksV3.size(); ++i){
      g_ksV3[i]->lookup = (int) !g_ksV3[i]->lookup;
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

  return EXIT_SUCCESS;
}


