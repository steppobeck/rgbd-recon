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
#include <FeedbackReceiver.h>
#include "configurator.hpp"
#include "texture_blitter.hpp"

#include <Point3.h>
#include <BoundingBox.h>
#include <PerspectiveCamera.h>
#include <StereoCamera.h>

#include <CameraNavigator.h>
#include "CalibVolumes.hpp"
#include <calibration_files.hpp>
#include <NetKinectArray.h>
#include <KinectCalibrationFile.h>

#include "timer_database.hpp"
#include "reconstruction.hpp"
#include "recon_trigrid.hpp"
#include "recon_points.hpp"
#include "recon_calibs.hpp"
#include "recon_integration.hpp"
#include "recon_mvt.hpp"

/// general setup
std::string g_server_socket = "127.0.0.1:7000";
sys::FeedbackReceiver* g_fbr = 0;
float    g_clear_color[4] = {0.0,0.0,0.0,0.0};
unsigned g_stereo_mode  = 0;
float    g_screenWidthReal = 1.28;
float    g_screenHeightReal = 0.72;
unsigned g_screenWidth  = 1280;
unsigned g_screenHeight = 720;
unsigned g_windowWidth  = 1280;
unsigned g_windowHeight = 720;
unsigned g_left_pos_x = 0;
unsigned g_left_pos_y = 0;
unsigned g_right_pos_x = 0;
unsigned g_right_pos_y = 0;

float    g_aspect       = g_screenWidth * 1.0/g_screenHeight;
bool     g_play         = true;
bool     g_draw_frustums= false;
bool     g_draw_grid    = true;
bool     g_animate      = false;
int      g_recon_mode   = 1;
bool     g_bilateral    = true;
bool     g_draw_calibvis= false;
bool     g_draw_textures= false;
int      g_texture_type = 0;
int      g_num_texture  = 0;
bool     g_processed    = true;
bool     g_refine       = true;
bool     g_colorfill    = true;
bool     g_bricking     = true;
bool     g_skip_space   = true;
bool     g_draw_bricks  = false;
bool     g_watch_errors = true;
int      g_num_kinect   = 1; 
float    g_voxel_size   = 0.01f;
float    g_brick_size   = 0.1f;
float    g_tsdf_limit   = 0.01f;
float    g_zoom         = 0.5f;
double   g_time_prev    = 0.0f;

int g_min_voxels   = 10;

bool     g_loaded_conf  = false;
unsigned g_time_limit   = 1;
std::string g_conf_file{};

gloost::BoundingBox     g_bbox{};
std::vector<std::pair<int, int>> g_gui_texture_settings{};
gloost::PerspectiveCamera g_camera{50.0, g_aspect, 0.1, 200.0};
gloost::StereoCamera* g_stereo_camera;
pmd::CameraNavigator g_navi{0.5f};
std::unique_ptr<kinect::NetKinectArray> g_nka;
std::unique_ptr<kinect::CalibVolumes> g_cv;
std::unique_ptr<kinect::CalibrationFiles> g_calib_files;
GLFWwindow* g_window = nullptr;
globjects::Buffer* g_buffer_shading;
struct shading_data_t {
  int mode = 0;
} g_shading_buffer_data;

void init_stereo_camera();
void init_fbr(const char* client_socket);
void init(std::vector<std::string> const& args);
void init_config(std::vector<std::string> const& args);
void load_config(std::string const&);
void update_model_matrix(bool load_ident = true);
void draw3d();
void watch_gl_errors(bool activate);
void quit(int status);
std::shared_ptr<kinect::ReconIntegration> g_recon_integration{};
std::vector<std::shared_ptr<kinect::Reconstruction>> g_recons;// 4
std::unique_ptr<kinect::ReconCalibs> g_calibvis;// 4
//////////////////////////////////////////////////////////////////////////////////////////



void init_stereo_camera(){

  gloost::Matrix eye_matrix;
  eye_matrix.setIdentity();
  eye_matrix.setTranslate(0.0,0.0,1.0);
  gloost::Matrix screen_matrix;
  screen_matrix.setIdentity();

  g_stereo_camera = new gloost::StereoCamera(eye_matrix,
					     0.2,
					     20.0,
					     0.064 /*eyesep*/,
					     screen_matrix,
					     g_screenWidthReal,
					     g_screenHeightReal);
  
}

std::ostream& operator<< (std::ostream& os, const glm::mat4& m){
  os << "mat4[" << std::fixed << std::endl;
  os << "       ("               << m[0][0] << ", " << m[1][0] << ", " << m[2][0]  << ", " << m[3][0] << ")," << std::endl;
  os << "       ("               << m[0][1] << ", " << m[1][1] << ", " << m[2][1]  << ", " << m[3][1] << ")," << std::endl;
  os << "       ("               << m[0][2] << ", " << m[1][2] << ", " << m[2][2]  << ", " << m[3][2] << ")," << std::endl;
  os << "       ("               << m[0][3] << ", " << m[1][3] << ", " << m[2][3]  << ", " << m[3][3] << ") ]" << std::endl;

  return os;
}

gloost::Matrix glm2gloost(const glm::mat4 m){
  gloost::Matrix tmp;
  tmp[0] = m[0][0];
  tmp[1] = m[0][1];
  tmp[2] = m[0][2];
  tmp[3] = m[0][3];

  tmp[4] = m[1][0];
  tmp[5] = m[1][1];
  tmp[6] = m[1][2];
  tmp[7] = m[1][3];

  tmp[8]  = m[2][0];
  tmp[9]  = m[2][1];
  tmp[10] = m[2][2];
  tmp[11] = m[2][3];

  tmp[12] = m[3][0];
  tmp[13] = m[3][1];
  tmp[14] = m[3][2];
  tmp[15] = m[3][3];


  return tmp;
}
void init_fbr(const char* client_socket){

  sys::feedback initial_fb;
  initial_fb.cyclops_mat[3][0] = 0.0;
  initial_fb.cyclops_mat[3][1] = 0.0;
  initial_fb.cyclops_mat[3][2] = 1.0;
  initial_fb.recon_mode = 1;
  g_fbr = new sys::FeedbackReceiver(initial_fb, client_socket);

}

void init(std::vector<std::string> const& args){

  std::string ext{args[0].substr(args[0].find_last_of(".") + 1)};
  std::string file_name{};
  if("ks" == ext) {
    file_name = args[0];
  }
  else {
    throw std::invalid_argument{"No .ks file specified"};
  }


  // read ks file
  std::vector<std::string> calib_filenames;
  gloost::Point3 bbox_min{-1.0f ,0.0f, -1.0f};
  gloost::Point3 bbox_max{ 1.0f ,2.2f, 1.0f};

  std::string resource_path = file_name.substr(0, file_name.find_last_of("/\\")) + '/';
  std::cout << resource_path << std::endl;
  std::ifstream in(file_name);
  std::string token;
  while(in >> token){
    if (token == "kinect") {
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
  g_nka = std::unique_ptr<kinect::NetKinectArray>{new kinect::NetKinectArray(g_server_socket, g_calib_files.get(), g_cv.get())};
  
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
  
  // apply settings
  g_nka->useProcessedDepths(g_processed);
  g_nka->filterTextures(g_bilateral);
  g_nka->refineBoundary(g_refine);
  g_recon_integration->setTsdfLimit(g_tsdf_limit);
  g_recon_integration->setVoxelSize(g_voxel_size);
  g_recon_integration->setBrickSize(g_brick_size);
  g_recon_integration->setColorFilling(g_colorfill);
  g_recon_integration->setSpaceSkip(g_skip_space);
  g_recon_integration->setDrawBricks(g_draw_bricks);
  g_recon_integration->setUseBricks(g_bricking);
}

void init_config(std::vector<std::string> const& args) {
    // read config file
  if(args.size() > 1) {
    std::string ext = args[1].substr(args[1].find_last_of(".") + 1);
    if("conf" == ext) {
      load_config(args[1]);
    }
    else {
      throw std::invalid_argument{"No .conf file specified"};
    }    
  }
}

void load_config(std::string const& file_name) {
  configurator().read(file_name);
  configurator().print();
  g_recon_mode   = configurator().getUint("recon_mode");
  g_screenWidth  = configurator().getUint("screenWidth");
  g_screenHeight = configurator().getUint("screenHeight");
  g_play         = configurator().getBool("play");
  g_draw_grid    = configurator().getBool("draw_grid");
  g_animate      = configurator().getBool("animate");
  g_bilateral    = configurator().getBool("bilateral");
  g_processed    = configurator().getBool("processed");
  g_refine       = configurator().getBool("refine");
  g_colorfill    = configurator().getBool("colorfill");
  g_bricking     = configurator().getBool("bricking");
  g_skip_space   = configurator().getBool("skip_space");
  g_watch_errors = configurator().getBool("watch_errors");
  g_voxel_size   = configurator().getFloat("voxel_size");
  g_brick_size   = configurator().getFloat("brick_size");
  g_tsdf_limit   = configurator().getFloat("tsdf_limit");
  g_zoom         = configurator().getFloat("zoom");
  g_time_limit   = configurator().getUint("time_limit");
  g_loaded_conf = true;
  g_conf_file = file_name;
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
    if (ImGui::Checkbox("Watch OpenGL errors", &g_watch_errors)) {
      watch_gl_errors(g_watch_errors);
    } 
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    if (ImGui::CollapsingHeader("Reconstruction Mode",ImGuiTreeNodeFlags_DefaultOpen)) {
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
        g_recon_integration->integrate();

      }
      if (ImGui::Checkbox("Bilateral Filter", &g_bilateral)) {
        g_nka->filterTextures(g_bilateral);
        g_recon_integration->integrate();

      }
      if (ImGui::Checkbox("Boundary Refinement", &g_refine)) {
        g_nka->refineBoundary(g_refine);
        g_recon_integration->integrate();

      }
    }
    if (ImGui::CollapsingHeader("Integration ")) {
      if (ImGui::DragFloat("TSDF Limit", &g_tsdf_limit, 0.001f, 0.001f, 0.03f, "%.3f")) {
        g_recon_integration->setTsdfLimit(g_tsdf_limit);
      }
      if (ImGui::DragFloat("Voxel Size", &g_voxel_size, 0.001f, 0.003f, 0.1f, "%.3f")) {
        g_recon_integration->setVoxelSize(g_voxel_size);
        g_brick_size = g_recon_integration->getBrickSize();
      }
      if (ImGui::DragFloat("Brick Size", &g_brick_size, 0.01f, 0.1f, 1.0f, "%.3f")) {
        g_recon_integration->setBrickSize(g_brick_size);
        g_brick_size = g_recon_integration->getBrickSize();
      }
      if (ImGui::DragInt("Min Brick Voxels", &g_min_voxels, 1, 0, 500, "%.0f")) {
        g_recon_integration->setMinVoxelsPerBrick(g_min_voxels);
        g_recon_integration->updateOccupiedBricks();
        g_recon_integration->integrate();
      }
      if (ImGui::Checkbox("Color hole filling", &g_colorfill)) {
        g_recon_integration->setColorFilling(g_colorfill);
      }
      if(g_recons[g_recon_mode].get() == g_recon_integration.get() &&g_bricking) {
        ImGui::Columns(2, NULL, false);
        if (ImGui::Checkbox("Volume Bricking", &g_bricking)) {
          g_recon_integration->setUseBricks(g_bricking);
        }
        ImGui::NextColumn();
        ImGui::Text("%.3f %% occupied", g_recon_integration->occupiedRatio() * 100.0f);
        ImGui::Columns(1);
        
        if(g_bricking) {
          if (ImGui::Checkbox("Skip empty Spaces", &g_skip_space)) {
            g_recon_integration->setSpaceSkip(g_skip_space);
          }
          if (ImGui::Checkbox("Draw occupied bricks", &g_draw_bricks)) {
            g_recon_integration->setDrawBricks(g_draw_bricks);
          }
        }        
      }
      else {
        if (ImGui::Checkbox("Volume Bricking", &g_bricking)) {
          g_recon_integration->setUseBricks(g_bricking);
          g_recon_integration->integrate();
        }
      }
      if (ImGui::Checkbox("Draw TSDF", &g_draw_calibvis)) {
        // g_recon_integration->setDrawBricks(g_skip_space);
      }
    }
    if (ImGui::CollapsingHeader("Processing Performance")) {

      if (ImGui::TreeNode("Texture Processing")) {
        ImGui::SameLine();
        ImGui::Text("   %.3f ms", TimerDatabase::instance().duration("1preprocess") / 1000000.0f);
        ImGui::Columns(2, NULL, false);
        ImGui::Text("Morphological Filtering");
        ImGui::Text("Bilateral Filtering");
        ImGui::Text("Boundary Refinement");
        ImGui::Text("Normal Computation");
        ImGui::Text("Quality Computation");
        ImGui::NextColumn();
        ImGui::Text("%.3f ms", TimerDatabase::instance().duration("morph") / 1000000.0f);
        ImGui::Text("%.3f ms", TimerDatabase::instance().duration("bilateral") / 1000000.0f);
        ImGui::Text("%.3f ms", TimerDatabase::instance().duration("boundary") / 1000000.0f);
        ImGui::Text("%.3f ms", TimerDatabase::instance().duration("normal") / 1000000.0f);
        ImGui::Text("%.3f ms", TimerDatabase::instance().duration("quality") / 1000000.0f);
        ImGui::Columns(1);
        ImGui::TreePop();
      }
      else {
        ImGui::SameLine();
        ImGui::Text("   %.3f ms", TimerDatabase::instance().duration("1preprocess") / 1000000.0f);
      }
      if(g_recons[g_recon_mode].get() == g_recon_integration.get()) {
        std::uint64_t full = TimerDatabase::instance().duration("3recon") + TimerDatabase::instance().duration("2integrate");
        if (ImGui::TreeNode("Reconstruction")) {
          ImGui::SameLine();
          ImGui::Text("   %.3f ms", full / 1000000.0f);
          ImGui::Columns(2, NULL, false);
          ImGui::Text("Raymarching");
          ImGui::Text("Integration");
          ImGui::Text("Colorfilling");
          if(g_skip_space) {
            ImGui::Text("Brickdrawing");
          }
          ImGui::NextColumn();
          ImGui::Text("%.3f ms", TimerDatabase::instance().duration("draw") / 1000000.0f);
          ImGui::Text("%.3f ms", TimerDatabase::instance().duration("2integrate") / 1000000.0f);
          ImGui::Text("%.3f ms", TimerDatabase::instance().duration("holefill") / 1000000.0f);
          if(g_skip_space) {
            ImGui::Text("%.3f ms", TimerDatabase::instance().duration("brickdraw") / 1000000.0f);
          }
          ImGui::Columns(1);
          ImGui::TreePop();
        }
        else {
          ImGui::SameLine();
          ImGui::Text("   %.3f ms", full / 1000000.0f);          
        }
      }
      else {
        ImGui::Text("   Reconstruction");
        ImGui::SameLine();
        ImGui::Text("   %.3f ms", TimerDatabase::instance().duration("draw") / 1000000.0f);
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
  // ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
  // ImGui::ShowTestWindow();
}

void frameStep (){
  glfwPollEvents();

  update_gui();


  
  draw3d();

  if(2 != g_stereo_mode){
    ImGui::Render();
  }

  glfwSwapBuffers(g_window);
}

//////////////////////////////////////////////////////////////////////////////////////////
void update_model_matrix(bool load_ident) {
  
  

  gloost::Point3 speed(0.0,0.0,0.0);
  glm::fvec2 speed_button1(g_navi.getOffset(0));
  glm::fvec2 speed_button2(g_navi.getOffset(1));
  float fac = 0.005;
  speed[0] = speed_button1.x * fac;
  speed[1] = speed_button1.y * - 1.0 * fac;
  speed[2] = speed_button2.y * fac;
  gloost::Matrix camview(g_navi.get(speed));
  camview.invert();

  glMatrixMode(GL_MODELVIEW);
  if(load_ident){
    glLoadIdentity();
  }
  glMultMatrixf(camview.data());

  static double curr_rot = 0.0;
  const double TAU = 2.0 * 180.0;
  if(g_animate){
    curr_rot += ImGui::GetIO().DeltaTime * 10.0;
    if (curr_rot >= TAU) curr_rot = 0.0;
  }
  glRotatef(curr_rot, 0.0,1.0,0.0);

  g_navi.resetOffsets();

}

void process_textures() {
  if(g_recon_integration.get() == g_recons.at(g_recon_mode).get()) {
    g_recon_integration->clearOccupiedBricks();
  }      
  g_nka->processTextures();
  if(g_recon_integration.get() == g_recons.at(g_recon_mode).get()) {
    g_recon_integration->updateOccupiedBricks();
  }      
}
//////////////////////////////////////////////////////////////////////////////////////////
  /// main loop function, render with 3D setup
void draw3d(void)
{  


  bool update_textures = false;
  if (g_play) {
    update_textures = update_textures || g_nka->update();
  }
  
  if (update_textures) {
    process_textures();
  }


  // perform integration
  if(g_recon_integration.get() == g_recons.at(g_recon_mode).get()) {
    if(update_textures) {
      g_recon_integration->integrate();
    }
  }


  glClearColor(g_clear_color[0],g_clear_color[1],g_clear_color[2],g_clear_color[3]);



  


  if(g_stereo_mode == 0){ // MONO
    glViewport(0,0,g_screenWidth, g_screenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    g_camera.set();
    update_model_matrix();
    g_recons.at(g_recon_mode)->drawF();
  }
  else if(g_stereo_mode == 1){ // ANAGLYPH STEREO
    glViewport(0,0,g_screenWidth, g_screenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    g_stereo_camera->setLeft();
    update_model_matrix(false);

    // g_recon_integration->setColorMaskMode(1);
    g_recons.at(g_recon_mode)->setColorMaskMode(1);
    g_recons.at(g_recon_mode)->drawF();

    glClear(GL_DEPTH_BUFFER_BIT);
    g_stereo_camera->setRight();
    update_model_matrix(false);
    // g_recon_integration->setColorMaskMode(2);
    g_recons.at(g_recon_mode)->setColorMaskMode(2);
    g_recons.at(g_recon_mode)->drawF();
  }
  else if(g_stereo_mode == 2 && g_fbr){ // SIDE-BY-SIDE STEREO

    sys::feedback fb = g_fbr->get();
    const gloost::Matrix cyclops_mat = glm2gloost(fb.cyclops_mat);
    const gloost::Matrix screen_mat = glm2gloost(fb.screen_mat);
    const gloost::Matrix model_mat = glm2gloost(fb.model_mat);
    g_recon_mode = fb.recon_mode;
    // currently only works without depth aware color filling
    if(1 == g_recon_mode){
      g_recon_integration->setColorFilling(false);
    }

    glViewport(0,0,g_windowWidth, g_windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    g_stereo_camera->setCyclopsMatrix(cyclops_mat);
    g_stereo_camera->setScreenMatrix(screen_mat);

    glViewport(g_left_pos_x, g_left_pos_y, g_screenWidth, g_screenHeight);
    g_stereo_camera->setLeft();
    glMatrixMode(GL_MODELVIEW);
    glMultMatrixf(model_mat.data());
    // g_recon_integration->setViewportOffset((float) g_left_pos_x, (float) g_left_pos_y);
    g_recons.at(g_recon_mode)->setViewportOffset((float) g_left_pos_x, (float) g_left_pos_y);
    g_recons.at(g_recon_mode)->drawF();

    glViewport(g_right_pos_x, g_right_pos_y, g_screenWidth, g_screenHeight);
    g_stereo_camera->setRight();
    glMatrixMode(GL_MODELVIEW);
    glMultMatrixf(model_mat.data());
    // g_recon_integration->setViewportOffset((float) g_right_pos_x, (float) g_right_pos_y);
    g_recons.at(g_recon_mode)->setViewportOffset((float) g_right_pos_x, (float) g_right_pos_y);
    g_recons.at(g_recon_mode)->drawF();

    glViewport(0,0,g_screenWidth, g_screenHeight);
  }


  if(0 == g_stereo_mode){
    if (g_draw_calibvis) {
      g_calibvis->draw();
    }

    if(g_draw_frustums) {
      g_cv->drawFrustums();
    }

    if(g_draw_bricks && g_recon_integration.get() != g_recons.at(g_recon_mode).get()) {
      g_recon_integration->drawOccupiedBricks();
    }
    // draw black grid on floor for fancy look
    if(g_draw_grid) {
      // glPushAttrib(GL_ALL_ATTRIB_BITS);
      // glColor3f(1.0,1.0,1.0);
      // glBegin(GL_LINES);
      // const float lsize = 10.0f;
      // const float gstep = 0.5f;
      // for(float s = -lsize; s <= lsize; s += gstep){
      //   glVertex3f(s, 0.0, -lsize);
      //   glVertex3f(s, 0.0,  lsize);
      
      //   glVertex3f(-lsize, 0.0, s);
      //   glVertex3f( lsize, 0.0, s);
      // }
      
      // glEnd();
      // glPopAttrib();
      g_bbox.draw();
    }
    
    if (g_draw_textures) {
      unsigned num = g_num_texture % 2;
      TextureBlitter::blit(15 + num, glm::fvec2{g_recon_integration->m_view_inpaint->resolution_full()} / 2.0f);
    }
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
  case GLFW_KEY_O:
    g_draw_bricks = !g_draw_bricks;
    g_recon_integration->setDrawBricks(g_draw_bricks);
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
      process_textures();
      g_recon_integration->integrate();
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
  if(g_loaded_conf) {
  time_t t = time(0);   // get time now
  struct tm * now = localtime( & t );
  std::stringstream file_name;

  file_name << g_conf_file.substr(0, g_conf_file.length() - 5) << ","
     << (now->tm_year + 1900) << '-'
     << (now->tm_mon + 1) << '-'
     <<  now->tm_mday << ','
     <<  now->tm_hour << '-'
     <<  now->tm_min
     << ".csv";
    TimerDatabase::instance().writeMean(file_name.str());
    TimerDatabase::instance().writeMin(file_name.str());
    TimerDatabase::instance().writeMax(file_name.str());
  }

  //free globjects 
  globjects::detachAllObjects();

  ImGui_ImplGlfwGLB_Shutdown();
  // free glfw resources
  glfwDestroyWindow(g_window);
  glfwTerminate();

  if(g_stereo_mode > 0){
    delete g_stereo_camera;
  }

  std::exit(status);
}

int main(int argc, char *argv[]) {
  
  CMDParser p("kinect_surface ...");

  p.addOpt("s",2,"screensize", "set screen size in meter");
  p.addOpt("d",2,"displaysize", "set display size in pixel");

  p.addOpt("w",2,"windowsize", "set window size in pixel for stereomode side-by-side");
  p.addOpt("l",2,"leftpos", "set the position of the left viewport (upper left corner) in pixel for stereomode side-by-side");
  p.addOpt("r",2,"rightpos", "set the position of the right viewport (upper left corner) in pixel for stereomode side-by-side");

  p.addOpt("m",1,"stereomode", "set stereo mode 0: none, 1: anaglyph, 2: side-by-side (default: 0)");
  p.addOpt("c",4,"clearcolor", "set clear color (default: 0.0 0.0 0.0 0.0)");

  p.addOpt("f",1,"feedbacksocket", "set socket for feedback receiver (e.g. 127.0.0.1:9000)");
  p.addOpt("p",1,"serversocket", "set server socket for input stream : default " + g_server_socket);
 
  p.init(argc,argv);

  if(p.isOptSet("p")){
    g_server_socket = p.getOptsString("p")[0];
  }
  std::cout << "using server socket for input stream: " << g_server_socket << std::endl;


  if(p.isOptSet("s")){
    g_screenWidthReal  = p.getOptsFloat("s")[0];
    g_screenHeightReal = p.getOptsFloat("s")[1];
  }
  if(p.isOptSet("d")){
    g_screenWidth  = p.getOptsInt("d")[0];
    g_screenHeight = p.getOptsInt("d")[1];
  }
  if(p.isOptSet("w")){
    g_windowWidth  = p.getOptsInt("w")[0];
    g_windowHeight = p.getOptsInt("w")[1];
  }

  if(p.isOptSet("l")){
    g_left_pos_x = p.getOptsInt("l")[0];
    g_left_pos_y = p.getOptsInt("l")[1];
  }
  if(p.isOptSet("r")){
    g_right_pos_x = p.getOptsInt("r")[0];
    g_right_pos_y = p.getOptsInt("r")[1];
  }


  if(p.isOptSet("m")){
    g_stereo_mode = p.getOptsInt("m")[0];
  }

  if(p.isOptSet("c")){
    g_clear_color[0]  = p.getOptsFloat("c")[0];
    g_clear_color[1]  = p.getOptsFloat("c")[1];
    g_clear_color[2]  = p.getOptsFloat("c")[2];
    g_clear_color[3]  = p.getOptsFloat("c")[3];
  }

  if(p.isOptSet("f")){
    std::string client_socket = p.getOptsString("f")[0].c_str();
    init_fbr(client_socket.c_str());
  }

  if((1 == g_stereo_mode) || (2 == g_stereo_mode)){
    init_stereo_camera();
  }

  // load global variables
  init_config(p.getArgs());


  // Setup window
  glfwSetErrorCallback(error_callback);
  if(!glfwInit()) {
    std::exit(EXIT_FAILURE);  
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  if((0 == g_stereo_mode) || (1 == g_stereo_mode)){
    g_window = glfwCreateWindow(g_screenWidth, g_screenHeight, "Kinect Reconstruction", NULL, NULL); 
  }
  else if(2 == g_stereo_mode){
    g_window = glfwCreateWindow(g_windowWidth, g_windowHeight, "Kinect Reconstruction", NULL, NULL); 
  }

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
  if(0 == g_stereo_mode){
    glfwSetFramebufferSizeCallback(g_window, update_view);
  }
  // allow unlimited mouse movement

  // Initialize globjects (internally initializes glbinding, and registers the current context)
  globjects::init();

  watch_gl_errors(g_watch_errors);

  // set some gl states
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // load and intialize objects
  init(p.getArgs());


  //  update_view(g_window, g_screenWidth, g_screenHeight);
  g_aspect       = g_screenWidth * 1.0/g_screenHeight;
  g_camera.setAspect(g_aspect);
  g_navi.resize(g_screenWidth, g_screenHeight);
  g_navi.setZoom(g_zoom);
  for (auto& recon : g_recons) {
    recon->resize(g_screenWidth, g_screenHeight);
  }


  //start of rendering
  auto time_start =  std::chrono::high_resolution_clock::now();

  while (!glfwWindowShouldClose(g_window)) {
    frameStep();
    // keep track fo time if config was loaded
    if(g_loaded_conf) {
      unsigned time_in_s = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - time_start).count();
      if(time_in_s >= g_time_limit) {
        quit(EXIT_SUCCESS);
      }
    }
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
