#include "PoseTracker.h"
#include <Vector3.h>


namespace gloost{


  PoseTracker::PoseTracker()
    : _poses(),
      _lastNumStables(0)
  {
    gloost::Matrix m;
    m.setIdentity();
    _poses.push_back(m);
    _poses.push_back(m);
  }



  PoseTracker::~PoseTracker()
  {}
  

  void
  PoseTracker::update(const gloost::Matrix& m){
    _poses[0] = _poses[1];
    _poses[1] = m;

  }


  float
  PoseTracker::currSpeed(const gloost::Matrix& m){
    update(m);
    gloost::Vector3 a(_poses[0].getTranslate());
    gloost::Vector3 b(_poses[1].getTranslate());
    float speed = (b-a).length();
    return speed;
  }


  float
  PoseTracker::getSpeed(float minspeed){
    gloost::Vector3 a(_poses[0].getTranslate());
    gloost::Vector3 b(_poses[1].getTranslate());
    float speed = (b-a).length();
    return speed < minspeed ? 0.0 : speed;
  }

  bool
  PoseTracker::isStable(const gloost::Matrix& m, float minspeed, unsigned numstables){
    update(m);
    if(getSpeed(minspeed) < minspeed){
      ++_lastNumStables;
    }
    else{
      _lastNumStables = 0;
    }
    return (_lastNumStables > numstables);
  }
}
