#include "FeedbackReceiver.h"

#include <zmq.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <iostream>


namespace sys{


  FeedbackReceiver::FeedbackReceiver(const feedback& initial_feedback, const char* client_socket)
    : m_running(true),
      m_receive_thread(),
      m_mutex(new boost::mutex),
      m_fb(initial_feedback)
  {
    std::string client_s(client_socket);
    m_receive_thread = new boost::thread(boost::bind(&FeedbackReceiver::reveive_loop, this, client_s));
  }


  FeedbackReceiver::~FeedbackReceiver(){
    m_running = false;
    m_receive_thread->join();
    delete m_mutex;
  }


  feedback
  FeedbackReceiver::get(){
    boost::mutex::scoped_lock lock(*m_mutex);
    return m_fb;
  }


  void 
  FeedbackReceiver::reveive_loop(std::string client_socket){
    // prepare zmq for SUBSCRIBER

    zmq::context_t ctx(1); // means single threaded
    zmq::socket_t  socket(ctx, ZMQ_SUB); // means a subscriber
    socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);;
    uint32_t hwm = 1;
    socket.setsockopt(ZMQ_RCVHWM,&hwm, sizeof(hwm));
    std::cout << "opening feedback receiver socket: " << client_socket << std::endl;
    std::string endpoint(std::string("tcp://") + client_socket);
    socket.connect(endpoint.c_str());


    while(m_running){


      zmq::message_t zmqm(sizeof(feedback));
      socket.recv(&zmqm);
      feedback tmp_fb;
      memcpy((unsigned char*) &tmp_fb, (const unsigned char* ) zmqm.data(), sizeof(feedback));
      {
	boost::mutex::scoped_lock lock(*m_mutex);
	m_fb = tmp_fb;
      }
    }

    // free zmq
  }
 
}
