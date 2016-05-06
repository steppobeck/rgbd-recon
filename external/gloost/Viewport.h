
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



#ifndef GLOOST_VIEWPORT_H
#define GLOOST_VIEWPORT_H


/// gloost system includes


/// cpp includes
#include <GL/glew.h>


namespace gloost
{


  /// wrapper for glViewport with push/pop functionality

class Viewport
{

  public:

    Viewport(GLsizei x, GLsizei y, GLsizei width, GLsizei height);
    ~Viewport();

    /// configure this viewport
    void set(GLsizei x, GLsizei y, GLsizei width, GLsizei height);

    /// use this viewport
    void enter(bool push = true);

    /// don't use this viewport anymore (recover old viewport if push was true)
    void leave();


  private:

    GLsizei m_vp_params[4];
    GLsizei m_old_vp_params[4];


};


} // namespace gloost

#endif // #ifndef GLOOST_VIEWPORT_H
