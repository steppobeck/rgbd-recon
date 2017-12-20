#ifndef SYS_FEEDBACKRECEIVER_H
#define SYS_FEEDBACKRECEIVER_H

#include <glm/gtc/matrix_transform.hpp>

#include <string>

namespace boost{
  class thread;
  class mutex;
}

namespace sys{


struct feedback{
  glm::mat4 cyclops_mat;
  glm::mat4 screen_mat;
  glm::mat4 model_mat;
  unsigned recon_mode;
};


class FeedbackReceiver{

 public:
  FeedbackReceiver(const feedback& initial_feedback, const char* client_socket);

  ~FeedbackReceiver();

  feedback get();

 private:

  void reveive_loop(std::string client_socket);
  bool           m_running;
  boost::thread* m_receive_thread;
  boost::mutex*  m_mutex;
  feedback m_fb;

};

}

#endif // #ifndef SYS_FEEDBACKRECEIVER_H
