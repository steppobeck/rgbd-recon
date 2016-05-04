#ifndef MVT_FOURTILEDWINDOW_H
#define MVT_FOURTILEDWINDOW_H

#include <UniformTypes.h>

namespace mvt{


  class FourTiledWindow{

  public:

    FourTiledWindow(unsigned width, unsigned height);
    ~FourTiledWindow();

    void resize(unsigned width, unsigned height);

    void setViewport(unsigned windowNum);

    float getAspect(unsigned windowNum);

    // here goes glutMouseFunc, glutMotionFunc, glutPassiveMotionFunc
    void mouse(int button, int state, int x, int y);
    void motion(int x, int y);
    void passive(int x, int y);

    void specialKey(int key, int x, int y);

    // from PassiveMotion and Motion
    unsigned getActiveWindow();
    gloost::vec2 getRelativeLocation(unsigned windowNum);


    void endFrame();
    gloost::vec2 getButtonSpeed(unsigned buttonNum);

    // currently only draws the separetors as 2 white lines
    void draw();

    void drawCursor();

    unsigned getWidth();
    unsigned getHeight();

  private:
    unsigned findActive(int x, int y);
    unsigned m_width;
    unsigned m_height;
    int m_mouseX;
    int m_mouseY;
    unsigned m_button;
    unsigned m_pressed;
    unsigned m_activeWindow;
    gloost::vec2 m_buttonSpeeds[3];
    

  };

}// namepsace mvt



#endif // #ifndef MVT_FOURTILEDWINDOW_H
