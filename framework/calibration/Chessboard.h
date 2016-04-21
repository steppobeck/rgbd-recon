



#ifndef MVT_CHESSBOARD_H
#define MVT_CHESSBOARD_H





/// cpp includes
#include <GL/glew.h>
#include <GL/gl.h>


namespace mvt {


  //  Generates and renders resH x resV chessboard (black and white) with width and height patterns that lies in the XY plane

  class Chessboard {
  public:

    // class constructor
    Chessboard(float width, float height, unsigned resH, unsigned resV);

    // class destructor
    ~Chessboard();


    // inits the opengl stuff
    void init();

    // draws the chessbaord
    void draw();

  protected:
    float m_width;
    float m_height;
    unsigned int m_resH;
    unsigned int m_resV;
    GLuint       m_displayListHandle;

};


} // namespace mvt


#endif // MVT_CHESSBOARD_H


