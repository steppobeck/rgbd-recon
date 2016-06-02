
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



#ifndef GLOOST_TEXTURE_H
#define GLOOST_TEXTURE_H



/// gloost system includes
#include <MultiGlContext.h>
#include <Matrix.h>
#include <UniformTypes.h>
#include <BitMask.h>



/// cpp includes
#include <string>
#include <map>
#include <glbinding/gl/gl.h>
using namespace gl;






namespace gloost
{


	/// Wrapper for an OpenGL texture based on a SDL_Surface

class Texture : public MultiGlContext
{

	public:


    /// takes a filename and reads 8, 24 and 32bit images automaticly
    Texture(const std::string& fileName);


	  /// class constructor
		Texture( unsigned int width,
             unsigned int height,
		         GLenum target        = GL_TEXTURE_2D,
		         GLenum internalFormat = GL_RGBA,
		         GLenum pixelFormat   = GL_RGBA,
		         GLenum pixelType     = GL_FLOAT);


	  /// class constructor for your own pixeldata
		Texture( unsigned int width,
             unsigned int height,
             unsigned int depth,
             void*        pixeldata,
		         GLenum target,
		         GLenum  internalFormat,
		         GLenum pixelFormat,
		         GLenum pixelType);



		/// class destructor
		virtual ~Texture();


		/// set a glTexParameter for this texture to be applied when texture is gernerated
		void setTexParameter(GLenum propertyName, float value);

		/// get TexParameter
		float getTexParameter(GLenum propertyName);

		/// remove a TexParameter from this texture to use GL default value
		void removeTexParameter(GLenum propertyName);

		/// get a map with all Texparameter for this texture
		std::map<GLenum, float>& getTexParameterMap();



		/// get width of the texture
		unsigned int getWidth() const;

		/// get height of the texture
		unsigned int getHeight() const;


		/// get the aspect ratio of the texture ( ar = width/height)
		float getAspecRatio() const;



		/// get gl's texture target for this texture (GL_TEXTURE_2D, GL_TEXTURE_3D, ...)
    GLenum getTarget();

    /// get gl's internal format for this texture
    GLenum getInternalFormat();

    /// get gl's pixel format for this texture
    GLenum getPixelFormat();

    /// get gl's pixel type for this texture
    GLenum getPixelType();



    /// get the GL texture handle
		unsigned int getTextureHandle(unsigned int contextId = 0);


   /// ...
   void freeHostData();



		/// writes the surfaces to a jpeg file (may be upside down)
//		void writeToFile(std::string);


		/// returns the  program side color value of the pixel with index i
		vec4 getColor(unsigned int index);

		/// returns the  program side color value of the pixel position x,y
		vec4 getColor (int x , int y );



		/// set the dirty flag to signal texture changes
		void setDirty();

		/// unset the dirty flag to signal that texture is up to date
		void setClean(unsigned int contextId = 0);

		/// check if the texture was changed to any time
		bool isDirty(unsigned int contextId = 0) const;



    /// generates a Gl texture from this texture
    /*virtual*/ bool initInContext(unsigned int contextId = 0);

	  /// remove the texture from a context
	  /*virtual*/ void removeFromContext(unsigned int contextId = 0);


    /// remove the texture from the gpu ram
    void ungenerate(unsigned int contextId);



		/// bind Texture to GL state
		void bind(GLenum texId=GL_TEXTURE0, unsigned int contextId = 0);

		///unbind Texture
		void unbind(unsigned int contextId = 0);



		/// reads the (changed) texture from the gfx-card back into the surface
		void readBack();

		/// copys the current framebuffer into the surface
		void copyFramebuffer();



    /// set the texture matrix
    void setTextureMatrix(const Matrix& matrix);

    /// get the texture matrix
    const Matrix& getTextureMatrix();

    /// use the textureMatrix
    void enableTextureMatrix(bool trueOrFalse = true);

    /// returns 1 if texture matrix is enabled
    bool isTextureMatrixEnabled();


    /// use Mipmapping (don't work if attachet to an Fbo)
    void enableMipmaps(bool trueOrFalse = true);

    /// returns true if Mipmapping is enabled
    bool isMipmapsEnabled();



    /// set this texture shared or unshared (this flag is used by the TextureManager)
    void setShared(bool shared);

    /// returns 1 if this texture is shared within the TextureManager
    bool isShared();



    /// get the pixels of this texture
    void* getPixels();

    /// replaces pixeldata on host and server side, new pixels must have same format as old ones! (Texture will not become _dirty)
    void updatePixels(unsigned char* pixelbuffer,
                      unsigned int lengthInByte,
                      unsigned int contextId = 0);



    /// get the filename of this texture
    std::string getFileName() const;


	protected:


	  /// texture target (GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB, ...)
	  GLenum _target;


	  /// internal format of the texture (GL_RGB, GL_RGBA, GL_ALPHA, GL_DEPTH_COMPONENT32, ...)
	  GLenum _internalFormat;


		/// width of the texture
		unsigned int _width;

		/// width of the texture
		unsigned int _height;

		/// depth of the (3D)texture
		unsigned int _depth;


	  /// format of the pixeldata (GL_COLOR_INDEX, GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA, GL_RGB, GL_RGBA, GL_LUMINANCE, and GL_LUMINANCE_ALPHA)
	  GLenum _pixelFormat;


	  /// Specifies the	type of the pixel data (GL_UNSIGNED_BYTE, GL_BYTE, GL_BITMAP, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, and GL_FLOAT)
		GLenum _pixelType;



    /// determines if the texture is up to date
    gloost::BitMask _dirty;


		/// texture Unit Id, given with the bind(id) methode, used for unbind
		std::vector<GLenum> _textureUnitIdsForContexts;


		/// determines if this texture is using mipmapping
		bool _mipmapsEnabled;


		/// SDL SURFACE
		//SDL_Surface* _surface;
        

		/// pixeldata if exiting
		void* _pixelData;


    /// map stores all texparameters for this textures to be applied when texture is generated
    std::map<GLenum, float> _glTexParameters;


    /// texture matrix
    Matrix _texMatrix;

    /// use texture matrix
    bool _useTextureMatrix;

    /// flag is true if this texture is shared
    bool _shared;

    /// filename of this texture if it was loaded
    std::string _fileName;


    /// generates a Gl texture from this texture
    unsigned int generate( unsigned int width,
                           unsigned int height,
                           unsigned int depth     = 0,
                           GLenum target          = GL_TEXTURE_2D,
                           GLenum internalFormat   = GL_RGBA,
                           GLenum pixelFormat     = GL_RGBA,
                           GLenum pixelType       = GL_FLOAT,
                           unsigned int contextId = 0);

};


  // ostream operator
  extern std::ostream& operator<< (std::ostream&, const Texture& );


}  // namespace gloost


#endif // #ifndef GLOOST_TEXTURE_H
