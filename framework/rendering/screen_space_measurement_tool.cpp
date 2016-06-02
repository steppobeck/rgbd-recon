#include <screen_space_measurement_tool.hpp>

#include <gl_util.h>
#include <GL/glut.h>
#include <gloostMath.h>
#include <gloostHelper.h>

static gloost::Point3 multH(gloost::Matrix const& m,
                            gloost::Point3 const& p )
{
  gloost::Point3 tmp(m[0]*p[0] + m[4]*p[1] + m[8] *p[2] + m[12] *p[3],
         m[1]*p[0] + m[5]*p[1] + m[9] *p[2] + m[13] *p[3],
         m[2]*p[0] + m[6]*p[1] + m[10]*p[2] + m[14] *p[3]);
  tmp[3] =           m[3]*p[0] + m[7]*p[1] + m[11]*p[2] + m[15] *p[3];
  return tmp;
}

ScreenSpaceMeasureTool::ScreenSpaceMeasureTool(gloost::Camera* cam, unsigned w, unsigned h)
  : m_cam(cam),
    m_w(w),
    m_h(h),
    m_pp(),
    m_mp(),
    m_modelview()
{}

void ScreenSpaceMeasureTool::resize(unsigned w, unsigned h){
  m_w = w;
  m_h = h;
}

void ScreenSpaceMeasureTool::mouse(int button, int state, int mouse_h, int mouse_v){
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    m_pp.clear();
    pickpos pp(mouse_h, mouse_v);
    m_pp.push_back(pp);


    float d = pickDepth(m_pp[0]);

    // calculate img_to_eye for this view
    gloost::Matrix viewport_translate;
    viewport_translate.setIdentity();
    viewport_translate.setTranslate(1.0,1.0,1.0);
    gloost::Matrix viewport_scale;
    viewport_scale.setIdentity();
    viewport_scale.setScale(m_w * 0.5, m_h * 0.5, 0.5f);
    gloost::Matrix image_to_eye =  viewport_scale * viewport_translate * m_cam->getProjectionMatrix() * m_modelview;
    image_to_eye.invert();
    gloost::Point3 pos_img(m_pp[0].x + 0.5, m_pp[0].y + 0.5, d);
    gloost::Point3 pos_es = multH(image_to_eye, pos_img);
    std::cerr << pos_es[3] << std::endl;
    pos_es[0] /= pos_es[3];
    pos_es[1] /= pos_es[3];
    pos_es[2] /= pos_es[3];
    pos_es[3] /= pos_es[3];
#if 0      
    std::cerr << m_cam->getProjectionMatrix() << std::endl;
    std::cerr << m_pp[0].x << " " << m_pp[0].y << ": " << d << " 3D pos: " << pos_es << std::endl;
#endif

    m_mp.push_back(pos_es);

  }
  else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
    m_mp.pop_back();
  }
  else if(button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN){
    m_mp.clear();
  }
}

float ScreenSpaceMeasureTool::measure(){
  const size_t mp_count = m_mp.size();
  if(mp_count > 1){
    return (m_mp[mp_count - 1] - m_mp[mp_count - 2]).length();
  }

  return -1.0;
}

std::vector<gloost::Point3>& ScreenSpaceMeasureTool::getMeasurePoints(){
  return m_mp;
}

void ScreenSpaceMeasureTool::setModelView(gloost::Matrix& mv){
  m_modelview = mv;
}

float ScreenSpaceMeasureTool::pickDepth(const pickpos& pp){
  float d;
  glReadPixels( pp.x, pp.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &d);
  return d;
}