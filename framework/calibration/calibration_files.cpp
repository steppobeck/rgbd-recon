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
      m_min_length(0),
      m_compressed_rgb{false},
      m_compressed_d{false},
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
      }
    }
    in.close();

    reload();

    m_numLayers = m_calibs.size();
    m_width   = m_calibs[0].getWidth();
    m_widthc  = m_calibs[0].getWidthC();
    m_height  = m_calibs[0].getHeight();
    m_heightc = m_calibs[0].getHeightC();
    m_min_length = m_calibs[0].min_length;

    m_compressed_rgb = m_calibs[0].isCompressedRGB();
    m_compressed_d = m_calibs[0].isCompressedDepth();
  }

  void CalibrationFiles::reload() {
    for (auto& calib : m_calibs) {
      calib.parse();
    }
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
  CalibrationFiles::num() const {
    return m_numLayers;
  }

  float
  CalibrationFiles::minLength() const {
    return m_min_length;
  }

  bool CalibrationFiles::isCompressedRGB() const {
    return m_compressed_rgb;
  }
  bool CalibrationFiles::isCompressedDepth() const {
    return m_compressed_d;
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