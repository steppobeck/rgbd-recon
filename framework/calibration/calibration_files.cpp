#include "calibration_files.hpp"
#include <KinectCalibrationFile.h>

#include <fstream>

namespace kinect{
  CalibrationFiles::CalibrationFiles(const char* config)
    : m_width(0),
      m_widthc(0),
      m_height(0),
      m_heightc(0),
      m_numLayers(0),
      m_calibs(),
      m_filenames{}
  {
    std::ifstream in(config);
    std::string token;
    while(in >> token){
      if(token == "kinect"){
        in >> token;
        m_filenames.push_back(token);
        m_calibs.push_back(KinectCalibrationFile{token});
        m_calibs.back().parse();
      }
    }
    in.close();

    m_numLayers = m_calibs.size();
    m_width   = m_calibs[0].getWidth();
    m_widthc  = m_calibs[0].getWidthC();
    m_height  = m_calibs[0].getHeight();
    m_heightc = m_calibs[0].getHeightC();
  }

  unsigned
  CalibrationFiles::getWidth() const {
    return m_width;
  }
  unsigned
  CalibrationFiles::getWidthC() const {
    return m_widthc;
  }
  unsigned
  CalibrationFiles::getHeight() const {
    return m_height;
  }
  unsigned
  CalibrationFiles::getHeightC() const {
    return m_heightc;
  }

  unsigned
  CalibrationFiles::getNumLayers() const {
    return m_numLayers;
  }

  std::vector<KinectCalibrationFile> const&
  CalibrationFiles::getCalibs() const {
    return m_calibs;
  }
  std::vector<std::string> const&
  CalibrationFiles::getFileNames() const {
    return m_filenames;
  }
}