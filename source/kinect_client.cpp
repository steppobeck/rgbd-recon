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
#include <calibration_files.hpp>
#include <NetKinectArray.h>
#include <KinectCalibrationFile.h>
#include <Statistics.h>
#include <GlPrimitives.h>

#include <reconstruction.hpp>
#include <recon_trigrid.hpp>
#include <recon_points.hpp>

/// general setup
unsigned g_screenWidth  = 1280;
unsigned g_screenHeight = 720;
float    g_aspect       = g_screenWidth * 1.0/g_screenHeight;
unsigned g_frameCounter = 0;
bool     g_info         = false;
bool     g_play         = true;
bool     g_draw_axes    = false;
bool     g_draw_frustums= false;
bool     g_draw_grid    = true;
bool     g_animate      = false;
bool     g_wire         = false;
unsigned g_recon_mode   = 1;

gloost::PerspectiveCamera g_camera{50.0, g_aspect, 0.1, 200.0};
mvt::FourTiledWindow g_ftw{g_screenWidth, g_screenHeight};
pmd::CameraNavigator g_navi{0.1f};
std::unique_ptr<mvt::Statistics> g_stats{};
ScreenSpaceMeasureTool g_ssmt{&g_camera, g_screenWidth, g_screenHeight};
std::unique_ptr<kinect::NetKinectArray> g_nka;
std::unique_ptr<kinect::CalibVolume> g_cv;
std::unique_ptr<kinect::CalibrationFiles> g_calib_files;

void init(std::vector<std::string>& args);
void update_view_matrix();
void draw3d();
void resize(int width, int height);
void key(unsigned char key, int x, int y);
void motionFunc(int mouse_h, int mouse_v);
void mouseFunc(int button, int state, int mouse_h, int mouse_v);
void idle(void);

std::unique_ptr<kinect::ReconTrigrid> g_ksV3;// 4
std::vector<std::unique_ptr<kinect::Reconstruction>> g_recons;// 4

bool g_picking = false;

//////////////////////////////////////////////////////////////////////////////////////////
void init(std::vector<std::string> args){
  g_stats.reset(new mvt::Statistics{});
  g_stats->setInfoSlot("Volume Based Mapping", 0);

  bool found_file = false;
  for(unsigned i = 0; i < args.size(); ++i){
    const std::string ext(args[i].substr(args[i].find_last_of(".") + 1));
    std::cerr << ext << std::endl;
   if("ks" == ext){
      g_calib_files = std::unique_ptr<kinect::CalibrationFiles>{new kinect::CalibrationFiles(args[i].c_str())};
      g_nka = std::unique_ptr<kinect::NetKinectArray>{new kinect::NetKinectArray(args[i].c_str(), g_calib_files.get())};
      found_file = true;
      break;
    }
  }

  if (!found_file) {
    throw std::invalid_argument{"No .ks file specified"};
  }

  g_cv = std::unique_ptr<kinect::CalibVolume>{new kinect::CalibVolume(g_calib_files->getFileNames())};
  // g_ksV3 = std::unique_ptr<kinect::ReconTrigrid>(new kinect::ReconTrigrid(*g_calib_files, g_cv.get()));
  g_recons.emplace_back(new kinect::ReconTrigrid(*g_calib_files, g_cv.get()));
  g_recons.emplace_back(new kinect::ReconPoints(*g_calib_files, g_cv.get()));
  
  // binds to unit 0 and 1
  g_nka->bindToTextureUnits(0);

  // bind calubration volumes from 2 - 11
  g_cv->bindToTextureUnits(2);
}

//////////////////////////////////////////////////////////////////////////////////////////
  /// logic

void frameStep (){
  /// increment the frame counter
  ++g_frameCounter;

  glClearColor(0, 0, 0, 0);
  glViewport(0,0,g_screenWidth, g_screenHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  update_view_matrix();
  draw3d();
  
  g_ftw.endFrame();

  if(g_info)
    g_stats->draw(g_screenWidth, g_screenHeight);

  //std::cerr << g_frameCounter << std::endl;
  glutSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////////////////////
void update_view_matrix() {
  g_camera.set();
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gloost::Point3 speed(0.0,0.0,0.0);

  gloost::vec2 speed_button1(g_ftw.getButtonSpeed(1));
  gloost::vec2 speed_button2(g_ftw.getButtonSpeed(2));
  // std::cout << "speed1 " << speed_button1 << ", " << speed_button2 << std::endl;
  float fac = 0.005;
  speed[0] = speed_button1.u * fac;
  speed[1] = speed_button1.v * - 1.0 * fac;
  speed[2] = speed_button2.v * fac;

  gloost::Matrix camview(g_navi.get(speed));
  camview.invert();

  gloostMultMatrix(camview.data());

  gloost::Matrix modelview;
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview.data());
  g_ssmt.setModelView(modelview);
}

//////////////////////////////////////////////////////////////////////////////////////////
  /// main loop function, render with 3D setup
void draw3d(void)
{
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

  g_stats->startGPU();
  if (g_play) {
    g_nka->update();
  }
  // draw active reconstruction
  g_recons.at(g_recon_mode)->draw();

  g_stats->stopGPU();
  //std::cerr << "after stopGPU" << std::endl; check_gl_errors("after stopGPU", false);

  if(g_picking){
    float dist = g_ssmt.measure();
    g_stats->setInfoSlot(("measuring: " + gloost::toString(dist)).c_str(), 1);
  }
  else{
    g_stats->setInfoSlot("navigation mode", 1);
  }
  mvt::GlPrimitives::get()->drawLineSegments(g_ssmt.getMeasurePoints());

  if(g_draw_axes){
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    //glDisable(GL_DEPTH_TEST);
    mvt::GlPrimitives::get()->drawCoords();
    glPopAttrib();
  }   

  if(g_draw_frustums) {
    for(unsigned idx = 0; idx  < g_calib_files->num(); ++idx){
      glPushAttrib(GL_ALL_ATTRIB_BITS);
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
      
      mvt::CameraView* v = (mvt::CameraView*) &g_calib_files->getCalibs()[idx];
      v->updateMatrices();
      
      glPushMatrix();
      gloostMultMatrix(v->eye_d_to_world.data());
      glColor4f(0.0,0.0,1.0,1.0);
      v->drawFrustum();
      glPopMatrix();
      glPopAttrib();
    }
  }
  // draw black grid on floor for fancy look
  if(g_draw_grid) {
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
  g_camera.setAspect(g_aspect);

  g_navi.resize(width, height);
  g_ftw.resize(width, height);
  g_ssmt.resize(g_screenWidth, g_screenHeight);
  glutPostRedisplay();
}


//////////////////////////////////////////////////////////////////////////////////////////
  /// this function is called by glut when a key press occured

void key(unsigned char key, int x, int y)
{
  switch (key){
    /// press ESC or q to quit
  case 27 :
  case 'q':
    exit(0);
    break;
  case 9:
    g_recon_mode = (g_recon_mode + 1) % g_recons.size();
    break;
  case 'r':
    g_draw_axes = !g_draw_axes;
    break;
  case 'c':
    g_draw_frustums = !g_draw_frustums;
    break;
  case 'g':
    g_draw_grid = !g_draw_grid;
    break;
  case 'a':
    g_animate = !g_animate;
    break;
  case 'w':
    g_wire = !g_wire;
    break;
  case 's':
      for (auto& recon : g_recons) {
        recon->reload();
      }
      g_nka->reloadShader();
      g_calib_files->reload();
      g_cv->reload();
    break;
  case 'm':
    g_picking = !g_picking;
    break;
  case 'p':
    g_play = !g_play;
    break;
  case '#':
    for(unsigned i = 0; i < g_calib_files->num(); ++i){
      g_nka->depth_compression_lex = !g_nka->depth_compression_lex;
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
    g_navi.motion(mouse_h, mouse_v);
    g_ftw.motion(mouse_h, mouse_v);
  }
  glutPostRedisplay();
}


//////////////////////////////////////////////////////////////////////////////////////////
void passiveFunc(int x, int y){
  if(g_picking){
    
  }
  else{
    g_ftw.passive(x,y);
  }
  glutPostRedisplay();
}


//////////////////////////////////////////////////////////////////////////////////////////
void mouseFunc(int button, int state, int mouse_h, int mouse_v){

  if(g_picking){
    g_ssmt.mouse(button, state, mouse_h, g_screenHeight - mouse_v);
  }
  else{
    g_navi.mouse(button, state, mouse_h, mouse_v);
    g_ftw.mouse(button, state, mouse_h, mouse_v);
  }
  glutPostRedisplay();
}

void specialKey(int key, int x, int y){
  g_ftw.specialKey(key, x, y);
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