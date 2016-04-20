#ifndef KINECT_EVALUATIONVOLUMES_H
#define KINECT_EVALUATIONVOLUMES_H


#include <vector>

namespace kinect{

  class KinectCalibrationFile;
  class CalibVolume;

  class EvaluationVolumes{

  public:
    EvaluationVolumes(const std::vector<KinectCalibrationFile*>& calibs, CalibVolume* cv);
    ~EvaluationVolumes();

    std::vector<unsigned> error3D_ids;
    std::vector<unsigned> error2D_ids;
    std::vector<unsigned> nnistats_ids;

  };

}

#endif // #ifndef KINECT_EVALUATIONVOLUMES_H
