#include "reconstruction.hpp"

#include "calibration_files.hpp"
#include "CalibVolumes.hpp"
#include "timer_database.hpp"

#include <glbinding/gl/gl.h>
using namespace gl;
#include <globjects/NamedString.h>
#include <globjects/base/File.h>

namespace kinect {

Reconstruction::Reconstruction(CalibrationFiles const& cfs, CalibVolumes const* cv, gloost::BoundingBox const&  bbox)
 :m_cv(cv)
 ,m_cf(&cfs)
 ,m_tex_width{cfs.getWidth()}
 ,m_tex_height{cfs.getHeight()}
 ,m_num_kinects{cfs.num()}
 ,m_min_length{cfs.minLength()}
 ,m_bbox{bbox}
 ,m_color_mask_mode(0)
{
   globjects::NamedString::create("/shading.glsl", new globjects::File("glsl/shading.glsl"));
   TimerDatabase::instance().addTimer("3recon");
   TimerDatabase::instance().addTimer("draw");
}

void Reconstruction::reload() {

}

void  Reconstruction::resize(std::size_t width, std::size_t height) {
}
void Reconstruction::drawF() {
  TimerDatabase::instance().begin("draw");
  draw();
  TimerDatabase::instance().end("draw");
}

glm::uvec2 Reconstruction::getViewportRes() {
  auto params = getViewport();
  return glm::uvec2{params[3], params[4]};
}
glm::uvec4 Reconstruction::getViewport() {
  gl::GLsizei vp_params[4];
  glGetIntegerv(GL_VIEWPORT,vp_params);
  return glm::uvec4{vp_params[0], vp_params[1], vp_params[2], vp_params[3]};
}

  void Reconstruction::setColorMaskMode(unsigned mode){
    m_color_mask_mode = mode;
  }
  void Reconstruction::setViewportOffset(float x, float y){
    std::cerr << "Reconstruction::setViewportOffset(float x, float y) -> implement me in derived class!" << std::endl;
  }
}
