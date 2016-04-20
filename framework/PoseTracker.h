#ifndef GLOOST_POSE_TRACKER_H
#define GLOOST_POSE_TRACKER_H


#include <Matrix.h>
#include <vector>


namespace gloost{


  class PoseTracker{

  public:
    PoseTracker();
    ~PoseTracker();

    void update(const gloost::Matrix& m);
    float getSpeed(float minspeed = 0.005f);
    bool isStable(const gloost::Matrix& m, float minspeed, unsigned numstables);
    float currSpeed(const gloost::Matrix& m);

  private:


    std::vector<gloost::Matrix> _poses;
    unsigned _lastNumStables;
  };

}



#endif // #ifndef  GLOOST_POSE_TRACKER_H
