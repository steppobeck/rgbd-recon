
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



/// cpp includes
#include <iostream>



/// gloost system includes
#include <Texture.h>
#include <glErrorUtil.h>

#include <string.h>


namespace gloost
{


///////////////////////////////////////////////////////////////////////////////


  /// class constructor

Texture::Texture(unsigned int width,
		            unsigned int height,
		            GLenum target,
		            GLint internalFormat,
		            GLenum pixelFormat,
		            GLenum pixelType):
    MultiGlContext(1),
    _target(target),
    _internalFormat(internalFormat),
    _width(width),
    _height(height),
    _depth(0),
    _pixelFormat(pixelFormat),
    _pixelType(pixelType),
    _dirty(true),
    _textureUnitIdsForContexts(GLOOST_SYSTEM_NUM_RENDER_CONTEXTS, 0),
    _mipmapsEnabled(false),
    //_surface(0),
    _pixelData(0),
    _glTexParameters(),
    _texMatrix(),
    _useTextureMatrix(0),
    _shared(0),
    _fileName("default_texture_name.fake")

{
  // Set some default texParameter so you will see something
  setTexParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  setTexParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  setTexParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
  setTexParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
}

///////////////////////////////////////////////////////////////////////////////


  /// class constructor

Texture::Texture(const std::string& fileName):
    MultiGlContext(1),
    _target(GL_TEXTURE_2D),
    _internalFormat(GL_RGB),
    _width(1024),
    _height(1024),
    _depth(0),
    _pixelFormat(GL_RGB),
    _pixelType(GL_UNSIGNED_BYTE),
    _dirty(GLOOST_BITMASK_ALL_SET),
    _textureUnitIdsForContexts(GLOOST_SYSTEM_NUM_RENDER_CONTEXTS, 0),
    _mipmapsEnabled(false),
    //_surface(0),
    _pixelData(0),
    _glTexParameters(),
    _texMatrix(),
    _useTextureMatrix(0),
    _shared(0),
    _fileName(fileName)
{

	// Set some default texParameter so you will see something
  setTexParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  setTexParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  setTexParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
  setTexParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	//_surface = IMG_Load(fileName.c_str());



	// If the image loaded
	if( /*_surface != NULL*/0 )
	{
#if 0
    unsigned int bytesPerPixel = (int)_surface->format->BytesPerPixel;

    _pixelData = _surface->pixels;

#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_MESSAGES
    std::cout << std::endl;
    std::cout << std::endl << "Message from Texture::Texture(std::string) on SharedResource " << getSharedResourceId() << ":";
    std::cout << std::endl << "             Image \"" << fileName << "\" loaded as " << bytesPerPixel*8 << " bit texture.";
		std::flush(std::cout);
#endif

		_width  = _surface->w;
		_height = _surface->h;


		_target = GL_TEXTURE_2D;

    // Alpha 8 bit
		if (bytesPerPixel == 1)
		{
      _internalFormat = GL_LUMINANCE;
      _pixelFormat    = GL_LUMINANCE;
      _pixelType      = GL_UNSIGNED_BYTE;
		}
    // RGB 24 bit
		else if (bytesPerPixel == 3)
		{
      _internalFormat = GL_RGB;
      _pixelFormat    = GL_RGB;
      _pixelType      = GL_UNSIGNED_BYTE;
		}
		// RGBA 32 bit
		else if (bytesPerPixel == 4)
		{
      _internalFormat = GL_RGBA;
      _pixelFormat    = GL_RGBA;
      _pixelType      = GL_UNSIGNED_BYTE;
		}
#endif
	}
	else
	{


#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_ERRORS
    std::cout << std::endl;
    std::cout << std::endl << "ERROR in Texture::Texture() on SharedResource " << getSharedResourceId() << ":";
		std::cout << std::endl << "         Unable to load file \"" << fileName <<  "\"";
		std::cout << std::endl << "         creating empty surface here, so";
		std::cout << std::endl << "         the program dosn't crash...";
		std::flush(std::cout);
#endif

	}
}

///////////////////////////////////////////////////////////////////////////////


Texture::Texture(unsigned int width,
                 unsigned int height,
                 unsigned int depth,
                 void*        pixeldata,
                 GLenum target,
                 GLint internalFormat,
                 GLenum pixelFormat,
                 GLenum pixelType):
    MultiGlContext(1),
    _target(target),
    _internalFormat(internalFormat),
    _width(width),
    _height(height),
    _depth(depth),
    _pixelFormat(pixelFormat),
    _pixelType(pixelType),
    _dirty(true),
    _textureUnitIdsForContexts(32, 0),
    _mipmapsEnabled(false),
    //_surface(0),
    _pixelData(pixeldata),
    _glTexParameters(),
    _texMatrix(),
    _useTextureMatrix(0),
    _shared(0),
    _fileName("default_texture_name.fake")

{

	// Set some default texParameter so you will see something
  setTexParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  setTexParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  setTexParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
  setTexParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
  setTexParameter(GL_TEXTURE_WRAP_R, GL_REPEAT);

}


///////////////////////////////////////////////////////////////////////////////


  /// destructor

Texture::~Texture()
{
  freeHostData();
//
//
//  for (unsigned)
//

}


//////////////////////////////////////////////////////////////////////////////////////////


  /// ...

void
Texture::freeHostData()
{
#if 0
  /// delete pixeldata
  if (_pixelData != _surface->pixels)
  {
    delete _pixelData;
    _pixelData = 0;
  }


  /// remove the texture from ram
  if (_surface)
  {
    SDL_FreeSurface(_surface);
    _surface = 0;
  }
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////


  /// set a glTexParameter for this texture to be applied when texture is gernerated

void
Texture::setTexParameter(GLenum propertyName, float value)
{
  _glTexParameters[propertyName] = value;
  setDirty();
}


//////////////////////////////////////////////////////////////////////////////////////////


  /// get TexParameter

float
Texture::getTexParameter(GLenum propertyName)
{
  std::map<GLenum, float>::iterator pos = _glTexParameters.find(propertyName);

  if (pos == _glTexParameters.end())
  {

    float value = 0;
    glGetTexParameterfv(_target, propertyName, &value );

    return value;
  }
  else
  {
    return (*pos).second;
  }
}


//////////////////////////////////////////////////////////////////////////////////////////


  /// remove a TexParameter from this texture to use GL default value

void
Texture::removeTexParameter(GLenum propertyName)
{
  std::map<GLenum, float>::iterator pos = _glTexParameters.find(propertyName);

  if (pos == _glTexParameters.end())
  {
    return;
  }
  else
  {
    _glTexParameters.erase(pos);
  }
}


//////////////////////////////////////////////////////////////////////////////////////////


  /// get a map with all Texparameter for this texture

std::map<GLenum, float>&
Texture::getTexParameterMap()
{
  return _glTexParameters;
}


///////////////////////////////////////////////////////////////////////////////


  /// get width of the texture

unsigned int
Texture::getWidth() const
{
  return _width;
}


///////////////////////////////////////////////////////////////////////////////


  /// get height of the texture

unsigned int
Texture::getHeight() const
{
  return _height;
}


///////////////////////////////////////////////////////////////////////////////


  /// get height of the texture

float
Texture::getAspecRatio() const
{
  if (_height > 0)
  {
    return (float)_width/(float)_height;
  }

  return 0.0f;
}


///////////////////////////////////////////////////////////////////////////////


  /// get gl's texture target for this texture

GLenum
Texture::getTarget()
{
  return _target;
}


///////////////////////////////////////////////////////////////////////////////


  /// get gl's internal format for this texture

GLint
Texture::getInternalFormat()
{
  return _internalFormat;
}


///////////////////////////////////////////////////////////////////////////////


  /// get gl's pixel format for this texture

GLenum
Texture::getPixelFormat()
{
  return _pixelFormat;
}


///////////////////////////////////////////////////////////////////////////////


  /// get gl's pixel type for this texture

GLenum
Texture::getPixelType()
{
  return _pixelType;
}


///////////////////////////////////////////////////////////////////////////////


  /// get the GL texture handle

unsigned int
Texture::getTextureHandle(unsigned int contextId)
{

  if (!handleInContextExists(contextId))
  {
    initInContext(contextId);
  }

  return getResourceHandleForContext(contextId);
}


///////////////////////////////////////////////////////////////////////////////


  /// writes the surfaces to a jpeg file (may be upside down)

//void
//Texture::writeToFile(std::string path)
//{
////  Screenshot writer(_surface->w, _surface->h, 100);
////  writer.write(path, ( uchar* ) _surface->pixels);
//}


///////////////////////////////////////////////////////////////////////////////


  /// returns the color value of the pixel position x,y as vec4

vec4
Texture::getColor (int x , int y )
{
#if 0
  //determine position
  char* pPosition = ( char* ) _pixelData ;

  //offset by y
  pPosition += (_surface->pitch * y) ;

  //offset by x
  pPosition += (_surface->format->BytesPerPixel * x) ;


  //copy pixel data
  Uint32 color = 0;

  memcpy (&color, pPosition, _surface->format->BytesPerPixel );


  //convert color
  SDL_Color sdlColor;

  SDL_GetRGBA (color, _surface->format, &sdlColor.r,
                                        &sdlColor.g,
                                        &sdlColor.b,
                                        &sdlColor.unused);

  return vec4(((float)(sdlColor.r))/255.0,
              ((float)(sdlColor.g))/255.0,
              ((float)(sdlColor.b))/255.0,
              ((float)(sdlColor.unused))/255.0);
#endif
  return vec4(0.0,0.0,0.0,0.0);
}


///////////////////////////////////////////////////////////////////////////////


  /// set the dirty flag to signal texture changes

void
Texture::setDirty()
{
  _dirty.setFlags(true);
}


///////////////////////////////////////////////////////////////////////////////


  /// unset the dirty flag to signal that texture is up to date

void
Texture::setClean(unsigned int contextId)
{
  _dirty.setFlag(contextId, false);
}


///////////////////////////////////////////////////////////////////////////////


  /// was the texture modified

bool
Texture::isDirty(unsigned int contextId) const
{
  return _dirty.getFlag(contextId);
}


///////////////////////////////////////////////////////////////////////////////


  /// generates a Gl texture from this texture

/*virtual*/
bool
Texture::initInContext(unsigned int contextId)
{

//#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_MESSAGES
//    std::cout << std::endl;
//    std::cout << std::endl << "Message from Message from Texture::initInContext() on SharedResource " << getSharedResourceId() << ":";
//    std::cout << std::endl << "             Building for context " << contextId << ".";
//    std::cout << std::endl;
//#endif

  if (handleInContextExists(contextId))
  {
    ungenerate(contextId);
  }


  setResourceHandleForContext(generate(_width,
                                       _height,
                                       _depth,
                                       _target,
                                       _internalFormat,
                                       _pixelFormat,
                                       _pixelType,
                                       contextId),
                                       contextId);

  // Texture on your gpu ram is now up to date
  setClean(contextId);
  return true;
}


///////////////////////////////////////////////////////////////////////////////


  /// generates a Gl texture from this texture

unsigned int
Texture::generate( unsigned int width,
                   unsigned int height,
                   unsigned int depth,
                   GLenum target,
                   GLint internalFormat,
                   GLenum pixelFormat,
                   GLenum pixelType,
                   unsigned int contextId)
{


  unsigned int newTextureHandle = 0;


  //Create a texture
  glGenTextures(1, &newTextureHandle);

  // activate texture unit (could be redundant)
  // from otaco: it is
  // from otaco: its even deadly for UniformSets
  glActiveTexture(_textureUnitIdsForContexts[contextId]);

  // Load the texture
  glBindTexture(target, newTextureHandle);


  // add texParameter
  std::map<GLenum, float>::const_iterator index = _glTexParameters.begin();
  std::map<GLenum, float>::const_iterator ende  = _glTexParameters.end();

  for (; index != ende; index++ )
  {
    glTexParameterf(_target, (*index).first, (*index).second);
  }

#if 0
  if (_surface && target == GL_TEXTURE_2D)
  {
    // Generate the texture from SDL surface (always GL_TEXTURE_2D)
    if(_mipmapsEnabled)
    {

      gluBuild2DMipmaps(target,
			                  internalFormat,
			                  _surface->w,
			                  _surface->h,
			                  pixelFormat,
			                  pixelType,
			                  _surface->pixels);

    }
    else
    {
      glTexImage2D(target,
		   0,
		   internalFormat,
		   _surface->w,
		   _surface->h,
		   0,
		   pixelFormat,
		   pixelType,
		   _surface->pixels);
    }
  }
  else if (_pixelData)
  {

    if(target == GL_TEXTURE_1D)
    {
      // Generate a 1D texture from SDL surface
      glTexImage1D(GL_TEXTURE_1D,
                   0,
                   internalFormat,
                   width,
                   0,
                   pixelFormat,
                   pixelType,
                   _pixelData);
    }
    else if (target == GL_TEXTURE_2D)
    {
      // Generate a 2D texture from SDL surface
      glTexImage2D(GL_TEXTURE_2D,
                   0,
                   internalFormat,
                   width,
                   height,
                   0,
                   pixelFormat,
                   pixelType,
                   _pixelData);

    }
    else if (target == GL_TEXTURE_3D)
    {
      // Generate a 2D texture from SDL surface
      glTexImage3D(GL_TEXTURE_3D,
                   0,
                   internalFormat,
                   width,
                   height,
                   depth,
                   0,
                   pixelFormat,
                   pixelType,
                   _pixelData);
    }
  }
  else
  {
    // Generate the empty texture
    glTexImage2D(target,
                 0,
                 internalFormat,
                 width,
                 height,
                 0,
                 pixelFormat,
                 pixelType,
                 0);

  }
#endif

  glBindTexture(target, 0);

  return newTextureHandle;
}


///////////////////////////////////////////////////////////////////////////////


  /// remove a resource from a context

/*virtual*/
void
Texture::removeFromContext(unsigned int contextId)
{
  ungenerate(contextId);
}


///////////////////////////////////////////////////////////////////////////////


  /// remove the texture from the gpu ram

//void
//Texture::ungenerate(unsigned int &texHandle)
//{
//  if (texHandle)
//  {
//    glDeleteTextures(1, &texHandle);
//    texHandle = 0;
//  }
//  setDirty();
//}


///////////////////////////////////////////////////////////////////////////////


  /// remove the texture from the gpu ram

void
Texture::ungenerate(unsigned int contextId)
{
  // this shoud be tested befor calling ungenerate
//  if (handleInContextExists(contextId))
  {
    glDeleteTextures(1, &getResourceHandleForContext(contextId) );
    setResourceHandleForContext(0, contextId);
  }
  setDirty();
}


///////////////////////////////////////////////////////////////////////////////


  /// binds the Texture to GL state

void
Texture::bind(unsigned int texUnitId, unsigned int contextId)
{
  /// set internal id needed for unbind
  _textureUnitIdsForContexts[contextId] = texUnitId;


  /// if this texture is not up to date we have to remove it first from the gpu ram
  if (_dirty.getFlag(contextId))
  {

    if (handleInContextExists(contextId))
    {
      ungenerate(contextId);
    }

    /// generate and setClean
    initInContext(contextId);
  }


  /// is this case possible???
  if (!handleInContextExists(contextId))
  {
    initInContext(contextId);
  }


  // activate texture unit (could be redundant)
  glActiveTexture(texUnitId);


  // bind the texture to the unit
  glBindTexture(_target, getResourceHandleForContext(contextId));


  // apply texture matrix if needed
  if (_useTextureMatrix)
  {
    glMatrixMode(GL_TEXTURE);
    gloostLoadMatrix(_texMatrix.data());
    glMatrixMode(GL_MODELVIEW);
  }

}


///////////////////////////////////////////////////////////////////////////////


  /// unbind Texture

void
Texture::unbind(unsigned int contextId)
{
	/// yes we do! just for safety
	glActiveTexture(_textureUnitIdsForContexts[contextId]);

	// bind zero to this unit to disable this texture
	glBindTexture(_target, 0);

  // apply texture matrix if needed
  if (_useTextureMatrix)
  {
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
  }


}


///////////////////////////////////////////////////////////////////////////////


  /// read the (changed) texture from the gfx-card back into the surface

void
Texture::readBack()
{


  /* http://www.mathematik.uni-dortmund.de/~goeddeke/gpgpu/tutorial.html
     glGetTexImage: http://developer.3dlabs.com/documents/glmanpages/glgetteximage.htm

  */

//  bind(GL_TEXTURE0);
//
//  if (_pixelData == 0)
//  {
//    std::cout << std::endl << "-> : " << " allocating";
//
//    GLubyte* buffer = (GLubyte *)malloc(_width*_height*4);
//    _pixelData = buffer;
//  }
//
//
//  std::cout << std::endl << "-> : " << " done";
//
//
//  glGetTexImage(_target,
//                0,
//                _internalFormat,
//                _pixelType,
//                _pixelData);
//
//  for (unsigned int i=0; i!=_width*_height; ++i)
//  {
//    std::cout << std::endl << ((unsigned int)(&_pixelData)[i]);
//  }
//
//  unbind();
}


///////////////////////////////////////////////////////////////////////////////


  /// copys the current framebuffer into the surface

void
Texture::copyFramebuffer()
{

  // http://developer.3dlabs.com/documents/GLmanpages/glreadpixels.htm

//  glReadPixels(0,
//               0,
//               _surface->w,
//               _surface->h,
//               GL_RGB,
//               GL_UNSIGNED_BYTE,
//               _surface->pixels);
//
//  CheckErrorsGL("Texture::copyFramebuffer():    CheckErrorsGL:", std::cout);
//
//  setDirty();
}


///////////////////////////////////////////////////////////////////////////////


/// set the texture matrix
void
Texture::setTextureMatrix(const Matrix& matrix)
{
  _texMatrix = matrix;
}


///////////////////////////////////////////////////////////////////////////////


  /// get the texture matrix

const Matrix&
Texture::getTextureMatrix()
{
  return _texMatrix;
}


///////////////////////////////////////////////////////////////////////////////


  /// use the textureMatrix

void
Texture::enableTextureMatrix(bool trueOrFalse)
{
  _useTextureMatrix = trueOrFalse;
}


///////////////////////////////////////////////////////////////////////////////


  /// returns 1 if texture matrix is enabled

bool
Texture::isTextureMatrixEnabled()
{
  return _useTextureMatrix;
}


///////////////////////////////////////////////////////////////////////////////


  /// use Mipmapping (don't work if attachet to an Fbo)

void
Texture::enableMipmaps(bool trueOrFalse)
{
   _mipmapsEnabled = trueOrFalse;

  if(_mipmapsEnabled)
  {
    setTexParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    setTexParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  }
  else
  {
    setTexParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    setTexParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  setDirty();
}


///////////////////////////////////////////////////////////////////////////////


  /// returns 1 if Mipmapping is enabled

bool
Texture::isMipmapsEnabled()
{
   return _mipmapsEnabled;
}


///////////////////////////////////////////////////////////////////////////////



/// set this texture shared or unshared (this flag is used by the TextureManager)
void
Texture::setShared(bool shared)
{
   _shared = shared;
}

///////////////////////////////////////////////////////////////////////////////



/// returns 1 if this texture is shared within the TextureManager
bool
Texture::isShared()
{
   return _shared;
}


///////////////////////////////////////////////////////////////////////////////


  /// get the sdl surface of this texture

void*
Texture::getPixels()
{
   return _pixelData ;
}


///////////////////////////////////////////////////////////////////////////////


  /// replaces pixeldata on host and server side, new pixels must have same format as old ones! (Texture will not become _dirty)

void
Texture::updatePixels(unsigned char* pixelbuffer,
                      unsigned int lengthInByte,
                      unsigned int contextId)
{

  // update host side
  memcpy(_pixelData,(void*)pixelbuffer, lengthInByte);

  // this is not so cool if GL_TEXTURE7 is used in the current state
  // system for managing texture units is nessesary
  // TODO: SYSTEM FOR MANAGING TEXTURE UNITS:
  glActiveTexture(GL_TEXTURE7);
  bind(contextId);

  glTexSubImage2D(_target,
                  0,
                  0,
                  0,
                  _width,
                  _height,
                  _pixelFormat,
                  _pixelType,
                  _pixelData);

  unbind(contextId);





}

///////////////////////////////////////////////////////////////////////////////



  /// get the filename of this texture

std::string
Texture::getFileName() const
{
   return _fileName;
}


////////////////////////////////////////////////////////////////////////////////


/* extern */
std::ostream&
operator<< (std::ostream& os, const Texture& texture)
{

  os << std::endl << "Texture" << std::endl << "{";
  os << std::endl << "    SharedResourceId: " << texture.getWidth();
  os << std::endl << "    width:            " << texture.getWidth();
  os << std::endl << "    height:           " << texture.getHeight();
  os << std::endl;
  os << std::endl << "    referenced:       " << texture.isReferenced();
  os << std::endl << "    refcount:         " << texture.getReferenceCount();
  os << std::endl;

  os << std::endl << "} // Texture" << std::endl;
  std::flush(os);

  return os;
}



} // namespace gloost
