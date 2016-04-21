#ifndef C3DPOSETRACKER_H
#define C3DPOSETRACKER_H

#include <KinectCalibrationFile.h>

#include <Matrix.h>
#include <vector>

namespace kinect{
  class CornerTracker;
  class CalibVolume;
  class C3DPoseTracker{

  public:
    C3DPoseTracker(int num_images, unsigned width, unsigned height, int bitsperchannels, int cb_width, int cb_height);
    ~C3DPoseTracker();

    gloost::Matrix getPoseMatrixART();
    gloost::Matrix getPoseMatrixKinect(std::vector<unsigned char* > irdata, std::vector<float* > depthdata, CalibVolume* cv, std::vector<KinectCalibrationFile*>& calibs);
    gloost::Matrix getPoseMatrixKinect2(std::vector<unsigned char* > irdata, std::vector<float* > depthdata, CalibVolume* cv, std::vector<KinectCalibrationFile*>& calibs);

  private:
    kinect::CornerTracker* m_ct;

  };

}

#endif // #ifndef C3DPOSETRACKER_H
