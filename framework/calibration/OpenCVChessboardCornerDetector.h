#ifndef KINECT_OPENCVCHESSBOARDCORNERDETECTOR_H
#define KINECT_OPENCVCHESSBOARDCORNERDETECTOR_H

#include <DataTypes.h>

#include <opencv/cv.h>
#include <vector>

namespace kinect{


  class OpenCVChessboardCornerDetector{

  public:
    OpenCVChessboardCornerDetector(unsigned width, unsigned height, int depth /*bits per channel*/, int channels, unsigned board_w, unsigned board_h);
    ~OpenCVChessboardCornerDetector();

    bool process(const void*, unsigned bytes, bool showimages = true);


  private:
    unsigned m_channels;
    unsigned m_width;
    unsigned m_height;
    unsigned m_depth;
    unsigned m_bytes;
    IplImage* m_image;
    IplImage* m_gray_image;
    IplImage* m_gray_image_f;
    IplImage* m_tmp_image;
    CvSize m_board_sz;
    CvPoint2D32f* m_corners;
    unsigned m_board_w;
    unsigned m_board_h;
    unsigned m_num_corners;


  public:
    std::vector<uv> corners;
  };


} // namespace kinect


#endif // #ifndef KINECT_OPENCVCHESSBOARDCORNERDETECTOR_H

