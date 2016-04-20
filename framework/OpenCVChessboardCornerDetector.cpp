#include "OpenCVChessboardCornerDetector.h"

#include <iostream>


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>

#include <sstream> //for std::stringstream 
#include <string>  //for std::string


namespace kinect{

  OpenCVChessboardCornerDetector::OpenCVChessboardCornerDetector(unsigned width, unsigned height, int depth /*bits per channel*/, int channels, unsigned board_w, unsigned board_h)
    : m_channels(channels),
      m_width(width),
      m_height(height),
      m_depth(depth),
      m_bytes(width * height * (depth/8) * channels),
      m_image(),
      m_gray_image(),
      m_gray_image_f(),
      m_tmp_image(),
      m_board_sz(cvSize( board_w, board_h )),
      m_corners(new CvPoint2D32f[ board_w * board_h ]),
      m_board_w(board_w),
      m_board_h(board_h),
      m_num_corners(board_w * board_h)
  {


    const void * address = static_cast<const void*>(this);
    std::stringstream ss;
    ss << address;  
    std::string name = ss.str();


    if(1 != m_channels){
      m_image = cvCreateImage(cvSize(width,height), depth, channels);
      m_gray_image = cvCreateImage(cvSize(width,height), depth, 1);
      cvNamedWindow(name.c_str(), CV_WINDOW_AUTOSIZE);
    }
    else{
      m_tmp_image = cvCreateImage(cvSize(width,height), depth, 1);
      m_gray_image = cvCreateImage(cvSize(1*width,1*height), depth, 1);
      m_gray_image_f = cvCreateImage(cvSize(1*width,1*height), depth, 1);
      cvNamedWindow(name.c_str(), CV_WINDOW_AUTOSIZE);
      
    }
    

  }


  OpenCVChessboardCornerDetector::~OpenCVChessboardCornerDetector(){
    if(1 != m_channels)    
      cvReleaseImage(&m_image);
    cvReleaseImage(&m_gray_image);
    cvReleaseImage(&m_gray_image_f);
    delete [] m_corners;
  }

  bool
  OpenCVChessboardCornerDetector::process(const void* buffer, unsigned bytes, bool showimages){

    if(1 != m_channels){
      //std::cerr << this << " using color image " << std::endl;
      memcpy(m_image->imageData, buffer, bytes);
      if(3 == m_channels){
	cvCvtColor( m_image, m_gray_image, CV_RGB2GRAY );
      }
      else if(4 == m_channels){
	cvCvtColor( m_image, m_gray_image, CV_RGBA2GRAY );
      }
      else{
	std::cerr << "ERROR in OpenCVChessboardCornerDetector::process unsupported number of channels" << std::endl;
	return false;
      }
    }
    else{
      //std::cerr << this << " using grey image " << std::endl;

#if 1
      // without median filter
      memcpy(m_gray_image->imageData, buffer, bytes);
#else
      // with median filter
      memcpy(m_tmp_image->imageData, buffer, bytes);
      cvResize(m_tmp_image, m_gray_image_f);
      cvSmooth(m_gray_image_f, m_gray_image, CV_MEDIAN, 5);
      std::cerr << "using median" << std::endl;
#endif
      
    }
      
    
    // now the image is in the gray_image
    int corner_count;

    int found = cvFindChessboardCorners( m_gray_image, m_board_sz, m_corners,
					 &corner_count, /*CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE*/
					 CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

    //std::cerr << found << " " << corner_count << std::endl;
    // Get subpixel accuracy on those corners
#if 1
    if( corner_count == m_num_corners ){			
      cvFindCornerSubPix( m_gray_image, m_corners, corner_count, cvSize( 5, 5 ), 
			  cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
    }
#endif



    if(showimages){

    const void * address = static_cast<const void*>(this);
    std::stringstream ss;
    ss << address;  
    std::string name = ss.str();


      // Draw it
      cvDrawChessboardCorners( m_gray_image, m_board_sz, m_corners, corner_count, found );

      if(1 != m_channels){
	// original
	//cvShowImage( name.c_str(), m_gray_image);

	// needed for talk
	
	IplImage* tmp_image = cvCreateImage(cvSize(m_width,m_height), m_depth, m_channels);
	cvCvtColor( m_image, tmp_image, CV_BGR2RGB );
	cvDrawChessboardCorners( tmp_image, m_board_sz, m_corners, corner_count, found );
	cvShowImage( name.c_str(), tmp_image);
	cvReleaseImage(&tmp_image);
      }
      else{
	cvShowImage( name.c_str(), m_gray_image);
      }
      int key = cvWaitKey(10);
    }

    // If we got a good board, add it to our data
    if( corner_count == m_num_corners ){
      
      corners.clear();
      
      for( unsigned j=0; j < m_num_corners; ++j ){
	//std::cerr << j << " " << m_corners[j].x << " " << m_corners[j].y << std::endl;
	uv c;
	c.u = m_corners[j].x;
	c.v = m_corners[j].y;

#if 0
	//std::cerr << "NOTE: if KinectV1 need to smooth and translate the corners from IR to depth by ir_x += 5 and ir_y += 4" << std::endl;
	if(1 == m_channels){
	  c.u += 5;
	  c.v += 4;
	}
#endif


	corners.push_back(c);


      }


      return true;
    }

    return false;
  }



} // namespace kinect

