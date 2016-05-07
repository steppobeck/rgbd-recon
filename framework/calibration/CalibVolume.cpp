#include "CalibVolume.h"
#include <KinectCalibrationFile.h>
#include <timevalue.h>

#include <glbinding/gl/functions-patches.h>

namespace kinect{

/*static*/ std::string CalibVolume::serverendpoint("tcp://141.54.147.22:7001");

CalibVolume::CalibVolume(std::vector<std::string> const& calib_volume_files):
  m_cv_xyz_filenames(),
  m_cv_uv_filenames(),
  m_volumes_xyz{},
  m_volumes_uv{},
  m_cv_widths(),
  m_cv_heights(),
  m_cv_depths(),
  m_cv_min_ds(),
  m_cv_max_ds(),
  m_buffer_minmax_d{new globjects::Buffer()},
  m_start_texture_unit(-1)
{
  for(auto const& calib_file : calib_volume_files){
  	std::string basefile = calib_file;
  	basefile.replace( basefile.end() - 3, basefile.end(), "");
  	m_cv_xyz_filenames.push_back(basefile + "cv_xyz");
  	m_cv_uv_filenames.push_back(basefile + "cv_uv");
  }

  m_buffer_minmax_d->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  reload();
}

/*virtual*/
CalibVolume::~CalibVolume(){
  for(unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i){
    m_volumes_xyz[i]->destroy();
    m_volumes_uv[i]->destroy();
  }
}

void CalibVolume::reload(){
  if (!m_volumes_xyz.empty()) { 
    for(unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i){
      m_volumes_xyz[i]->destroy();
      m_volumes_uv[i]->destroy();
    }
  }

  m_cv_widths.clear();
  m_cv_heights.clear();
  m_cv_depths.clear();
  m_cv_min_ds.clear();
  m_cv_max_ds.clear();
  m_volumes_xyz.clear();
  m_volumes_uv.clear();

  for(unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i){
    addVolume(m_cv_xyz_filenames[i], m_cv_uv_filenames[i]);
  }

  if(m_start_texture_unit >= 0) {
    bindToTextureUnits(m_start_texture_unit);
  }
  uploadMinMaXDepths();
}

void CalibVolume::addVolume(std::string const& filename_xyz, std::string filename_uv) {
  unsigned width_xyz = 0;
  unsigned height_xyz = 0;
  unsigned depth_xyz = 0;

  unsigned width_uv = 0;
  unsigned height_uv = 0;
  unsigned depth_uv = 0;

  float min_d_xyz = 0.0f;
  float max_d_xyz = 0.0f;
  float min_d_uv = 0.0f;
  float max_d_uv = 0.0f;

  std::cerr << "loading " << filename_xyz << std::endl;
  FILE* file_xyz = fopen( filename_xyz.c_str(), "rb");
  fread(&width_xyz, sizeof(unsigned), 1, file_xyz);
  fread(&height_xyz, sizeof(unsigned), 1, file_xyz);
  fread(&depth_xyz, sizeof(unsigned), 1, file_xyz);
  fread(&min_d_xyz, sizeof(float), 1, file_xyz);
  fread(&max_d_xyz, sizeof(float), 1, file_xyz);
  std::cout << "dimensions xyz - " << width_xyz << ", " << height_xyz << ", " << depth_xyz << " minmax d - " << min_d_xyz << ", " << max_d_xyz << std::endl;
  std::vector<xyz> storage_xyz(width_xyz * height_xyz * depth_xyz);
  fread(storage_xyz.data(), sizeof(xyz), width_xyz * height_xyz * depth_xyz, file_xyz);
  fclose(file_xyz);

  std::cerr << "loading " << filename_uv << std::endl;
  FILE* file_uv = fopen( filename_uv.c_str(), "rb");
  fread(&width_uv, sizeof(unsigned), 1, file_uv);
  fread(&height_uv, sizeof(unsigned), 1, file_uv);
  fread(&depth_uv, sizeof(unsigned), 1, file_uv);
  fread(&min_d_uv, sizeof(float), 1, file_uv);
  fread(&max_d_uv, sizeof(float), 1, file_uv);
  std::cout << "dimensions uv - " << width_uv << ", " << height_uv << ", " << depth_uv << " minmax d - " << min_d_uv << ", " << max_d_uv << std::endl;
  std::vector<uv> storage_uv(width_uv * height_uv * depth_uv);
  fread(storage_uv.data(), sizeof(uv), width_uv * height_uv * depth_uv, file_uv);
  fclose(file_uv);
  
  auto volume_xyz = globjects::Texture::createDefault(GL_TEXTURE_3D);
  volume_xyz->image3D(0, GL_RGB32F, width_xyz, height_xyz, depth_xyz, 0, GL_RGB, GL_FLOAT, storage_xyz.data());
  auto volume_uv = globjects::Texture::createDefault(GL_TEXTURE_3D);
  volume_uv->image3D(0, GL_RG32F, width_uv, height_uv, depth_uv, 0, GL_RG, GL_FLOAT, storage_uv.data());

  m_volumes_xyz.push_back(volume_xyz);
  m_volumes_uv.push_back(volume_uv);

  m_cv_widths.push_back(width_xyz);
  m_cv_heights.push_back(height_xyz);
  m_cv_depths.push_back(depth_xyz);
  m_cv_min_ds.push_back(min_d_xyz);
  m_cv_max_ds.push_back(max_d_xyz);
}

void CalibVolume::uploadMinMaXDepths() const {
  auto buffer = m_cv_min_ds;
  // 5 kinects max per pc
  buffer.resize(5);
  copy(m_cv_max_ds.begin(),m_cv_max_ds.end(),std::back_inserter(buffer));
  m_buffer_minmax_d->setStorage(buffer, GL_MAP_WRITE_BIT);
}

std::vector<int> CalibVolume::getXYZVolumeUnits() const {
  std::vector<int> units(5, 0);
  for(int i = 0; i < int(m_cv_xyz_filenames.size()); ++i) {
    units[i] = m_start_texture_unit + i * 2;
  }
  return units;
}
std::vector<int> CalibVolume::getUVVolumeUnits() const {
  std::vector<int> units(5, 0);
  for(int i = 0; i < int(m_cv_xyz_filenames.size()); ++i) {
    units[i] = m_start_texture_unit + i * 2 + 1;
  }
  return units;
}

void
CalibVolume::bindToTextureUnits(unsigned start_texture_unit) {
  for(unsigned layer = 0; layer < m_volumes_xyz.size(); ++layer){
    m_volumes_xyz[layer]->bindActive(GL_TEXTURE0 + start_texture_unit + layer * 2);
    m_volumes_uv[layer]->bindActive(GL_TEXTURE0 + start_texture_unit + layer * 2 + 1);
  }
  glActiveTexture(GL_TEXTURE0);
  m_start_texture_unit = start_texture_unit;    
}

unsigned CalibVolume::getStartTextureUnit() const {
  return m_start_texture_unit;
}

}