#include "3DPoseTracker.h"

#include <CornerTracker.h>
#include <DataTypes.h>
#include <CalibVolume.h>
#include <Vector3.h>
#if 0

  kinect::CornerTracker ct(2         /*number of image*/
                          ,img->width ,img->height  /*width, height*/
                          ,1 ,8      /*numchannels, numbitsperchannel*/
                          ,7, 5);    /*chessboard_width, chessboard_height*/ 

  std::vector<char* > imgdata;
  imgdata.push_back(grey_img->imageData);
  imgdata.push_back(grey_img2->imageData);
  
  std::vector< std::vector<kinect::uv>* > found_results = ct.process(imgdata);

#endif

namespace kinect{


  C3DPoseTracker::C3DPoseTracker(int num_images, unsigned width, unsigned height, int bitsperchannels, int cb_width, int cb_height):
    m_ct(0)
  {
    m_ct = new CornerTracker(num_images, width, height, 1, bitsperchannels, cb_width, cb_height);
  }

  C3DPoseTracker::~C3DPoseTracker()
  {}

  gloost::Matrix
  C3DPoseTracker::getPoseMatrixKinect(std::vector<unsigned char* > irdata, std::vector<float* > depthdata, CalibVolume* cv, std::vector<KinectCalibrationFile*>& calibs){
    static gloost::Matrix pose_kinect;
    //pose_kinect.setIdentity();

    std::vector< std::vector<kinect::uv>* > found_results = m_ct->process(irdata);

#if 0
    for(unsigned i = 0; i<found_results.size(); ++i)
      {
	std::cout << "Image " << i << "  Corners: " << std::endl;
	for(unsigned j = 0; j<found_results[i]->size(); ++j)
	  {
	    std::cout << j << " :" << (*found_results[i])[j].u << " " << (*found_results[i])[j].v << std::endl;
	  }
      }
#endif

    std::vector<gloost::Point3> oxs;
    std::vector<gloost::Point3> exs;
    std::vector<gloost::Point3> eys;
    for(unsigned i = 0; i < found_results.size(); ++i){
      for(unsigned j = 0; j < found_results[i]->size(); ++j){
	const float u = (*found_results[i])[j].u;
	const float v = (*found_results[i])[j].v;
	const float depth = cv->getBilinear(depthdata[i], calibs[i]->getWidth(),  calibs[i]->getHeight(), u, v);
	//std::cout << "----------------------------: " << z << std::endl;


	const unsigned cv_width = cv->m_cv_widths[i];
	const unsigned cv_height = cv->m_cv_heights[i];
	const unsigned cv_depth = cv->m_cv_depths[i];
	const float x = cv_width *  ( u)/ calibs[i]->getWidth();
	const float y = cv_height *  ( v)/ calibs[i]->getHeight();
	const float z = cv_depth * (  depth - cv->m_cv_min_ds[i])/(cv->m_cv_max_ds[i] - cv->m_cv_min_ds[i]);
	xyz pos = cv->getTrilinear(cv->m_cv_xyzs[i], cv_width, cv_height, cv_depth, x , y , z );

	gloost::Point3 sp(pos.x, pos.y, pos.z);
	if(0 == j){
	  oxs.push_back(sp);
	}
	if(1 == j){
	  exs.push_back(sp);
	}
	if(7 == j){
	  eys.push_back(sp);
	}
	
      }
    }

#if 0

  _worldTranslation[0] = -c._o[0];
  _worldTranslation[1] = -c._o[1];
  _worldTranslation[2] = -c._o[2];

  _worldRotation.setIdentity();

  _worldRotation[0] = c._ex[0];
  _worldRotation[1] = c._ex[1];
  _worldRotation[2] = c._ex[2];

  _worldRotation[4] = c._ey[0];
  _worldRotation[5] = c._ey[1];
  _worldRotation[6] = c._ey[2];

  _worldRotation[8] = c._ez[0];
  _worldRotation[9] = c._ez[1];
  _worldRotation[10]= c._ez[2];
  
  //_worldRotation.transpose();
  _worldRotation.invert();

#endif

  if(oxs.size() == exs.size() && exs.size() == eys.size() && oxs.size() > 0){
    gloost::Point3 ox(0.0,0.0,0.0);
    gloost::Point3 ex(0.0,0.0,0.0);
    gloost::Point3 ey(0.0,0.0,0.0);
    for(unsigned i = 0; i < oxs.size(); ++i){

#if 1
      ox[0] += oxs[i][0];
      ex[0] += exs[i][0];
      ey[0] += eys[i][0];

      ox[1] += oxs[i][1];
      ex[1] += exs[i][1];
      ey[1] += eys[i][1];

      ox[2] += oxs[i][2];
      ex[2] += exs[i][2];
      ey[2] += eys[i][2];
#endif

#if 0
      ox[0] = oxs[i][0];
      ex[0] = exs[i][0];
      ey[0] = eys[i][0];

      ox[1] = oxs[i][1];
      ex[1] = exs[i][1];
      ey[1] = eys[i][1];

      ox[2] = oxs[i][2];
      ex[2] = exs[i][2];
      ey[2] = eys[i][2];
#endif

    }
#if 1
    ox[0] /= oxs.size();
    ex[0] /= oxs.size();
    ey[0] /= oxs.size();

    ox[1] /= oxs.size();
    ex[1] /= oxs.size();
    ey[1] /= oxs.size();

    ox[2] /= oxs.size();
    ex[2] /= oxs.size();
    ey[2] /= oxs.size();


    ox[3] = 1.0;
    ex[3] = 1.0;
    ey[3] = 1.0;

#endif

    gloost::Vector3 vex(ey - ox);
    gloost::Vector3 vey(ex - ox);
    vex.normalize();
    vey.normalize();
    gloost::Vector3 vez = cross(vex, vey);



    gloost::Matrix rot;
    rot.setIdentity();

    rot[0] = vex[0];
    rot[1] = vex[1];
    rot[2] = vex[2];

    rot[4] = vey[0];
    rot[5] = vey[1];
    rot[6] = vey[2];

    rot[8] = vez[0];
    rot[9] = vez[1];
    rot[10]= vez[2];

    gloost::Matrix trans;
    trans.setIdentity();
    trans.setTranslate(ox[0],ox[1],ox[2]);


    pose_kinect = trans * rot;

  }

  //pose_kinect.setTranslate(sp[0],sp[1],sp[2]);


    return pose_kinect;
  }



gloost::Matrix
  C3DPoseTracker::getPoseMatrixKinect2(std::vector<unsigned char* > irdata, std::vector<float* > depthdata, CalibVolume* cv, std::vector<KinectCalibrationFile*>& calibs){

    static gloost::Matrix pose_kinect;



    std::vector< std::vector<kinect::uv>* > found_results = m_ct->process(irdata);

    std::vector<gloost::Point3> oxs;
    // 1 ... 4 -------------------------------------
    std::vector<gloost::Point3> exs1;
    std::vector<gloost::Point3> eys1;
    std::vector<gloost::Point3> exs2;
    std::vector<gloost::Point3> eys2;
    std::vector<gloost::Point3> exs3;
    std::vector<gloost::Point3> eys3;
    std::vector<gloost::Point3> exs4;
    std::vector<gloost::Point3> eys4;


    for(unsigned i = 0; i < found_results.size(); ++i){
      for(unsigned j = 0; j < found_results[i]->size() && found_results[i]->size() == 35; ++j){
	const float u = (*found_results[i])[j].u;
	const float v = (*found_results[i])[j].v;
	const float depth = cv->getBilinear(depthdata[i], calibs[i]->getWidth(),  calibs[i]->getHeight(), u, v);
	const unsigned cv_width = cv->m_cv_widths[i];
	const unsigned cv_height = cv->m_cv_heights[i];
	const unsigned cv_depth = cv->m_cv_depths[i];
	const float x = cv_width *  ( u)/ calibs[i]->getWidth();
	const float y = cv_height *  ( v)/ calibs[i]->getHeight();
	const float z = cv_depth * (  depth - cv->m_cv_min_ds[i])/(cv->m_cv_max_ds[i] - cv->m_cv_min_ds[i]);
	const xyz pos = cv->getTrilinear(cv->m_cv_xyzs[i], cv_width, cv_height, cv_depth, x , y , z );
	const gloost::Point3 sp(pos.x, pos.y, pos.z);


      // 1 ... 4 -------------------------------------
#if 0

	0* 7*  14* 21* 28*
	1* 8   15  22  29
	2* 9   16  23  30
	3* 10  17  24  31
	4* 11  18  25  32
	5  12  19  26  33
	6  13  20  27  34

#endif

	if(0 == j){
	  oxs.push_back(sp);
	}

	if(1 == j){
	  exs1.push_back(sp);
	}
	if(2 == j){
	  exs2.push_back(sp);
	}
	if(3 == j){
	  exs3.push_back(sp);
	}
	if(4 == j){
	  exs4.push_back(sp);
	}


	if(7 == j){
	  eys1.push_back(sp);
	}
	if(14 == j){
	  eys2.push_back(sp);
	}
	if(21 == j){
	  eys3.push_back(sp);
	}
	if(28 == j){
	  eys4.push_back(sp);
	}


	
      }
    }


  const unsigned ns(oxs.size());
  const float invns(1.0f/ns);
  if(ns > 0){
    
    gloost::Point3 ox(0.0,0.0,0.0);
    // 1 ... 4 -------------------------------------
    gloost::Point3 ex1(0.0,0.0,0.0);
    gloost::Point3 ey1(0.0,0.0,0.0);
    gloost::Point3 ex2(0.0,0.0,0.0);
    gloost::Point3 ey2(0.0,0.0,0.0);
    gloost::Point3 ex3(0.0,0.0,0.0);
    gloost::Point3 ey3(0.0,0.0,0.0);
    gloost::Point3 ex4(0.0,0.0,0.0);
    gloost::Point3 ey4(0.0,0.0,0.0);
    
    for(unsigned i = 0; i < ns; ++i){
      ox += oxs[i];
      // 1 ... 5 -------------------------------------
      ex1 += exs1[i];
      ey1 += eys1[i];
      ex2 += exs2[i];
      ey2 += eys2[i];
      ex3 += exs3[i];
      ey3 += eys3[i];
      ex4 += exs4[i];
      ey4 += eys4[i];
    }

    ox = ox * invns; // has to produce w == 1

    // 1 ... 4 -------------------------------------

    // 1
    ex1 = ex1 * invns; // has to produce w == 1
    ey1 = ey1 * invns; // has to produce w == 1
    // 2
    ex2 = ex2 * invns; // has to produce w == 1
    ey2 = ey2 * invns; // has to produce w == 1
    // 3
    ex3 = ex3 * invns; // has to produce w == 1
    ey3 = ey3 * invns; // has to produce w == 1
    // 4
    ex4 = ex4 * invns; // has to produce w == 1
    ey4 = ey4 * invns; // has to produce w == 1

  
    // 1 ... 4 -------------------------------------
    // 1
    gloost::Vector3 vex1(ey1 - ox);
    gloost::Vector3 vey1(ex1 - ox);
    vex1.normalize();
    vey1.normalize();
    gloost::Vector3 vez1 = cross(vex1, vey1);

    // 2
    gloost::Vector3 vex2(ey2 - ox);
    gloost::Vector3 vey2(ex2 - ox);
    vex2.normalize();
    vey2.normalize();
    gloost::Vector3 vez2 = cross(vex2, vey2);

    // 3
    gloost::Vector3 vex3(ey3 - ox);
    gloost::Vector3 vey3(ex3 - ox);
    vex3.normalize();
    vey3.normalize();
    gloost::Vector3 vez3 = cross(vex3, vey3);

    // 4
    gloost::Vector3 vex4(ey4 - ox);
    gloost::Vector3 vey4(ex4 - ox);
    vex4.normalize();
    vey4.normalize();
    gloost::Vector3 vez4 = cross(vex4, vey4);



    // 1 ... 4 -------------------------------------
    gloost::Vector3 vex( (vex1 + vex2 + vex3 + vex4) * 1.0/4);
    gloost::Vector3 vey( (vey1 + vey2 + vey3 + vey4) * 1.0/4);
    gloost::Vector3 vez( (vez1 + vez2 + vez3 + vez4) * 1.0/4);


    gloost::Matrix rot;
    rot.setIdentity();

    rot[0] = vex[0];
    rot[1] = vex[1];
    rot[2] = vex[2];

    rot[4] = vey[0];
    rot[5] = vey[1];
    rot[6] = vey[2];

    rot[8] = vez[0];
    rot[9] = vez[1];
    rot[10]= vez[2];

    gloost::Matrix trans;
    trans.setIdentity();
    trans.setTranslate(ox[0],ox[1],ox[2]);


    pose_kinect = trans * rot;

  }
    return pose_kinect;
  }




}
