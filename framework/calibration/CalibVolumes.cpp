#include "CalibVolumes.hpp"
#include <KinectCalibrationFile.h>
#include <timevalue.h>

#include <glbinding/gl/functions-patches.h>
#include <globjects/Shader.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>

#include <stdexcept>

namespace kinect{

static glm::uvec3 volume_res{128,256,128};
static int start_image_unit = 1;

CalibVolumes::CalibVolumes(std::vector<std::string> const& calib_volume_files, gloost::BoundingBox const& bbox)
 :m_cv_xyz_filenames()
 ,m_cv_uv_filenames()
 ,m_volumes_xyz{}
 ,m_volumes_uv{}
 ,m_volumes_xyz_inv{}
 ,m_frustums{}
 ,m_bbox{bbox}
 ,m_start_texture_unit(-1)
 ,m_start_texture_unit_inv(-1)
{
  for(auto const& calib_file : calib_volume_files){
  	std::string basefile = calib_file;
  	basefile.replace( basefile.end() - 3, basefile.end(), "");
  	m_cv_xyz_filenames.push_back(basefile + "cv_xyz");
  	m_cv_uv_filenames.push_back(basefile + "cv_uv");
  }

  for(unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i){
    addVolume(m_cv_xyz_filenames[i], m_cv_uv_filenames[i]);
  }

  createVolumeTextures();
}

CalibVolumes::~CalibVolumes(){
  for(unsigned i = 0; i < m_volumes_xyz.size(); ++i){
    m_volumes_xyz[i]->destroy();
    m_volumes_uv[i]->destroy();
  }
  for(unsigned i = 0; i < m_volumes_xyz_inv.size(); ++i){
    m_volumes_xyz_inv[i]->destroy();
  }
}

void CalibVolumes::loadInverseCalibs(std::string const& path) {
  for (unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i){
    std::string name_source{m_cv_xyz_filenames[i].substr( m_cv_xyz_filenames[i].find_last_of("/\\") + 1)};
    std::string name_input{path + name_source + "_inv"};
    std::cout << "loading " << name_input << std::endl;
    m_data_volumes_xyz_inv.emplace_back(name_input);
    auto const& calib(m_data_volumes_xyz_inv.back());
    std::cout << "dimensions xyz - " << calib.res().x << ", " << calib.res().y << ", " << calib.res().z 
              << " minmax d - " << calib.depthLimits().x << ", " << calib.depthLimits().y << std::endl;
  }

  for (auto const& calib : m_data_volumes_xyz_inv) {
    auto volume_xyz_inv = globjects::Texture::createDefault(GL_TEXTURE_3D);
    volume_xyz_inv->image3D(0, GL_RGBA32F, glm::ivec3{calib.res()}, 0, GL_RGBA, GL_FLOAT, calib.volume().data());
    m_volumes_xyz_inv.emplace_back(volume_xyz_inv);
  }
}

std::vector<int> CalibVolumes::getXYZVolumeUnitsInv() const {
  std::vector<int> units(5, 0);
  for(int i = 0; i < int(m_cv_xyz_filenames.size()); ++i) {
    units[i] = m_start_texture_unit_inv + i;
  }
  return units;
}

glm::uvec3 CalibVolumes::getVolumeRes() const {
  return m_data_volumes_xyz_inv[0].res();
}

glm::fvec2 CalibVolumes::getDepthLimits(unsigned i) const {
  return m_data_volumes_xyz[i].depthLimits();
}

static std::array<glm::fvec3, 8> getCornerPoints(CalibrationVolume<xyz> const& curr_volume) {
  glm::uvec3 end_points{curr_volume.res() - glm::uvec3{1}};
  std::array<glm::fvec3, 8> points_corner{};
  // 
  points_corner[0] = (curr_volume(0,          0,          0));
  points_corner[1] = (curr_volume(0,          end_points.y, 0));
  points_corner[2] = (curr_volume(end_points.x, end_points.y, 0));
  points_corner[3] = (curr_volume(end_points.x, 0,          0));

  points_corner[4] = (curr_volume(0,          0,          end_points.z));
  points_corner[5] = (curr_volume(0,          end_points.y, end_points.z));
  points_corner[6] = (curr_volume(end_points.x, end_points.y, end_points.z));
  points_corner[7] = (curr_volume(end_points.x, 0,          end_points.z));

  return points_corner;
}

void CalibVolumes::addVolume(std::string const& filename_xyz, std::string const& filename_uv) {
  std::cout << "loading " << filename_xyz << std::endl;
  m_data_volumes_xyz.emplace_back(filename_xyz);
  auto const& calib_xyz(m_data_volumes_xyz.back());
  std::cout << "dimensions xyz - " << calib_xyz.res().x << ", " << calib_xyz.res().y << ", " << calib_xyz.res().z 
            << " minmax d - " << calib_xyz.depthLimits().x << ", " << calib_xyz.depthLimits().y << std::endl;

  m_frustums.emplace_back(getCornerPoints(calib_xyz));

  std::cout << "loading " << filename_uv << std::endl;
  m_data_volumes_uv.emplace_back(filename_uv);
  auto const& calib_uv(m_data_volumes_uv.back());
  std::cout << "dimensions uv - " << calib_uv.res().x << ", " << calib_uv.res().y << ", " << calib_uv.res().z 
            << " minmax d - " << calib_uv.depthLimits().x << ", " << calib_uv.depthLimits().y << std::endl;

}

void CalibVolumes::createVolumeTextures() {
  for(unsigned i = 0; i < m_data_volumes_xyz.size(); ++i){
    auto const& calib_xyz = m_data_volumes_xyz[i];
    auto volume_xyz = globjects::Texture::createDefault(GL_TEXTURE_3D);
    volume_xyz->image3D(0, GL_RGB32F, glm::ivec3{calib_xyz.res()}, 0, GL_RGB, GL_FLOAT, calib_xyz.volume().data());
    m_volumes_xyz.push_back(volume_xyz);
    
    auto const& calib_uv = m_data_volumes_uv[i];
    auto volume_uv = globjects::Texture::createDefault(GL_TEXTURE_3D);
    volume_uv->image3D(0, GL_RG32F, glm::ivec3{calib_uv.res()}, 0, GL_RG, GL_FLOAT, calib_uv.volume().data());
    m_volumes_uv.push_back(volume_uv);
  }
}

std::vector<int> CalibVolumes::getXYZVolumeUnits() const {
  std::vector<int> units(5, 0);
  for(int i = 0; i < int(m_cv_xyz_filenames.size()); ++i) {
    units[i] = m_start_texture_unit + i * 2;
  }
  return units;
}
std::vector<int> CalibVolumes::getUVVolumeUnits() const {
  std::vector<int> units(5, 0);
  for(int i = 0; i < int(m_cv_xyz_filenames.size()); ++i) {
    units[i] = m_start_texture_unit + i * 2 + 1;
  }
  return units;
}

void
CalibVolumes::bindToTextureUnits() {
  for(unsigned layer = 0; layer < m_cv_xyz_filenames.size(); ++layer){
    m_volumes_xyz[layer]->bindActive(GL_TEXTURE0 + m_start_texture_unit + layer * 2);
    m_volumes_uv[layer]->bindActive(GL_TEXTURE0 + m_start_texture_unit + layer * 2 + 1);
  }
  glActiveTexture(GL_TEXTURE0);
}

void
CalibVolumes::bindToTextureUnitsInv() {
  for(unsigned layer = 0; layer < m_cv_xyz_filenames.size(); ++layer){
    m_volumes_xyz_inv[layer]->bindActive(GL_TEXTURE0 + m_start_texture_unit_inv + layer);
  }
  glActiveTexture(GL_TEXTURE0);
}

void CalibVolumes::setStartTextureUnit(unsigned start_texture_unit) {
  m_start_texture_unit = start_texture_unit;
  bindToTextureUnits();
}

void CalibVolumes::setStartTextureUnitInv(unsigned start_texture_unit) {
  m_start_texture_unit_inv = start_texture_unit;
  bindToTextureUnitsInv();
}

void CalibVolumes::drawValidVoxels() const {
  glm::fvec3 bbox_dimensions = glm::fvec3{m_bbox.getPMax()[0] - m_bbox.getPMin()[0],
                                        m_bbox.getPMax()[1] - m_bbox.getPMin()[1],
                                        m_bbox.getPMax()[2] - m_bbox.getPMin()[2]};
  glm::fvec3 bbox_translation = glm::fvec3{m_bbox.getPMin()[0], m_bbox.getPMin()[1], m_bbox.getPMin()[2]};

  glm::fvec3 const& dims{m_data_volumes_xyz_inv[0].res()};
  glPointSize(1.0f);
  glBegin(GL_POINTS);
  for(unsigned x = 0; x < dims.x; ++x) {
    for(unsigned y = 0; y < dims.y; ++y) {
      for(unsigned z = 0; z < dims.z; ++z) {
        glm::fvec3 pos{glm::fvec3{x,y,z} / dims * bbox_dimensions + bbox_translation};
        bool inside = true;
        for(unsigned i = 0; i < m_frustums.size(); ++i) {
           inside &= m_frustums[i].inside(pos);
        }
        if (inside) {
          glVertex3f(pos.x, pos.y, pos.z);
        }
      }
    }
  }
  glEnd();
}

void CalibVolumes::drawFrustums() const {
  for(unsigned i = 0; i < m_volumes_xyz.size(); ++i) {
    m_frustums[i].draw();
  }
}

Frustum const& CalibVolumes::getFrustum(unsigned i) const {
  return m_frustums[i];
}

std::vector<glm::fvec3> CalibVolumes::getCameraPositions() const {
  std::vector<glm::fvec3> cam_positions{};
  for (auto const& frustum : m_frustums) {
    cam_positions.emplace_back(frustum.getCameraPos());
  }
  return cam_positions;
}

gloost::BoundingBox const& CalibVolumes::getBBox() const {
  return m_bbox;
}

}