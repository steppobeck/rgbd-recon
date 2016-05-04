#include "EvaluationVolumes.h"

#include <KinectCalibrationFile.h>
#include <CalibVolume.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace kinect{

  EvaluationVolumes::EvaluationVolumes(const std::vector<KinectCalibrationFile*>& calibs, CalibVolume* cv)
    : error3D_ids(),
      error2D_ids(),
      nnistats_ids()
  {
    for(unsigned i = 0; i < calibs.size(); ++i){
      std::string basefile = calibs[i]->_filePath;
      basefile.replace( basefile.end() - 3, basefile.end(), "");
      std::string error3D_filename = basefile + "cv_error3D_nni";
      std::string error2D_filename = basefile + "cv_error2D_nni";
      std::string nnistats_filename = basefile + "cv_nnistats";
      
      std::uint8_t* error3D       = new std::uint8_t [cv->m_cv_widths[i] * cv->m_cv_heights[i] * cv->m_cv_depths[i]];
      std::uint8_t* error2D       = new std::uint8_t [cv->m_cv_widths[i] * cv->m_cv_heights[i] * cv->m_cv_depths[i]];
      std::uint8_t* nnistats = new std::uint8_t [cv->m_cv_widths[i] * cv->m_cv_heights[i] * cv->m_cv_depths[i]];

      FILE* error3D_f = fopen( error3D_filename.c_str(), "rb");
      fread(error3D, sizeof(std::uint8_t), cv->m_cv_widths[i] * cv->m_cv_heights[i] * cv->m_cv_depths[i], error3D_f);
      fclose(error3D_f);

      FILE* error2D_f = fopen( error2D_filename.c_str(), "rb");
      fread(error2D, sizeof(std::uint8_t), cv->m_cv_widths[i] * cv->m_cv_heights[i] * cv->m_cv_depths[i], error2D_f);
      fclose(error2D_f);

      FILE* nnistats_f = fopen( nnistats_filename.c_str(), "rb");
      fread(nnistats, sizeof(std::uint8_t), cv->m_cv_widths[i] * cv->m_cv_heights[i] * cv->m_cv_depths[i], nnistats_f);
      fclose(nnistats_f);

      unsigned tex_id;
      glGenTextures(1, &tex_id);
      glBindTexture(GL_TEXTURE_3D, tex_id);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
      glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, cv->m_cv_widths[i], cv->m_cv_heights[i], cv->m_cv_depths[i], 0, GL_RED, GL_UNSIGNED_BYTE, error3D);
      error3D_ids.push_back(tex_id);


      glGenTextures(1, &tex_id);
      glBindTexture(GL_TEXTURE_3D, tex_id);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
      glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, cv->m_cv_widths[i], cv->m_cv_heights[i], cv->m_cv_depths[i], 0, GL_RED, GL_UNSIGNED_BYTE, error2D);
      error2D_ids.push_back(tex_id);


      glGenTextures(1, &tex_id);
      glBindTexture(GL_TEXTURE_3D, tex_id);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
      glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, cv->m_cv_widths[i], cv->m_cv_heights[i], cv->m_cv_depths[i], 0, GL_RED, GL_UNSIGNED_BYTE, nnistats);
      nnistats_ids.push_back(tex_id);




      delete [] error3D;
      delete [] error2D;
      delete [] nnistats;

    }


  }


  EvaluationVolumes::~EvaluationVolumes()
  {}

  


}
