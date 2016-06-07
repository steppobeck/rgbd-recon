#include <glbinding/gl/gl.h>
using namespace gl;
// load glbinding function type
#include <glbinding/Function.h>
// load meta info extension
#include <glbinding/Meta.h>
// load callback support
#include <glbinding/callbacks.h>

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <globjects/globjects.h>
#include <globjects/Buffer.h>
#include <globjects/Query.h>
#include <globjects/base/File.h>

#include <imgui.h>
#include <imgui_impl_glfw_glb.h>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <memory>
#include <tuple>

#include <CMDParser.h>
#include "texture_blitter.hpp"

#include <Point3.h>
#include <BoundingBox.h>
#include <PerspectiveCamera.h>

#include <CameraNavigator.h>
#include "CalibVolumes.hpp"
#include <calibration_files.hpp>
#include <NetKinectArray.h>
#include <KinectCalibrationFile.h>
#include <Statistics.h>

#include "reconstruction.hpp"
#include "recon_trigrid.hpp"
#include "recon_points.hpp"
#include "recon_calibs.hpp"
#include "recon_integration.hpp"
#include "recon_mvt.hpp"

/// general setup
unsigned g_screenWidth  = 1280;
unsigned g_screenHeight = 720;
float    g_aspect       = g_screenWidth * 1.0/g_screenHeight;
bool     g_play         = true;
bool     g_draw_frustums= false;
bool     g_draw_grid    = true;
bool     g_animate      = false;
bool     g_wire         = false;
int      g_recon_mode   = 0;
bool     g_bilateral    = true;
bool     g_draw_calibvis= false;
bool     g_draw_textures= false;
int      g_texture_type = 0;
int      g_num_texture  = 0;
bool     g_processed    = true;
bool     g_refine       = true;
int      g_num_kinect   = 1; 
float    g_voxel_size   = 0.007f;
float    g_tsdf_limit   = 0.01f;
double   g_time_prev    = 0.0f;
globjects::ref_ptr<globjects::Query> g_query;

gloost::BoundingBox     g_bbox{};
std::vector<std::pair<int, int>> g_gui_texture_settings{};
gloost::PerspectiveCamera g_camera{50.0, g_aspect, 0.1, 200.0};
pmd::CameraNavigator g_navi{0.1f};
std::unique_ptr<mvt::Statistics> g_stats{};
std::unique_ptr<kinect::NetKinectArray> g_nka;
std::unique_ptr<kinect::CalibVolumes> g_cv;
std::unique_ptr<kinect::CalibrationFiles> g_calib_files;
GLFWwindow* g_window = nullptr;
globjects::Buffer* g_buffer_shading;
struct shading_data_t {
  int mode = 0;
} g_shading_buffer_data;

void init(std::vector<std::string>& args);
void update_view_matrix();
void draw3d();
void watch_gl_errors(bool activate);
void quit(int status);
std::shared_ptr<kinect::ReconIntegration> g_recon_integration{};
std::vector<std::shared_ptr<kinect::Reconstruction>> g_recons;// 4
std::unique_ptr<kinect::ReconCalibs> g_calibvis;// 4
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
  g_cv->setStartTextureUnit(9);
  g_cv->loadInverseCalibs(resource_path);
  g_cv->setStartTextureUnitInv(30);

  g_recons.emplace_back(new kinect::ReconPoints(*g_calib_files, g_cv.get(), g_bbox));
  g_recon_integration = std::make_shared<kinect::ReconIntegration>(*g_calib_files, g_cv.get(), g_bbox, g_tsdf_limit, g_voxel_size);
  g_recons.emplace_back(g_recon_integration);
  g_recons.emplace_back(new kinect::ReconTrigrid(*g_calib_files, g_cv.get(), g_bbox));
  g_recons.emplace_back(new kinect::ReconMVT(*g_calib_files, g_cv.get(), g_bbox)); 

  g_calibvis = std::unique_ptr<kinect::ReconCalibs>(new kinect::ReconCalibs(*g_calib_files, g_cv.get(), g_bbox));

  // enable point scaling in vertex shader
  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_POINT_SPRITE);

  g_buffer_shading = new globjects::Buffer();
  g_buffer_shading->ref();
  g_buffer_shading->setData(sizeof(shading_data_t), &g_shading_buffer_data, GL_STATIC_DRAW);
  g_buffer_shading->bindBase(GL_UNIFORM_BUFFER, 1);
}

//////////////////////////////////////////////////////////////////////////////////////////
  /// logic
void update_gui() {
  ImGui_ImplGlfwGLB_NewFrame();

  // 1. Show a simple window
  // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
  // {
  //   static float f = 0.0f;
  //   ImGui::Text("Hello, world!");
  //   ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
  //   ImGui::ColorEdit3("clear color", (float*)&clear_color);
  //   if (ImGui::Button("Another Window")) show_another_window ^= 1;
  // }
  // 2. Show another simple window, this time using an explicit Begin/End pair
  // if (show_another_window)
  {
    ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Settings");
    if (ImGui::Button("Show textures")) {
      g_gui_texture_settings.emplace_back(0, 0);
    }
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    if (ImGui::CollapsingHeader("Reconstruction",ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::RadioButton("Points", &g_recon_mode, 0);
      ImGui::RadioButton("Integration", &g_recon_mode, 1);
      ImGui::RadioButton("Trigrid", &g_recon_mode, 2);     
      ImGui::RadioButton("Trigrid orig", &g_recon_mode, 3);      
    }
    if (ImGui::CollapsingHeader("Visualisation")) {
      int prev = g_shading_buffer_data.mode;
      ImGui::RadioButton("Textured", &g_shading_buffer_data.mode, 0);
      ImGui::RadioButton("Shaded", &g_shading_buffer_data.mode, 1);
      ImGui::RadioButton("Normals", &g_shading_buffer_data.mode, 2);     
      ImGui::RadioButton("Blending", &g_shading_buffer_data.mode, 3);
      if(prev != g_shading_buffer_data.mode) {
        g_buffer_shading->setSubData(0, sizeof(shading_data_t), &g_shading_buffer_data);
      }
    }
    if (ImGui::CollapsingHeader("Processing")) {
      if (ImGui::Checkbox("Morphological Filter", &g_processed)) {
        g_nka->useProcessedDepths(g_processed);
      }
      if (ImGui::Checkbox("Bilateral Filter", &g_bilateral)) {
        g_nka->filterTextures(g_bilateral);
      }
      if (ImGui::Checkbox("Boundary Refinement", &g_refine)) {
        g_nka->refineBoundary(g_refine);
      }
    }
    if (ImGui::CollapsingHeader("Integration ")) {
      if (ImGui::DragFloat("TSDF Limit", &g_tsdf_limit, 0.001f, 0.001f, 0.03f, "%.3f")) {
        g_recon_integration->setTsdfLimit(g_tsdf_limit);
      }
      if (ImGui::DragFloat("Voxel Size", &g_voxel_size, 0.001f, 0.003f, 0.1f, "%.3f")) {
        g_recon_integration->setVoxelSize(g_voxel_size);
      }
    }
    if (ImGui::CollapsingHeader("Processing Performance")) {
      std::uint64_t total = 
        g_nka->getStageTime("morph")
        + g_nka->getStageTime("bilateral")
        + g_nka->getStageTime("boundary")
        + g_nka->getStageTime("normal")
        + g_nka->getStageTime("quality");

      if (ImGui::TreeNode("Texture Processing")) {
        ImGui::SameLine();
        ImGui::Text("   %.3f ms", total / 1000000.0f);
        ImGui::Columns(2, NULL, false);
        ImGui::Text("Morphological Filtering");
        ImGui::Text("Bilateral Filtering");
        ImGui::Text("Boundary Refinement");
        ImGui::Text("Normal Computation");
        ImGui::Text("Quality Computation");
        ImGui::NextColumn();
        ImGui::Text("%.3f ms", g_nka->getStageTime("morph") / 1000000.0f);
        ImGui::Text("%.3f ms", g_nka->getStageTime("bilateral") / 1000000.0f);
        ImGui::Text("%.3f ms", g_nka->getStageTime("boundary") / 1000000.0f);
        ImGui::Text("%.3f ms", g_nka->getStageTime("normal") / 1000000.0f);
        ImGui::Text("%.3f ms", g_nka->getStageTime("quality") / 1000000.0f);
      }
      else {
        ImGui::SameLine();
        ImGui::Text("   %.3f ms", total / 1000000.0f);
      }
    }
    ImGui::End();
  }
  for(unsigned i = 0; i < g_gui_texture_settings.size(); ++i) {
    auto& setting = g_gui_texture_settings[i];
    ImGui::SetNextWindowSize(ImVec2(100,100), ImGuiSetCond_FirstUseEver);
    bool show_tex = true;
    if (!ImGui::Begin(std::string{"Textures " + std::to_string(i)}.c_str(), &show_tex)) {
        ImGui::End();
    }
    else {
      if (ImGui::CollapsingHeader("Kinect", ImGuiTreeNodeFlags_DefaultOpen)) {
        static std::vector<const char*> listbox_items = {"1", "2", "3", "4"};
        ImGui::ListBox("Number", &setting.second, listbox_items.data(), listbox_items.size(), listbox_items.size());
      }
      if (ImGui::CollapsingHeader("Texture Type", ImGuiTreeNodeFlags_DefaultOpen)) {
        static std::vector<const char*> listbox_items = {"Color", "Depth", "Quality", "Normals", "Silhouette", "Orig Depth", "LAB colors"};
        ImGui::ListBox("Type", &setting.first, listbox_items.data(), listbox_items.size(), listbox_items.size());
      }
      TexInfo test = {g_nka->getStartTextureUnit() + setting.first, -setting.second - 1};
      ImTextureID cont;
      std::memcpy(&cont, &test, sizeof(test));
      glm::uvec2 res{g_nka->getDepthResolution()};
      float aspect = float(res.x) / res.y;
      float width = ImGui::GetWindowContentRegionWidth();
      ImGui::Image(cont, ImVec2(width, width / aspect), ImVec2(0,0), ImVec2(1,1), ImColor(255,255,255,255), ImColor(255,255,255,128));
      ImGui::End();
    }
    if (!show_tex) {
      g_gui_texture_settings.pop_back();
    }
  }
  ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
  ImGui::ShowTestWindow();
}

void frameStep (){
  glfwPollEvents();
  update_gui();

  update_view_matrix();
  draw3d();

  ImGui::Render();
  glfwSwapBuffers(g_window);
}

//////////////////////////////////////////////////////////////////////////////////////////
void update_view_matrix() {
  g_camera.set();
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gloost::Point3 speed(0.0,0.0,0.0);

  glm::fvec2 speed_button1(g_navi.getOffset(0));
  glm::fvec2 speed_button2(g_navi.getOffset(1));
  // std::cout << "speed1 " << speed_button1 << ", " << speed_button2 << std::endl;
  float fac = 0.005;
  speed[0] = speed_button1.x * fac;
  speed[1] = speed_button1.y * - 1.0 * fac;
  speed[2] = speed_button2.y * fac;

  gloost::Matrix camview(g_navi.get(speed));
  camview.invert();

  glMultMatrixf(camview.data());

  gloost::Matrix modelview;
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview.data());

  g_navi.resetOffsets();
}

//////////////////////////////////////////////////////////////////////////////////////////
  /// main loop function, render with 3D setup
void draw3d(void)
{  
  glClearColor(0, 0, 0, 0);
  glViewport(0,0,g_screenWidth, g_screenHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

  if (g_draw_calibvis) {
    g_calibvis->draw();
  }

  if(g_draw_frustums) {
    g_cv->drawFrustums();
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
    g_bbox.draw();
  }

  if (g_draw_textures) {
    TextureBlitter::blit(g_nka->getStartTextureUnit() + g_texture_type, g_num_texture, g_nka->getDepthResolution());
  }
  glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

////////////////////////////////////////////////////////////////////////////////
  /// this function is triggered when the screen is resized

void update_view(GLFWwindow* window, int width, int height){

  g_screenWidth = width;
  g_screenHeight = height;
  g_aspect       = g_screenWidth * 1.0/g_screenHeight;
  g_camera.setAspect(g_aspect);

  for (auto& recon : g_recons) {
    recon->resize(width, height);
  }

  g_navi.resize(width, height);
}

//////////////////////////////////////////////////////////////////////////////////////////
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if(action != GLFW_RELEASE) return;

  switch (key){
    /// press ESC or q to quit
  case GLFW_KEY_ESCAPE:
  case GLFW_KEY_Q:
    glfwSetWindowShouldClose(g_window, 1);
    break;
  case GLFW_KEY_F:
    g_draw_frustums = !g_draw_frustums;
    break;
  case GLFW_KEY_B:
    g_bilateral = !g_bilateral;
    g_nka->filterTextures(g_bilateral);
    break;
  case GLFW_KEY_D:
    g_processed = !g_processed;
    g_nka->useProcessedDepths(g_processed);
    break;
  case GLFW_KEY_N:
    g_refine = !g_refine;
    g_nka->refineBoundary(g_refine);
    break;
  case GLFW_KEY_G:
    g_draw_grid = !g_draw_grid;
    break;
  case GLFW_KEY_A:
    g_animate = !g_animate;
    break;
  case GLFW_KEY_W:
    g_wire = !g_wire;
    break;
  case GLFW_KEY_Y:
    g_num_texture = (g_num_texture + 1) % g_calib_files->num();
    break;
  case GLFW_KEY_U:
    g_texture_type = (g_texture_type + 1) % 7;
    break;
  case GLFW_KEY_T:
    g_draw_textures = !g_draw_textures;
    break;
  case GLFW_KEY_S:
      for (auto& recon : g_recons) {
        recon->reload();
      }
      globjects::File::reloadAll();
      g_nka->processTextures(); 
    break;
  case GLFW_KEY_P:
    g_play = !g_play;
    break;
  case GLFW_KEY_1:
    g_shading_buffer_data.mode = (g_shading_buffer_data.mode + 1) % 4;
    g_buffer_shading->setSubData(0, sizeof(shading_data_t), &g_shading_buffer_data);
    break;
  case GLFW_KEY_V:
    g_draw_calibvis = !g_draw_calibvis;
    break;
  case GLFW_KEY_C:
    g_num_kinect = (g_num_kinect+ 1) % g_calib_files->num();
    g_calibvis->setActiveKinect(g_num_kinect);
    break;
  case GLFW_KEY_PAGE_UP:
    g_recon_mode = (g_recon_mode + 1) % g_recons.size();
    break;
  case GLFW_KEY_PAGE_DOWN:
    g_recon_mode = (g_recon_mode + g_recons.size() - 1) % g_recons.size();
    break;  
  default:
    break;
  }
}


//////////////////////////////////////////////////////////////////////////////////////////
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  g_navi.motion(xpos, ypos);
}

//////////////////////////////////////////////////////////////////////////////////////////
void click_callback(GLFWwindow* window, int button, int action, int mods){
  if(ImGui::GetIO().WantCaptureMouse) return;
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  int mouse_h = xpos;
  int mouse_v = ypos;

  g_navi.mouse(button, action, mouse_h, mouse_v);
}

////////////////////////////////////////////////////////////////////////////////
static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error %d: %s\n", error, description);
}

void quit(int status) {
  //free globjects 
  globjects::detachAllObjects();

  ImGui_ImplGlfwGLB_Shutdown();
  // free glfw resources
  glfwDestroyWindow(g_window);
  glfwTerminate();

  std::exit(status);
}

int main(int argc, char *argv[]) {
  CMDParser p("kinect_surface ...");

  p.addOpt("r",2,"resolution", "set screen resolution");
  p.init(argc,argv);

  if(p.isOptSet("r")){
    g_screenWidth = p.getOptsInt("r")[0];
    g_screenHeight = p.getOptsInt("r")[1];
  }

  // Setup window
  glfwSetErrorCallback(error_callback);
  if(!glfwInit()) {
    std::exit(EXIT_FAILURE);  
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  
  g_window = glfwCreateWindow(g_screenWidth, g_screenHeight, "Kinect Reconstruction", NULL, NULL); 
  if(!g_window) {
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(g_window);

  // Setup ImGui binding
  ImGui_ImplGlfwGLB_Init(g_window, true);
  // disable vsync
  glfwSwapInterval(0);
  glfwSetKeyCallback(g_window, key_callback);
  glfwSetCursorPosCallback(g_window, mouse_callback);
  glfwSetMouseButtonCallback(g_window, click_callback);
  glfwSetFramebufferSizeCallback(g_window, update_view);
  // allow unlimited mouse movement

  // Initialize globjects (internally initializes glbinding, and registers the current context)
  globjects::init();

  watch_gl_errors(true);

  // set some gl states
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  
  // load and intialize stuff for our demo
  init(p.getArgs());

  update_view(g_window, g_screenWidth, g_screenHeight);

  while (!glfwWindowShouldClose(g_window)) {
    frameStep();
  }

  quit(EXIT_SUCCESS);
}

void watch_gl_errors(bool activate) {
  if(activate) {
    // add callback after each function call
    glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After | glbinding::CallbackMask::ParametersAndReturnValue, {"glGetError", "glVertex3f", "glVertex2f", "glBegin", "glColor3f"});
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
        }
      }
    );
  }
  else {
    glbinding::setCallbackMask(glbinding::CallbackMask::None);
  }
}