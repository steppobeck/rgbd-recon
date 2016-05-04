#ifndef SCREEN_SPACE_MEASUREMENT_TOOL_HPP
#define SCREEN_SPACE_MEASUREMENT_TOOL_HPP

#include <vector>

#include <Camera.h>

struct pickpos{
public:
  pickpos(unsigned a, unsigned b)
    : x(a),
      y(b)
  {}
  unsigned x;
  unsigned y;

};

class ScreenSpaceMeasureTool{

public:

  ScreenSpaceMeasureTool(gloost::Camera* cam, unsigned w, unsigned h);

  void resize(unsigned w, unsigned h);

  void mouse(int button, int state, int mouse_h, int mouse_v);

  float measure();

  std::vector<gloost::Point3>& getMeasurePoints();
  void setModelView(gloost::Matrix& mv);
private:

  float pickDepth(const pickpos& pp);

  gloost::Camera*      m_cam;
  unsigned             m_w;
  unsigned             m_h;

  std::vector<pickpos> m_pp;
  std::vector<gloost::Point3> m_mp;
  gloost::Matrix m_modelview;
};

#endif