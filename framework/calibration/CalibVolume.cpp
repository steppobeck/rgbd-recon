#include "CalibVolume.h"
#include <KinectCalibrationFile.h>
#include <timevalue.h>

// #include <fstream>

namespace kinect{

  /*static*/ std::string CalibVolume::serverendpoint("tcp://141.54.147.22:7001");

  CalibVolume::CalibVolume(std::vector<std::string> const& calib_volume_files):
    m_cv_xyz_filenames(),
    m_cv_uv_filenames(),
    m_cv_xyz_ids(),
    m_cv_uv_ids(),
    m_cv_widths(),
    m_cv_heights(),
    m_cv_depths(),
    m_cv_min_ds(),
    m_cv_max_ds(),
    m_cv_xyzs(),
    m_cv_uvs(),
    m_start_texture_unit(0)
  {
    for(auto const& calib_file : calib_volume_files){
    	std::string basefile = calib_file;
    	basefile.replace( basefile.end() - 3, basefile.end(), "");
    	m_cv_xyz_filenames.push_back(basefile + "cv_xyz");
    	m_cv_uv_filenames.push_back(basefile + "cv_uv");
    }

    reload();
  }

  /*virtual*/
  CalibVolume::~CalibVolume(){
    for(unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i){
      delete [] m_cv_xyzs[i];
      delete [] m_cv_uvs[i];
      glDeleteTextures(1, &(m_cv_xyz_ids[i]));
      glDeleteTextures(1, &(m_cv_uv_ids[i]));
    }
  }

  bool
  CalibVolume::reload(){
    
    for(unsigned i = 0; i < m_cv_xyzs.size(); ++i){
      if(0 != m_cv_xyzs[i]){
      	delete [] m_cv_xyzs[i];
      	delete [] m_cv_uvs[i];
      	glDeleteTextures(1, &(m_cv_xyz_ids[i]));
      	glDeleteTextures(1, &(m_cv_uv_ids[i]));
      }
    }
    m_cv_xyz_ids.clear();
    m_cv_uv_ids.clear();
    m_cv_widths.clear();
    m_cv_heights.clear();
    m_cv_depths.clear();
    m_cv_min_ds.clear();
    m_cv_max_ds.clear();
    m_cv_xyzs.clear();
    m_cv_uvs.clear();

    for(unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i){
      
      unsigned cv_width;
      unsigned cv_height;
      unsigned cv_depth;
      float    cv_min_d;
      float    cv_max_d;
      unsigned cv_xyz_id;
      unsigned cv_uv_id;

      std::cerr << "loading " << m_cv_xyz_filenames[i] << std::endl;
      FILE* f_xyz = fopen( m_cv_xyz_filenames[i].c_str(), "rb");
      fread(&cv_width, sizeof(unsigned), 1, f_xyz);
      fread(&cv_height, sizeof(unsigned), 1, f_xyz);
      fread(&cv_depth, sizeof(unsigned), 1, f_xyz);
      fread(&cv_min_d, sizeof(float), 1, f_xyz);
      fread(&cv_max_d, sizeof(float), 1, f_xyz);
      xyz* cv_xyz = new xyz[cv_width * cv_height * cv_depth];
      fread(cv_xyz, sizeof(xyz), cv_width * cv_height * cv_depth, f_xyz);
      fclose(f_xyz);
    
      std::cerr << "loading " << m_cv_uv_filenames[i] << std::endl;
      FILE* f = fopen( m_cv_uv_filenames[i].c_str(), "rb");
      fread(&cv_width, sizeof(unsigned), 1, f);
      fread(&cv_height, sizeof(unsigned), 1, f);
      fread(&cv_depth, sizeof(unsigned), 1, f);
      fread(&cv_min_d, sizeof(float), 1, f);
      fread(&cv_max_d, sizeof(float), 1, f);
      uv*  cv_uv  = new uv [cv_width * cv_height * cv_depth];
      fread(cv_uv, sizeof(uv), cv_width * cv_height * cv_depth, f);
      fclose(f);
      
      std::cerr << "generating textures" << std::endl;
      glGenTextures(1, &cv_xyz_id);
      
      glBindTexture(GL_TEXTURE_3D, cv_xyz_id);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
      glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB32F, cv_width, cv_height, cv_depth, 0, GL_RGB, GL_FLOAT, (byte*) cv_xyz);


      glGenTextures(1, &cv_uv_id);
      glBindTexture(GL_TEXTURE_3D, cv_uv_id);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
      glTexImage3D(GL_TEXTURE_3D, 0, GL_RG32F, cv_width, cv_height, cv_depth, 0, GL_RG, GL_FLOAT, (byte*) cv_uv);

      std::cerr << "done generating textures" << std::endl;


      m_cv_xyz_ids.push_back(cv_xyz_id);
      m_cv_uv_ids.push_back(cv_uv_id);
      m_cv_widths.push_back(cv_width);
      m_cv_heights.push_back(cv_height);
      m_cv_depths.push_back(cv_depth);
      m_cv_min_ds.push_back(cv_min_d);
      m_cv_max_ds.push_back(cv_max_d);
      m_cv_xyzs.push_back(cv_xyz);
      m_cv_uvs.push_back(cv_uv);
    }

    std::cerr << "reload done" << std::endl;

    return true;
  }

  void
  CalibVolume::bindToTextureUnits(unsigned start_texture_unit) {
    for(unsigned layer = 0; layer < m_cv_xyzs.size(); ++layer){
      glActiveTexture(GL_TEXTURE0 + start_texture_unit + layer * 2);
      glBindTexture(GL_TEXTURE_3D, m_cv_xyz_ids[layer]);
      glActiveTexture(GL_TEXTURE0 + start_texture_unit + layer * 2 + 1);
      glBindTexture(GL_TEXTURE_3D, m_cv_uv_ids[layer]);
    }
    glActiveTexture(GL_TEXTURE0);
    m_start_texture_unit = start_texture_unit;    
  }

  unsigned CalibVolume::getStartTextureUnit() const {
    return m_start_texture_unit;
  }
}