#ifndef KINECT_CALIB_FILES_HPP
#define KINECT_CALIB_FILES_HPP

#include <vector>
#include <string>

namespace kinect{

  class KinectCalibrationFile;

  class CalibrationFiles{

  public:
    CalibrationFiles(std::vector<std::string> const& calib_filenames);

    void reload();

    unsigned getWidth() const;
    unsigned getWidthC() const;

    unsigned getHeight() const;
    unsigned getHeightC() const;
    unsigned num() const;
    float minLength() const;

    unsigned isCompressedRGB() const;
    bool isCompressedDepth() const;

    std::vector<KinectCalibrationFile> const& getCalibs() const;
    std::vector<std::string> const& getFileNames() const;
  private:
    unsigned m_width;
    unsigned m_widthc;
    unsigned m_height;
    unsigned m_heightc;
    unsigned m_numLayers;
    float m_min_length;

    unsigned m_compressed_rgb;
    bool m_compressed_d;

    std::vector<KinectCalibrationFile> m_calibs;
    std::vector<std::string> m_filenames;
  };
}

#endif // #ifndef KINECT_CALIB_FILES_HPP

