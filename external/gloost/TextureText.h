
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



/*

  This class uses a texture to render text to the screen. Some of this textures can be found
  in "gloost/data/fonts/". This class (and the textures) support all ASCII characters between
  32 to 126 ( 32 is SPACE ):

   !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}

   A good tool to create this textures is the Bitmap Font Builder made by Thom Wetzel, Jr
   http://www.lmnopc.com/bitmapfontbuilder/
*/


#ifndef GLOOST_TEXTURETEXT_H
#define GLOOST_TEXTURETEXT_H



/// gloost system includes
#include <Vector2.h>
#include <Vector3.h>
#include <TextureManager.h>
#include <UniformTypes.h>



/// cpp includes
#include <string>
#include <glbinding/gl/gl.h>
using namespace gl;
#include <GL/glut.h>
#include <vector>



/// ALIGNMENT DEFINITIONS
#define GLOOST_ALIGN_LEFT    0
#define GLOOST_ALIGN_CENTER  1
#define GLOOST_ALIGN_RIGHT   2



namespace gloost
{


//////////////////////////////////////////////////////////////////////////////////////////


  ///  Writes Text using a Texture with chars from ASCII 32 to 126

class TextureText
{
	public:

    /// class constructor
    TextureText(std::string fontTextureFileName);


    /// class destructor
	  ~TextureText();


    /// initialise the object
//    void init();


    /// start orthogonal projection for text rendering
    void begin();

    /// binds the font texture
    void bindFontTexture();

    /// unbinds the font texture
    void unbindFontTexture();

    /// end orthogonal projection
    void end();


    /// render a line of text
    void renderTextLine(const std::string textLine);

    /// render a line of text
    void renderTextLine(float offsetX, const std::string textLine);

    /// render a line of text
    void renderTextLine(float posH, float posV, const std::string textLine);

    /// 'render' a empty line (to create empty lines)
    void renderFreeLine(unsigned int howmuch = 1);


    /// dertermines the dimesion of an textline
    Vector2 determineLineDimension(const std::string& textLine);

    /// get the texture coordinates for one character on the font texture
    vec4 getCharsTexCoords(const char character);



    /// get the write position
    vec2 getWritePosition();
    /// set the write position
    void  setWritePosition(vec2 writePosition);
    /// set the write position
    void  setWritePosition(const float posX, const float posY);



    /// get the width of the chars
    float getCharWidth();
    /// set the width of the chars
    void  setCharWidth(float charWidth);

    /// get the height of the chars
    float getCharHeight();
    /// set the height of the chars
    void  setCharHeight(float charHeight);

    /// get the space between the chars
    float getCharSpace();
    /// set the space between the chars
    void  setCharSpace(float charSpace);



    /// get space between the lines
    float getLineSpace();
    /// set space between the lines
    void  setLineSpace(float lineSpace);

    /// get the line alignment
    unsigned int getLineAlignment();
    /// set the line alignment
    void  setLineAlignment(unsigned int lineAlignment);



    /// get the alpha value of the chars color
    float getCharAlpha();
    /// set the alpha value of the chars color
    void  setCharAlpha(float charAlpha);

    /// get the scalation of the chars and spaces
    float getCharScale();
    /// set the scalation of the chars and spaces
    void  setCharScale(float charScale);



  private:

    /// Id of the font texture within the TextureManager
    unsigned int _fontTextureId;
    Texture*     _fontTexture;

    float _writePosX;
    float _writePosY;


    float _charWidth;
    float _charHeight;
    float _charSpace;

    float        _lineSpace;
    unsigned int _lineAlignment;

    float _charAlpha;

    float _normalizedCharSize;
    float _charScale;

    std::vector<vec4> _texCoordLookup;


    TextureManager* _texManager;

};


} // namespace gloost


#endif // GLOOST_TEXTURETEXT_H


