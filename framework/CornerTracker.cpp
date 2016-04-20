#include "CornerTracker.h"

#include <algorithm>

#include <iostream>
// boost includes
#include <boost/thread/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>



namespace kinect{

  CornerTracker::CornerTracker(int number_of_images, unsigned width, unsigned height, int numchannels, int channel_depth, int chessboard_width, int chessboard_height):
  m_numer_of_images(number_of_images),
  m_image_width(width),
  m_image_height(height),
  m_numchannels(numchannels),
  m_channel_depth(channel_depth),
  m_chessboard_width(chessboard_width),
  m_chessboard_height(chessboard_height)
  {
    for (unsigned i = 0; i < m_numer_of_images; ++i)
    {
      m_cornerDetectors.push_back( new OpenCVChessboardCornerDetector(m_image_width, m_image_height, m_channel_depth, m_numchannels, m_chessboard_width, m_chessboard_height) );
      m_results.push_back( new std::vector<uv> );      
    }
  }

  CornerTracker::~CornerTracker()
  {
    for (unsigned i = 0; i < m_numer_of_images; ++i)
    {
      assert (m_cornerDetectors[i] != nullptr);
      delete m_cornerDetectors[i];
      m_cornerDetectors[i] = nullptr;

      assert (m_results[i] != nullptr);
      delete m_results[i];
      m_results[i] = nullptr;
    }
  }

  void CornerTracker::processByThread(unsigned char* image_data, unsigned id)
  {
    m_results[id]->clear();
    unsigned num_bytes((m_numchannels * m_channel_depth * m_image_height * m_image_width)/8);
    
    bool success = m_cornerDetectors[id]->process(image_data, num_bytes, true);
    if(!success)
      return;
    for(unsigned i = 0; i < m_cornerDetectors[id]->corners.size(); ++i){
       m_results[id]->push_back(m_cornerDetectors[id]->corners[i]);
    }

  }

  std::vector< std::vector<uv>* > CornerTracker::process(std::vector<unsigned char* > image_data)
  {

    //boost::thread_group threadGroup;
     
    for (unsigned i = 0; i < m_numer_of_images; ++i)
    {
      //threadGroup.create_thread(boost::bind(&CornerTracker::processByThread, this, image_data[i], i));
      processByThread(image_data[i], i);
    }

    //threadGroup.join_all();

    
    return m_results;
  }


}
