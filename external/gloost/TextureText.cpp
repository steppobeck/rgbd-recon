
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



/// gloost includes
#include <TextureText.h>
#include <gloostRenderGoodies.h>


/// cpp includes


namespace gloost
{


///////////////////////////////////////////////////////////////////////////////


  /// class constructor

TextureText::TextureText(std::string fontTextureFileName):
    _fontTextureId(0),
    _fontTexture(0),
    _writePosX(0),
    _writePosY(400),
    _charWidth(16),
    _charHeight(16),
    _charSpace(0),
    _lineSpace(0),
    _lineAlignment(GLOOST_ALIGN_LEFT),
    _charAlpha(1.0),
    _normalizedCharSize(0.1),
    _charScale(1.0),
    _texCoordLookup(),
    _texManager(TextureManager::getInstance())
{

  /// load the font texture
  _fontTextureId = _texManager->createTexture(fontTextureFileName, 1);
  _fontTexture   = _texManager->getTextureWithoutRefcount(_fontTextureId);


  // calculate the chars width and height
  _charWidth           = (float)_fontTexture->getWidth()/16.0;
  _charHeight          = (float)_fontTexture->getHeight()/16.0;
  _normalizedCharSize = (float)_charWidth/(float)_fontTexture->getWidth();

  _charSpace = -_charWidth/2.0;



  /// calculate the texcoords for each char and store it in a lookup
  for(int i=0; i != 255; ++i)
  {
    float texCoordOffsetU = i % (16);
    float texCoordOffsetV = int( i / (16.0));

    texCoordOffsetU *= _normalizedCharSize;
    texCoordOffsetV *= _normalizedCharSize;

    _texCoordLookup.push_back( vec4(texCoordOffsetU,
                                    texCoordOffsetU+_normalizedCharSize,
                                    texCoordOffsetV,
                                    texCoordOffsetV+_normalizedCharSize));
  }

}


///////////////////////////////////////////////////////////////////////////////


  ///

TextureText::~TextureText()
{
  _texManager->dropReference(_fontTextureId);
}


///////////////////////////////////////////////////////////////////////////////


  /// start orthogonal projection for text rendering

void
TextureText::begin()
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);


    int viewportParams[4];
    glGetIntegerv(GL_VIEWPORT, viewportParams);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(viewportParams[0], viewportParams[2], viewportParams[1], viewportParams[3], 0.1, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 5.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);


    // color
    glColor4f(1.0, 1.0, 1.0, _charAlpha);


    // find the font texture
    bindFontTexture();

    glPushMatrix();
}


///////////////////////////////////////////////////////////////////////////////


  /// binds the font texture

void
TextureText::bindFontTexture()
{
  _fontTexture->bind(GL_TEXTURE0, 0);
}


///////////////////////////////////////////////////////////////////////////////


  /// unbinds the font texture

void
TextureText::unbindFontTexture()
{
  _fontTexture->unbind();
}


///////////////////////////////////////////////////////////////////////////////


  /// end orthogonal projection

void
TextureText::end()
{



      unbindFontTexture();
    glPopMatrix();
  glPopAttrib();
}


///////////////////////////////////////////////////////////////////////////////


  /// render a line of text

void
TextureText::renderTextLine(float posH, float posV, const std::string textLine)
{

  _writePosX = posH;
  _writePosY = posV;

  renderTextLine(0.0f, textLine);

}


///////////////////////////////////////////////////////////////////////////////


  /// render a line of text

void
TextureText::renderTextLine(const std::string textLine)
{
  renderTextLine(0.0f, textLine);
}


///////////////////////////////////////////////////////////////////////////////


  /// render a line of text

void
TextureText::renderTextLine(float offsetX, const std::string textLine)
{

  const char* textChars  = textLine.c_str();
  const float lineLength = textLine.length();


  // Line alignment
  int alignmentOffsetX = 0;

  if (_lineAlignment == (unsigned int)GLOOST_ALIGN_CENTER)
  {
    alignmentOffsetX = (int)((-lineLength-0.5)*(_charWidth+_charSpace)*_charScale/2.0f);
  }
  else if (_lineAlignment == (unsigned int)GLOOST_ALIGN_RIGHT)
  {
    alignmentOffsetX = (int)(-lineLength*(_charWidth+_charSpace)*_charScale);
  }

  glPushMatrix();
  {
    // transform for the whole line
    glTranslatef(_writePosX + offsetX + alignmentOffsetX, 0.0, 0.0);

    // draw quads with modified texCoords for every single char
    for (unsigned int i=0; i != lineLength; ++i )
    {

      // get the texture coordinates from the lookup table
      vec4 texCoords = _texCoordLookup[int(*textChars)];
      glPushMatrix();
      {
        glTranslatef(i*(_charWidth+_charSpace)*_charScale, _writePosY, 0.0);
        glScalef(_charWidth*_charScale, _charHeight*_charScale, 1.0);
        gloost::drawQuad(texCoords.r, texCoords.g,
                         texCoords.b, texCoords.a);
      }
      glPopMatrix();
      ++textChars;
    }

  }
  glPopMatrix();

  _writePosY -= (_charHeight+_lineSpace)*_charScale;

}


///////////////////////////////////////////////////////////////////////////////


  /// 'render' a empty line (to create empty lines)

void
TextureText::renderFreeLine(unsigned int howmuche)
{
  _writePosY -= (_charHeight+_lineSpace)*_charScale*howmuche;
}


///////////////////////////////////////////////////////////////////////////////


  /// dertermines the dimesion of an textline

Vector2
TextureText::determineLineDimension(const std::string& textLine)
{
  return Vector2(textLine.length()*(_charWidth+_charSpace)*_charScale,
                 _charHeight*_charScale);
}


///////////////////////////////////////////////////////////////////////////////


  /// get the texture coordinates for one character on the font texture

vec4
TextureText::getCharsTexCoords(const char character)
{
  return _texCoordLookup[character];
}


///////////////////////////////////////////////////////////////////////////////


  /// get the write position

vec2
TextureText::getWritePosition()
{
  return vec2(_writePosX, _writePosY);
}

///////////////////////////////////////////////////////////////////////////////


  /// set the write position

void
TextureText::setWritePosition(vec2 writePosition)
{
  _writePosX = writePosition.u;
  _writePosY = writePosition.v;
}

///////////////////////////////////////////////////////////////////////////////


  /// set the write position

void
TextureText::setWritePosition(const float posX, const float posY)
{
  _writePosX = posX;
  _writePosY = posY;
}


///////////////////////////////////////////////////////////////////////////////


  /// get the width of the chars

float
TextureText::getCharWidth()
{
  return _charWidth;
}


///////////////////////////////////////////////////////////////////////////////


  /// set the width of the chars

void
TextureText::setCharWidth(float charWidth)
{
  _charWidth = charWidth;
}


///////////////////////////////////////////////////////////////////////////////


  /// get the height of the chars

float
TextureText::getCharHeight()
{
  return _charHeight;
}


///////////////////////////////////////////////////////////////////////////////


  /// set the height of the chars

void
TextureText::setCharHeight(float charHeight)
{
  _charHeight = charHeight;
}


///////////////////////////////////////////////////////////////////////////////


  /// get space between the lines

float
TextureText::getLineSpace()
{
  return _lineSpace;
}


///////////////////////////////////////////////////////////////////////////////


  /// set space between the lines

void
TextureText::setLineSpace(float lineSpace)
{
  _lineSpace = lineSpace;
}


///////////////////////////////////////////////////////////////////////////////


  /// get the line alignment

unsigned int
TextureText::getLineAlignment()
{
  return _lineAlignment;
}


///////////////////////////////////////////////////////////////////////////////


  /// set the line alignment

void
TextureText::setLineAlignment(unsigned int lineAlignment)
{
  _lineAlignment = lineAlignment;
}


///////////////////////////////////////////////////////////////////////////////


  /// get the space between the chars

float
TextureText::getCharSpace()
{
  return _charSpace;
}


///////////////////////////////////////////////////////////////////////////////


  /// set the space between the chars

void
TextureText::setCharSpace(float charSpace)
{
  _charSpace = charSpace;
}


///////////////////////////////////////////////////////////////////////////////


  /// get the alpha value of the chars color

float
TextureText::getCharAlpha()
{
  return _charAlpha;
}


///////////////////////////////////////////////////////////////////////////////



/// set the alpha value of the chars color
void
TextureText::setCharAlpha(float charAlpha)
{
  _charAlpha = charAlpha;
}


///////////////////////////////////////////////////////////////////////////////




/// get the scalation of the chars and spaces
float
TextureText::getCharScale()
{
  return _charScale;
}


///////////////////////////////////////////////////////////////////////////////



/// set the scalation of the chars and spaces
void
TextureText::setCharScale(float charScale)
{
  _charScale = charScale;
}


///////////////////////////////////////////////////////////////////////////////









///////////////////////////////////////////////////////////////////////////////



} // namespace gloost
