#ifndef CORNERTRACKER_H
#define CORNERTRACKER_H

#include "OpenCVChessboardCornerDetector.h"
#include <DataTypes.h>

#include <vector>


namespace kinect{

  class CornerTracker{

  private:
    int m_numer_of_images;
    unsigned m_image_width;
    unsigned m_image_height;
    int m_numchannels;
    int m_channel_depth;
    int m_chessboard_width;
    int m_chessboard_height;

    std::vector< OpenCVChessboardCornerDetector* > m_cornerDetectors;
    std::vector< std::vector<uv>* > m_results;


  public:
    CornerTracker(int number_of_images, unsigned width, unsigned height, int numchannels, int channel_depth, int chessboard_width, int chessboard_height);
    ~CornerTracker();

    std::vector< std::vector<uv>* > process(std::vector<unsigned char* >image_data);
    void processByThread(unsigned char* image_data, unsigned id);

  };


}



#endif // #ifndef  CORNERTRACKER_H
