#include <CMDParser.h>
#include <screen_space_measurement_tool.hpp>

#include <globjects/globjects.h>
#include <globjects/base/File.h>
// load glbinding function type
#include <glbinding/Function.h>
#// load meta info extension
#include <glbinding/Meta.h>
// load callback support
#include <glbinding/callbacks.h>
using namespace gl;
#include <GL/glut.h>

#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <memory>

#include <Point3.h>
#include <BoundingBox.h>
#include <PerspectiveCamera.h>

#include <CameraNavigator.h>
#include <FourTiledWindow.h>
#include "CalibVolumes.hpp"
#include <calibration_files.hpp>
#include <NetKinectArray.h>
#include <KinectCalibrationFile.h>
#include <Statistics.h>
#include <GlPrimitives.h>

#include "reconstruction.hpp"
#include "recon_trigrid.hpp"
#include "recon_points.hpp"
#include "recon_calibs.hpp"
#include "recon_integration.hpp"

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
bool     g_bilateral    = true;
bool     g_draw_calibvis= false;
bool     g_draw_textures= false;
unsigned g_texture_type = 0;
unsigned g_num_texture  = 0;
gloost::BoundingBox     g_bbox{};

gloost::PerspectiveCamera g_camera{50.0, g_aspect, 0.1, 200.0};
mvt::FourTiledWindow g_ftw{g_screenWidth, g_screenHeight};
pmd::CameraNavigator g_navi{0.1f};
std::unique_ptr<mvt::Statistics> g_stats{};
ScreenSpaceMeasureTool g_ssmt{&g_camera, g_screenWidth, g_screenHeight};
std::unique_ptr<kinect::NetKinectArray> g_nka;
std::unique_ptr<kinect::CalibVolumes> g_cv;
std::unique_ptr<kinect::CalibrationFiles> g_calib_files;

void init(std::vector<std::string>& args);
void update_view_matrix();
void draw3d();
void resize(int width, int height);
void key(unsigned char key, int x, int y);
void motionFunc(int mouse_h, int mouse_v);
void mouseFunc(int button, int state, int mouse_h, int mouse_v);
void idle(void);
void watch_gl_errors(bool activate);

std::unique_ptr<kinect::ReconTrigrid> g_ksV3;// 4
std::vector<std::unique_ptr<kinect::Reconstruction>> g_recons;// 4
std::unique_ptr<kinect::ReconCalibs> g_calibvis;// 4

bool g_picking = false;

//////////////////////////////////////////////////////////////////////////////////////////
void init(std::vector<std::string> args){
  g_stats.reset(new mvt::Statistics{});
  g_stats->setInfoSlot("Volume Based Mapping", 0);

  std::string file_name{};
  for(unsigned i = 0; i < args.size(); ++i){
    const std::string ext(args[i].substr(args[i].find_last_of(".") + 1));
    std::cerr << ext << std::endl;
    if("ks" == ext) {
      file_name = args[i];
      break;
    }
  }

  if (file_name.empty()) {
    throw std::invalid_argument{"No .ks file specified"};
  }

  std::string serverport{};
  std::vector<std::string> calib_filenames;
  gloost::Point3 bbox_min{-1.0f ,0.0f, -1.0f};
  gloost::Point3 bbox_max{ 1.0f ,2.2f, 1.0f};

  std::string resource_path = file_name.substr(0, file_name.find_last_of("/\\")) + '/';
  std::cout << resource_path << std::endl;
  std::ifstream in(file_name);
  std::string token;
  while(in >> token){
    if(token == "serverport"){
      in >> serverport;
    } 
    else if (token == "kinect") {
      in >> token;
      // detect absolute path
      if (token[0] == '/' || token[1] == ':') {
        calib_filenames.push_back(token);
      }
      else {
        calib_filenames.push_back(resource_path + token);
      }
    }
    else if (token == "bbx") {
      in >> bbox_min[0];
      in >> bbox_min[1];
      in >> bbox_min[2];
      in >> bbox_max[0];
      in >> bbox_max[1];
      in >> bbox_max[2];
    }
  }
  in.close();
  // update bounding box dimensions with read values
  g_bbox.setPMin(bbox_min);
  g_bbox.setPMax(bbox_max);

  g_calib_files = std::unique_ptr<kinect::CalibrationFiles>{new kinect::CalibrationFiles(calib_filenames)};
  g_cv = std::unique_ptr<kinect::CalibVolumes>{new kinect::CalibVolumes(calib_filenames, g_bbox)};
  g_nka = std::unique_ptr<kinect::NetKinectArray>{new kinect::NetKinectArray(serverport, g_calib_files.get(), g_cv.get())};
  
  // binds to unit 1 to 3
  g_nka->setStartTextureUnit(1);
  // bind calubration volumes from 4 - 13
  g_cv->setStartTextureUnit(4);
  g_cv->loadInverseCalibs(resource_path);
  g_cv->setStartTextureUnitInv(30);

  g_recons.emplace_back(new kinect::ReconTrigrid(*g_calib_files, g_cv.get(), g_bbox));
  g_recons.emplace_back(new kinect::ReconPoints(*g_calib_files, g_cv.get(), g_bbox));
  g_recons.emplace_back(new kinect::ReconIntegration(*g_calib_files, g_cv.get(), g_bbox));

  g_calibvis = std::unique_ptr<kinect::ReconCalibs>(new kinect::ReconCalibs(*g_calib_files, g_cv.get(), g_bbox));

  // enable point scaling in vertex shader
  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_POINT_SPRITE);
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

  glMultMatrixf(camview.data());

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
  if (g_draw_calibvis) {
    g_calibvis->draw();
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

  if (g_draw_textures) {
    g_nka->drawTextures(g_texture_type, g_num_texture);
  }

  g_bbox.draw();
}

////////////////////////////////////////////////////////////////////////////////
  /// this function is triggered when the screen is resized

void resize(int width, int height){

  g_screenWidth = width;
  g_screenHeight = height;
  g_aspect       = g_screenWidth * 1.0/g_screenHeight;
  g_camera.setAspect(g_aspect);

  for (auto& recon : g_recons) {
    recon->resize(width, height);
  }

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
  case 'r':
    g_draw_axes = !g_draw_axes;
    break;
  case 'f':
    g_draw_frustums = !g_draw_frustums;
    break;
  case 'b':
    g_bilateral = !g_bilateral;
    g_nka->filterTextures(g_bilateral);
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
  case 'y':
    g_num_texture = (g_num_texture + 1) % g_calib_files->num();
    break;
  case 'u':
    g_texture_type = (g_texture_type + 1) % 3;
    break;
  case 't':
    g_draw_textures = !g_draw_textures;
    break;
  case 's':
      for (auto& recon : g_recons) {
        recon->reload();
      }
      globjects::File::reloadAll();
      g_nka->processTextures(); 
    break;
  case 'm':
    g_picking = !g_picking;
    break;
  case 'p':
    g_play = !g_play;
    break;
  case 'v':
    g_draw_calibvis = !g_draw_calibvis;
    break;
  case 'c':
    static unsigned num_kinect = 1; 
    num_kinect = (num_kinect+ 1) % g_calib_files->num();
    g_calibvis->setActiveKinect(num_kinect);
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

  switch(key) {
    case GLUT_KEY_PAGE_UP:
      g_recon_mode = (g_recon_mode + 1) % g_recons.size();
      break;
    case  GLUT_KEY_PAGE_DOWN:
      g_recon_mode = (g_recon_mode - 1 + g_recons.size()) % g_recons.size();
      break;  
    default:
      break;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////
  /// this function is triggered by glut,
  /// when nothing is left to do for this frame

void idle(void){
    glutPostRedisplay();
}


////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
  CMDParser p("kinect_surface ...");

  p.addOpt("r",2,"resolution", "set screen resolution");
  p.addOpt("i",-1,"info", "draw info");
  p.init(argc,argv);

  if(p.isOptSet("r")){
    g_screenWidth = p.getOptsInt("r")[0];
    g_screenHeight = p.getOptsInt("r")[1];
  }

  if(p.isOptSet("i")){
    g_info = true;
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

  // glbinding::Binding::initialize(); // only resolve functions that are actually used (lazy)
  // Initialize globjects (internally initializes glbinding, and registers the current context)
  globjects::init();

  watch_gl_errors(true);

  // set some gl states
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  
  // load and intialize stuff for our demo
  init(p.getArgs());

  
  /// start the loop (this will call display() every frame)
  glutMainLoop();

  //free globjects 
  globjects::detachAllObjects();

  return EXIT_SUCCESS;
}

void watch_gl_errors(bool activate) {
  if(activate) {
    // add callback after each function call
    glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After | glbinding::CallbackMask::ParametersAndReturnValue, {"glGetError", "glVertex3f", "glVertex2f", "glBegin"});
    glbinding::setAfterCallback(
      [](glbinding::FunctionCall const& call) {
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
          // print name
          std::cerr <<  "OpenGL Error: " << call.function->name() << "(";
          // parameters
          for (unsigned i = 0; i < call.parameters.size(); ++i)
          {
            std::cerr << call.parameters[i]->asString();
            if (i < call.parameters.size() - 1)
              std::cerr << ", ";
          }
          std::cerr << ")";
          // return value
          if(call.returnValue) {
            std::cerr << " -> " << call.returnValue->asString();
          }
          // error
          std::cerr  << " - " << glbinding::Meta::getString(error) << std::endl;
          throw std::exception{};
          exit(EXIT_FAILURE);
        }
      }
    );
  }
  else {
    glbinding::setCallbackMask(glbinding::CallbackMask::None);
  }
}