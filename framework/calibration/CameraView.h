#ifndef MVT_CAMERAVIEW_H
#define MVT_CAMERAVIEW_H


#include <Matrix.h>
#include <Point3.h>
#include <Vector3.h>

#include <frustumCulling.h>

#include <cmath>

#include <GL/glew.h>

namespace mvt{


  class CameraView{

  public:
    CameraView()
      : image_d_to_eye_d(),
      eye_d_to_image_d(),
      eye_d_to_world(),
      eye_d_to_eye_rgb(),
      eye_rgb_to_image_rgb(),
      eye_rgb_to_world(),
      m_near_lower_left(),
      m_near_lower_right(),
      m_near_upper_right(),
      m_near_upper_left(),
      m_far_lower_left(),
      m_far_lower_right(),
      m_far_upper_right(),
      m_far_upper_left(), 
      m_near_lower_left_color(),
      m_near_lower_right_color(),
      m_near_upper_right_color(),
      m_near_upper_left_color(),
      m_far_lower_left_color(),
      m_far_lower_right_color(),
      m_far_upper_right_color(),
      m_far_upper_left_color(),
      m_culler() {}

    virtual ~CameraView(){}


    virtual void updateMatrices(){}

    gloost::Matrix image_d_to_eye_d;
    gloost::Matrix eye_d_to_image_d;
    gloost::Matrix eye_d_to_world;
    gloost::Matrix eye_d_to_eye_rgb;
    gloost::Matrix eye_rgb_to_image_rgb;
    gloost::Matrix eye_rgb_to_world;


    gloost::Point3 calcViewCenterNear(){
      gloost::Vector3 half_leftright_near(0.5 * (m_near_lower_right - m_near_lower_left));
      gloost::Vector3 half_lowup_near(0.5 * (m_near_upper_right - m_near_lower_right));
      return gloost::Point3(m_near_lower_left + (half_leftright_near + half_lowup_near));
    }

    gloost::Point3 calcViewCenterFar(){
      gloost::Vector3 half_leftright_far(0.5 * (m_far_lower_right - m_far_lower_left));
      gloost::Vector3 half_lowup_far(0.5 * (m_far_upper_right - m_far_lower_right));
      return gloost::Point3(m_far_lower_left + (half_leftright_far + half_lowup_far));
    }

    gloost::Vector3 calcViewDir(){
      return (gloost::Vector3(calcViewCenterFar() - calcViewCenterNear())).normalized();
    }

    bool frustCull(){
      return false;
      // check all points
      m_culler.Calculate();
      
      /*
      m_near_lower_left(),
      m_near_lower_right(),
      m_near_upper_right(),
      m_near_upper_left(),
      m_far_lower_left(),
      m_far_lower_right(),
      m_far_upper_right(),
      m_far_upper_left(), 

      */
      
      gloost::Point3 nll(eye_d_to_world * m_near_lower_left);
      if(m_culler.IsPointWithin(nll[0], nll[1], nll[2]))
	 return false;

      gloost::Point3 nlr(eye_d_to_world * m_near_lower_right);
      if(m_culler.IsPointWithin(nlr[0], nlr[1], nlr[2]))
	 return false;

      gloost::Point3 nur(eye_d_to_world * m_near_upper_right);
      if(m_culler.IsPointWithin(nur[0], nur[1], nur[2]))
	 return false;

      gloost::Point3 nul(eye_d_to_world * m_near_upper_left);
      if(m_culler.IsPointWithin(nul[0], nul[1], nul[2]))
	 return false;




      gloost::Point3 fll(eye_d_to_world * m_far_lower_left);
      if(m_culler.IsPointWithin(fll[0], fll[1], fll[2]))
	 return false;

      gloost::Point3 flr(eye_d_to_world * m_far_lower_right);
      if(m_culler.IsPointWithin(flr[0], flr[1], flr[2]))
	 return false;

      gloost::Point3 fur(eye_d_to_world * m_far_upper_right);
      if(m_culler.IsPointWithin(fur[0], fur[1], fur[2]))
	 return false;

      gloost::Point3 ful(eye_d_to_world * m_far_upper_left);
      if(m_culler.IsPointWithin(ful[0], ful[1], ful[2]))
	 return false;

      return true;
    }


    bool backCull(gloost::Matrix& modelview){
      gloost::Vector3 kinect_view(calcViewDir());
      kinect_view = modelview * eye_d_to_world * kinect_view;
      gloost::Vector3 current_view(0.0,0.0,-1.0);
      const float rad = std::acos(kinect_view * current_view);
      const float angle = 180.0 * rad / M_PI;
      //std::cerr << angle << std::endl;
#if 1
      if(std::abs(angle) > 120.0)
	return true;
#endif
      return false;
    }

    void drawFrustum(){

      // front
      glBegin(GL_QUADS);
      glVertex3fv(m_near_lower_left.data());
      glVertex3fv(m_near_lower_right.data());
      glVertex3fv(m_near_upper_right.data());
      glVertex3fv(m_near_upper_left.data());
      glEnd();

      // back
      glBegin(GL_QUADS);
      glVertex3fv(m_far_lower_right.data());
      glVertex3fv(m_far_lower_left.data());
      glVertex3fv(m_far_upper_left.data());
      glVertex3fv(m_far_upper_right.data());
      glEnd();

      // left side
      glBegin(GL_QUADS);
      glVertex3fv(m_far_lower_left.data());
      glVertex3fv(m_near_lower_left.data());
      glVertex3fv(m_near_upper_left.data());
      glVertex3fv(m_far_upper_left.data());
      glEnd();

      // right side
      glBegin(GL_QUADS);
      glVertex3fv(m_near_lower_right.data());
      glVertex3fv(m_far_lower_right.data());
      glVertex3fv(m_far_upper_right.data());
      glVertex3fv(m_near_upper_right.data());
      glEnd();

      // top side
      glBegin(GL_QUADS);
      glVertex3fv(m_near_upper_right.data());
      glVertex3fv(m_far_upper_right.data());
      glVertex3fv(m_far_upper_left.data());
      glVertex3fv(m_near_upper_left.data());
      glEnd();

      // down side
      glBegin(GL_QUADS);
      glVertex3fv(m_near_lower_left.data());
      glVertex3fv(m_far_lower_left.data());
      glVertex3fv(m_far_lower_right.data());
      glVertex3fv(m_near_lower_right.data());
      glEnd();

#if 0
      // viewDir
      gloost::Point3 vcf(calcViewCenterFar());
      gloost::Point3 vcn(calcViewCenterNear());
      glBegin(GL_LINES);
      glLineWidth(2.5f);
      glVertex3fv(vcn.data());
      glVertex3fv(vcf.data());
      glEnd();
#endif
    }
    void drawFrustumColor(){

      // front
      glBegin(GL_QUADS);
      glVertex3fv(m_near_lower_left_color.data());
      glVertex3fv(m_near_lower_right_color.data());
      glVertex3fv(m_near_upper_right_color.data());
      glVertex3fv(m_near_upper_left_color.data());
      glEnd();

      // back
      glBegin(GL_QUADS);
      glVertex3fv(m_far_lower_right_color.data());
      glVertex3fv(m_far_lower_left_color.data());
      glVertex3fv(m_far_upper_left_color.data());
      glVertex3fv(m_far_upper_right_color.data());
      glEnd();

      // left side
      glBegin(GL_QUADS);
      glVertex3fv(m_far_lower_left_color.data());
      glVertex3fv(m_near_lower_left_color.data());
      glVertex3fv(m_near_upper_left_color.data());
      glVertex3fv(m_far_upper_left_color.data());
      glEnd();

      // right side
      glBegin(GL_QUADS);
      glVertex3fv(m_near_lower_right_color.data());
      glVertex3fv(m_far_lower_right_color.data());
      glVertex3fv(m_far_upper_right_color.data());
      glVertex3fv(m_near_upper_right_color.data());
      glEnd();

      // top side
      glBegin(GL_QUADS);
      glVertex3fv(m_near_upper_right_color.data());
      glVertex3fv(m_far_upper_right_color.data());
      glVertex3fv(m_far_upper_left_color.data());
      glVertex3fv(m_near_upper_left_color.data());
      glEnd();

      // down side
      glBegin(GL_QUADS);
      glVertex3fv(m_near_lower_left_color.data());
      glVertex3fv(m_far_lower_left_color.data());
      glVertex3fv(m_far_lower_right_color.data());
      glVertex3fv(m_near_lower_right_color.data());
      glEnd();


    }
  public:
    // depth
    gloost::Point3 m_near_lower_left;
    gloost::Point3 m_near_lower_right;
    gloost::Point3 m_near_upper_right;
    gloost::Point3 m_near_upper_left;
    gloost::Point3 m_far_lower_left;
    gloost::Point3 m_far_lower_right;
    gloost::Point3 m_far_upper_right;
    gloost::Point3 m_far_upper_left;
    // color
    gloost::Point3 m_near_lower_left_color;
    gloost::Point3 m_near_lower_right_color;
    gloost::Point3 m_near_upper_right_color;
    gloost::Point3 m_near_upper_left_color;
    gloost::Point3 m_far_lower_left_color;
    gloost::Point3 m_far_lower_right_color;
    gloost::Point3 m_far_upper_right_color;
    gloost::Point3 m_far_upper_left_color;

    frustumCulling m_culler;

  };

}// namespace mvt




#endif // #ifndef MVT_CAMERAVIEW_H
