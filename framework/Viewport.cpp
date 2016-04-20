
/*
                       ___                            __
                      /\_ \                          /\ \__
                   __ \//\ \     ___     ___     ____\ \  _\
                 /'_ `\ \ \ \   / __`\  / __`\  /  __\\ \ \/
                /\ \ \ \ \_\ \_/\ \ \ \/\ \ \ \/\__   \\ \ \_
                \ \____ \/\____\ \____/\ \____/\/\____/ \ \__\
                 \/___/\ \/____/\/___/  \/___/  \/___/   \/__/
                   /\____/
                   \_/__/

                   OpenGL framework for fast demo programming

                             http://www.gloost.org

    This file is part of the gloost framework. You can use it in parts or as
       whole under the terms of the GPL (http://www.gnu.org/licenses/#GPL).

            gloost is being created by Felix Weiﬂig and Stephan Beck

     Felix Weiﬂig (thesleeper@gmx.net), Stephan Beck (stephan@pixelstars.de)
*/



/// gloost system includes
#include <Viewport.h>



namespace gloost
{



  ///

Viewport::Viewport(GLsizei x, GLsizei y, GLsizei width, GLsizei height):
    m_vp_params(),
    m_old_vp_params()
{
  m_vp_params[0] = x;
  m_vp_params[1] = y;
  m_vp_params[2] = width;
  m_vp_params[3] = height;
}


///////////////////////////////////////////////////////////////////////////////


  ///

Viewport::~Viewport()
{

}


///////////////////////////////////////////////////////////////////////////////


  /// configure this viewport

void
Viewport::set(GLsizei x, GLsizei y, GLsizei width, GLsizei height)
{
  m_vp_params[0] = x;
  m_vp_params[1] = y;
  m_vp_params[2] = width;
  m_vp_params[3] = height;
}


///////////////////////////////////////////////////////////////////////////////


  /// use this viewport

void
Viewport::enter(bool push)
{

  if(push)
  {
    glGetIntegerv(GL_VIEWPORT,m_old_vp_params);
  }

  glViewport ((GLsizei) m_vp_params[0],
  (GLsizei) m_vp_params[1],
  (GLsizei) m_vp_params[2],
  (GLsizei) m_vp_params[3]);
}


///////////////////////////////////////////////////////////////////////////////


  /// don't use this viewport anymore (recover old viewport if push was true)

void Viewport::leave()
{
  glViewport ((GLsizei) m_old_vp_params[0],
  (GLsizei) m_old_vp_params[1],
  (GLsizei) m_old_vp_params[2],
  (GLsizei) m_old_vp_params[3]);
}


///////////////////////////////////////////////////////////////////////////////


  ///




} // namespace gloost

