#include "recon_integration.hpp"

#include "calibration_files.hpp"
#include "unit_cube.hpp"
#include "screen_quad.hpp"
#include <KinectCalibrationFile.h>
#include "CalibVolumes.hpp"
#include "view_lod.hpp"
#include "texture_blitter.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/io.hpp>

#include <glbinding/gl/gl.h>
using namespace gl;
#include <globjects/Buffer.h>
#include <globjects/Program.h>
#include <globjects/VertexArray.h>
#include <globjects/Texture.h>
#include <globjects/Framebuffer.h>

#include <globjects/Shader.h>
#include <globjects/globjects.h>

// #define VERT_FILL

namespace kinect{
static int start_image_unit = 3;

ReconIntegration::ReconIntegration(CalibrationFiles const& cfs, CalibVolumes const* cv, gloost::BoundingBox const&  bbox, float limit, float size)
 :Reconstruction(cfs, cv, bbox)
 ,m_view_inpaint{new ViewLod(1280, 720)}
 ,m_view_inpaint2{new ViewLod(1280, 720)}
 ,m_view_raymarch{new View(1280, 720, {GL_RGBA32F, GL_R32F})}
 ,m_view_depth{new View(1280, 720, false)}
 ,m_buffer_bricks{new globjects::Buffer()}
 ,m_program{new globjects::Program()}
 ,m_program_integration{new globjects::Program()}
 ,m_program_inpaint{new globjects::Program()}
 ,m_program_colorfill{new globjects::Program()}
 ,m_program_transfer{new globjects::Program()}
 ,m_program_solid{new globjects::Program()}
 ,m_program_bricks{new globjects::Program()}
 ,m_res_volume{0}
 ,m_res_bricks{0}
 ,m_sampler{glm::uvec3{0}}
 ,m_volume_tsdf{globjects::Texture::createDefault(GL_TEXTURE_3D)}
 ,m_mat_vol_to_world{1.0f}
 ,m_limit{limit}
 ,m_voxel_size{size}
 ,m_brick_size{0.1f}
 ,m_fill_holes{true}
 ,m_use_bricks{true}
 ,m_skip_space{true}
 ,m_draw_bricks{false}
 ,m_timer_integration{}
 ,m_timer_holefill{}
 ,m_timer_brickdraw{}
 ,m_ratio_occupied{0.0f}
{
  m_program->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/tsdf_raymarch.vs"),
    globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/tsdf_raymarch.fs")
  );

  glm::fvec3 bbox_dimensions = glm::fvec3{m_bbox.getPMax()[0] - m_bbox.getPMin()[0],
                                          m_bbox.getPMax()[1] - m_bbox.getPMin()[1],
                                          m_bbox.getPMax()[2] - m_bbox.getPMin()[2]};
  glm::fvec3 bbox_translation = glm::fvec3{m_bbox.getPMin()[0], m_bbox.getPMin()[1], m_bbox.getPMin()[2]};

  m_mat_vol_to_world = glm::scale(glm::fmat4{1.0f}, bbox_dimensions);
  m_mat_vol_to_world = glm::translate(glm::fmat4{1.0f}, bbox_translation) * m_mat_vol_to_world;

  m_program->setUniform("vol_to_world", m_mat_vol_to_world);
  m_program->setUniform("kinect_colors",1);
  m_program->setUniform("kinect_depths",2);
  m_program->setUniform("kinect_qualities",3);
  m_program->setUniform("kinect_normals",4);
  m_program->setUniform("cv_xyz_inv", m_cv->getXYZVolumeUnitsInv());
  m_program->setUniform("cv_uv", m_cv->getUVVolumeUnits());
  m_program->setUniform("camera_positions", m_cv->getCameraPositions());
  m_program->setUniform("num_kinects", m_num_kinects);
  m_program->setUniform("limit", m_limit);
  m_program->setUniform("depth_peels", 17);
  m_program->setUniform("skipSpace", m_skip_space);
  
  m_program_integration->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/tsdf_integration.vs")
  );
  m_program_integration->setUniform("cv_xyz_inv", m_cv->getXYZVolumeUnitsInv());
  m_program->setUniform("volume_tsdf", 29);

  m_program_integration->setUniform("volume_tsdf", start_image_unit);
  m_program_integration->setUniform("kinect_colors",1);
  m_program_integration->setUniform("kinect_depths",2);
  m_program_integration->setUniform("kinect_qualities",3);
  m_program_integration->setUniform("kinect_normals",4);
  m_program_integration->setUniform("kinect_silhouettes",5);

  m_program_integration->setUniform("num_kinects", m_num_kinects);
  m_program_integration->setUniform("res_depth", glm::uvec2{m_cf->getWidth(), m_cf->getHeight()});
  m_program_integration->setUniform("limit", m_limit);
  
  m_program_inpaint->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/texture_passthrough.vs"),
    globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/tsdf_inpaint.fs")
  );
  m_program_inpaint->setUniform("texture_color", 15);
  m_program_inpaint->setUniform("texture_depth", 16);

  m_program_transfer->attach(
   globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/texture_passthrough.vs")
  ,globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/framebuffer_transfer.fs")
  );
  m_program_transfer->setUniform("texture_color", 15);
  m_program_transfer->setUniform("texture_depth", 16);

  m_program_colorfill->attach(
   globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/texture_passthrough.vs")
  ,globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/tsdf_colorfill.fs")
  );
  m_program_colorfill->setUniform("texture_color", 15);
  m_program_colorfill->setUniform("texture_depth", 16);
  m_program_colorfill->setUniform("out_tex", start_image_unit);

  m_program_solid->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/solid.vs"),
    globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/solid.fs")
  );

  m_program_bricks->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/bricks.vs"),
    globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/bricks.fs")
  );

  setVoxelSize(m_voxel_size);
  // initialize with 1 to get minimal depth
  m_view_depth->setClearColor(glm::fvec4{1.0f, 0.0f, 0.0f, 0.0f});
}

void ReconIntegration::drawF() {
  // draw depth limits for space skipping
  if (m_skip_space && m_use_bricks) {
    drawDepthLimits();
  }

  Reconstruction::drawF();
  
  if (m_fill_holes) {
    m_timer_holefill.begin();
    fillColors();
    m_timer_holefill.end();
  }


  if (m_draw_bricks) {
    drawBricks();
  }
  // bind to units for displaying in gui
  m_view_raymarch->bindToTextureUnitsRGBA(16);
  m_view_depth->bindToTextureUnits(16);
}

void ReconIntegration::draw(){
  m_program->use();

  gloost::Matrix modelview;
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview.data());
  glm::fmat4 model_view{modelview};
  glm::fmat4 normal_matrix = glm::inverseTranspose(model_view * m_mat_vol_to_world);
  m_program->setUniform("NormalMatrix", normal_matrix);
  // upload camera pos in volume space for correct raycasting dir 
  glm::fvec4 camera_world{glm::inverse(model_view) * glm::fvec4{0.0f, 0.0f, 0.0f, 1.0f}};
  glm::vec3 camera_texturespace{glm::inverse(m_mat_vol_to_world) * camera_world};

  m_program->setUniform("CameraPos", camera_texturespace);

  m_view_raymarch->enable();
  if (m_fill_holes) {
    // m_view_inpaint->enable();
  }
  UnitCube::draw();
  m_program->release();
  m_view_raymarch->disable();
  m_view_raymarch->bindToTextureUnits(15);
  m_program_transfer->setUniform("resolution_tex", m_view_raymarch->resolution());
  m_program_transfer->setUniform("texture_depth", 17);
  m_program_transfer->use();
  m_program_transfer->setUniform("lod", int(0));
  if (m_fill_holes) {
    // m_view_inpaint->disable();
    m_view_inpaint->enable(0);
    ScreenQuad::draw();
    m_view_inpaint->disable();
  }
  else {
    ScreenQuad::draw(); 
  }
  m_program_transfer->release();  
  m_program_transfer->setUniform("resolution_tex", m_view_inpaint->resolution_full());
  m_program_transfer->setUniform("texture_depth", 16);
}

void ReconIntegration::integrate() {
  m_timer_integration.begin();

  glEnable(GL_RASTERIZER_DISCARD);
  m_program_integration->use();

  // clearing costs 0,4 ms on titan
  float negative = -m_limit;
  m_volume_tsdf->clearImage(0, GL_RED, GL_FLOAT, &negative);

  m_volume_tsdf->bindImageTexture(start_image_unit, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);
  
  if (m_use_bricks) {
    // load occupied brick info
    m_buffer_bricks->getSubData(sizeof(unsigned) * 8, m_active_bricks.size() * sizeof(unsigned), m_active_bricks.data());

    unsigned num_occupied = 0;
    for(unsigned i = 0; i < m_bricks.size(); ++i) {
      if(m_active_bricks[i] > 0) {
        m_sampler.sample(m_bricks[i].indices);
        ++num_occupied;
      }
    }
    m_ratio_occupied = num_occupied / float(m_active_bricks.size());
  }
  else {
    m_sampler.sample();
  }

  m_program_integration->release();
  glDisable(GL_RASTERIZER_DISCARD);
  
  m_timer_integration.end();
  // clear active bricks
  static unsigned zero = 0;
  m_buffer_bricks->clearSubData(GL_R32UI, sizeof(unsigned) * 8, m_bricks.size() * sizeof(unsigned), GL_RED_INTEGER, GL_UNSIGNED_INT, &zero);
}

void ReconIntegration::fillColors() {
  glDepthFunc(GL_ALWAYS);
  // transfer textures
  m_view_inpaint->bindToTextureUnits(15);
  m_view_inpaint2->enable(0);
  m_program_transfer->use();
  m_program_transfer->setUniform("lod", int(0));
  ScreenQuad::draw();
  m_view_inpaint2->disable();
  m_program_transfer->release();
  std::swap(m_view_inpaint, m_view_inpaint2);

  for(unsigned i = 1; i < m_view_inpaint->numLods(); ++i) {
    m_view_inpaint->bindToTextureUnits(15);
    // unsigned i = 1;
    // calculate next lod
    m_view_inpaint2->enable(i, false, false);
    m_program_inpaint->use();
    m_program_inpaint->setUniform("lod", int(i - 1));
    ScreenQuad::draw();
    m_view_inpaint2->disable();
    m_program_inpaint->release();
    std::swap(m_view_inpaint, m_view_inpaint2);
    
   // transfer textures
    m_view_inpaint->bindToTextureUnits(15);
    m_view_inpaint2->enable(0);
    m_program_transfer->use();
    m_program_transfer->setUniform("lod", int(0));
    ScreenQuad::draw();
    m_view_inpaint2->disable();
    m_program_transfer->release();
    std::swap(m_view_inpaint, m_view_inpaint2);
  }
  glDepthFunc(GL_LESS);
  // tranfer to default framebuffer
  m_view_inpaint2->bindToTextureUnits(15);
  m_program_colorfill->use();
  m_program_colorfill->setUniform("resolution_tex", m_view_inpaint->resolution(0));
  m_program_colorfill->setUniform("lod", 0);

  ScreenQuad::draw();
  m_program_colorfill->release();
}

void ReconIntegration::setVoxelSize(float size) {
  m_voxel_size = size;
  m_res_volume = glm::ceil(glm::fvec3{m_bbox.getPMax()[0] - m_bbox.getPMin()[0],
                                      m_bbox.getPMax()[1] - m_bbox.getPMin()[1],
                                      m_bbox.getPMax()[2] - m_bbox.getPMin()[2]} / m_voxel_size);
  m_sampler.resize(m_res_volume);
  m_program_integration->setUniform("res_tsdf", m_res_volume);
  m_volume_tsdf->image3D(0, GL_R32F, glm::ivec3{m_res_volume}, 0, GL_RED, GL_FLOAT, nullptr);
  m_volume_tsdf->bindActive(GL_TEXTURE0 + 29);
  std::cout << "resolution " << m_res_volume.x << ", " << m_res_volume.y << ", " << m_res_volume.z
    << " - " << (m_res_volume.x * m_res_volume.y * m_res_volume.z) / 1000 << "k voxels" << std::endl;

  divideBox();
}

bool point_in_brick(glm::fvec3 const& point, glm::fvec3 const& pos_n, glm::fvec3 const& size_n) {
  return point.x >= pos_n.x && point.y >= pos_n.y && point.z >= pos_n.z
      && point.x <= pos_n.x + size_n.x && point.y <= pos_n.y + size_n.y && point.z <= pos_n.z + size_n.z;
}

void ReconIntegration::divideBox() {
  m_bricks.clear();
  glm::fvec3 min{m_bbox.getPMin()};
  glm::fvec3 size{glm::fvec3{m_bbox.getPMax()} - min}; 
  glm::fvec3 start{min};
  while(size.z - start.z  + min.z > 0.0f) {
    while(size.y - start.y  + min.y > 0.0f) {
      while(size.x - start.x  + min.x > 0.0f) {
        m_bricks.emplace_back(start, glm::min(glm::fvec3{m_brick_size}, size - start + min));
        auto& curr_brick = m_bricks.back();
        curr_brick.indices = m_sampler.containedVoxels((curr_brick.pos - min) / size, curr_brick.size / size);
        start.x += m_brick_size;
      }
      start.x = min.x;
      start.y += m_brick_size;
    }
    start.y = min.y;
    start.z += m_brick_size;
  }
  m_res_bricks = glm::uvec3{glm::ceil(size / m_brick_size)};
  std::vector<unsigned> bricks(m_bricks.size() + 8, 0);
  std::memcpy(&bricks[0], &m_brick_size, sizeof(float));
  std::memcpy(&bricks[4], &m_res_bricks, sizeof(unsigned) * 3);
  // bricks[1] = m_brick_size;
  // for(unsigned i = 5; i < bricks.size(); ++i) {
  //   bricks[i] = i;
  // }
  m_buffer_bricks->setData(sizeof(unsigned) * bricks.size(), bricks.data(), GL_DYNAMIC_READ);
  m_buffer_bricks->bindRange(GL_SHADER_STORAGE_BUFFER, 3, 0, sizeof(unsigned) * bricks.size());
  m_active_bricks.resize(m_bricks.size());

  std::cout << "brick res " << m_res_bricks.x << ", " << m_res_bricks.y << ", " << m_res_bricks.z
    << " - " << m_bricks.front().indices.size() << " voxels per brick" << std::endl;
}

void ReconIntegration::drawDepthLimits() {
  m_view_depth->enable();
  gloost::Matrix projection_matrix;
  glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix.data());
  gloost::Matrix viewport_translate;
  viewport_translate.setIdentity();
  viewport_translate.setTranslate(1.0,1.0,1.0);
  gloost::Matrix viewport_scale;
  viewport_scale.setIdentity();
  
  glm::uvec4 viewport_vals{getViewport()};
  viewport_scale.setScale(viewport_vals[2] * 0.5, viewport_vals[3] * 0.5, 0.5f);
  gloost::Matrix image_to_eye =  viewport_scale * viewport_translate * projection_matrix;
  image_to_eye.invert();
  m_program->setUniform("img_to_eye_curr", image_to_eye);

  m_timer_brickdraw.begin();
  m_program_bricks->use();
  glEnable(GL_BLEND);
  glBlendEquation(GL_MIN);

  for(unsigned i = 0; i < m_bricks.size(); ++i) {
    if(m_active_bricks[i] > 0) {
      glm::fmat4 transform = glm::scale(glm::translate(glm::fmat4{1.0f}, m_bricks[i].pos), m_bricks[i].size);
      m_program_bricks->setUniform("transform", transform);
      UnitCube::draw();
    }
  }
  m_program_bricks->release();
 
  m_view_depth->disable();
  m_view_depth->bindToTextureUnits(17);
  glDisable(GL_BLEND);
  
  m_timer_brickdraw.end();
}

void ReconIntegration::drawBricks() {
  m_program_solid->use();
  m_program_solid->setUniform("Color", glm::fvec3{1.0f, 0.0f, 0.0f});

  for(unsigned i = 0; i < m_bricks.size(); ++i) {
    if(m_active_bricks[i] > 0) {
      glm::fmat4 transform = glm::scale(glm::translate(glm::fmat4{1.0f}, m_bricks[i].pos), m_bricks[i].size);
      m_program_bricks->setUniform("transform", transform);
      UnitCube::drawWire();
    }
  }
  m_program_bricks->release();
}

void ReconIntegration::drawBrickVoxels() const {
  m_program_solid->use();
  m_program_solid->setUniform("Color", glm::fvec3{0.0f, 1.0f, 0.0f});
  // for(auto const& brick: m_bricks) {
    glm::fmat4 transform = glm::scale(glm::translate(glm::fmat4{1.0f}, glm::fvec3{m_bbox.getPMin()}), glm::fvec3{m_bbox.getPMax()} - glm::fvec3{m_bbox.getPMin()});
    m_program_solid->setUniform("transform", transform);
    m_sampler.sample(m_bricks[5].indices);
  // }
  m_program_solid->release();
}

void ReconIntegration::setTsdfLimit(float limit) {
  m_limit = limit;
  m_program->setUniform("limit", m_limit);
  m_program_integration->setUniform("limit", m_limit);
}

void ReconIntegration::setBrickSize(float size) {
  m_brick_size = size;
  divideBox();
}

std::uint64_t ReconIntegration::integrationTime() const {
  return m_timer_integration.duration();
}

std::uint64_t ReconIntegration::holefillTime() const {
  return m_timer_holefill.duration();
}

std::uint64_t ReconIntegration::brickDrawTime() const {
  return m_timer_brickdraw.duration();
}

float ReconIntegration::occupiedRatio() const {
  return m_ratio_occupied;
}

void ReconIntegration::resize(std::size_t width, std::size_t height) {
  m_view_inpaint->setResolution(width, height);
  m_view_inpaint2->setResolution(width, height);
  m_view_raymarch->setResolution(width, height);
  m_view_depth->setResolution(width, height);
  
  m_program->setUniform("viewportSizeInv", glm::fvec2(1.0f/width, 1.0f/height));

  m_program_colorfill->setUniform("num_lods", int(m_view_inpaint->numLods()));
  m_program_colorfill->setUniform("texture_offsets", m_view_inpaint->offsets());
  m_program_colorfill->setUniform("texture_resolutions", m_view_inpaint->resolutions());
  m_program_inpaint->setUniform("texture_offsets", m_view_inpaint->offsets());
  m_program_inpaint->setUniform("texture_resolutions", m_view_inpaint->resolutions());
  
  m_program_colorfill->setUniform("resolution_inv", 1.0f / glm::fvec2{m_view_inpaint->resolution_full()});
  m_program_inpaint->setUniform("resolution_inv", 1.0f / glm::fvec2{m_view_inpaint->resolution_full()});
  m_program_transfer->setUniform("resolution_tex", m_view_inpaint->resolution_full());
}

void ReconIntegration::setColorFilling(bool active) {
  m_fill_holes = active;
}

void ReconIntegration::setUseBricks(bool active) {
  m_use_bricks = active;
}

void ReconIntegration::setDrawBricks(bool active) {
  m_draw_bricks = active;
}

void ReconIntegration::setSpaceSkip(bool active) {
  m_program->setUniform("skipSpace", active);
  m_skip_space = active;
}

}