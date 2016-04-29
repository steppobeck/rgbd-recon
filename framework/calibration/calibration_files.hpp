#ifndef KINECT_CALIB_FILES_HPP
#define KINECT_CALIB_FILES_HPP

#include <vector>
#include <string>

namespace kinect{

  class KinectCalibrationFile;

  class CalibrationFiles{

  public:
    CalibrationFiles(const char* config);

    unsigned getWidth() const;
    unsigned getWidthC() const;

    unsigned getHeight() const;
    unsigned getHeightC() const;

    unsigned getNumLayers() const;

    std::vector<KinectCalibrationFile> const& getCalibs() const;
    std::vector<std::string> const& getFileNames() const;
  private:
    unsigned m_width;
    unsigned m_widthc;
    unsigned m_height;
    unsigned m_heightc;
    unsigned m_numLayers;

    std::vector<KinectCalibrationFile> m_calibs;
    std::vector<std::string> m_filenames;
  };
}

#endif // #ifndef KINECT_CALIB_FILES_HPP

