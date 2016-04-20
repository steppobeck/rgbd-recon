
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


#ifndef H_GLOOST_TEXTUREMANAGER
#define H_GLOOST_TEXTUREMANAGER


/// gloost includes
#include <Texture.h>
/// cpp includes
#include <map>
namespace gloost
{


  // Texture factory and container

class TextureManager
{


public:

  ~TextureManager();

  // get an instance to the TextureManager
  static TextureManager* getInstance();

  // copy contructor to avoid assignment to a non reference
  explicit TextureManager(TextureManager const&) {}



  // This methodes will create new textures and increment the reference counter of this texture//
  // Creates empty Texture object of given width and height and returns a unique id (refCount +)
  unsigned int createTexture(unsigned int width, unsigned int height);

  // load a texture (automatic configuration of the GL_Texture_2Ds format and type) (tested: jpg, png24, png32) (refCount +)
  unsigned int createTexture(std::string file_name, bool share = 1);

  // Add existing Texture to be managed by the TextureManager (refCount +)
  unsigned int addTexture(Texture* texture);

  // Add existing Texture to be managed by the TextureManager (refCount +)
  //unsigned int addTextureAndAssignName(Texture* texture, const std::string& uniqueName);
  // assigns a name to the texture so you can reference it with this name instead with the id
  //bool assignName (unsigned int textureId, const std::string& uniqueName);


  // returns reference to an existing Texture object and increments the reference counter (refCount +)
  Texture* getTextureReference(unsigned int id);

  // returns reference to gloost::Texture object WITHOUT incrementing the reference counter
  Texture* getTextureWithoutRefcount(unsigned int id);

  // drops reference to gloost::Texture object (refCount -)
  bool dropReference(unsigned int id);


  // unloads all unused textures
  void cleanUp();

  // reloads all loaded textures (maybe for test uses)
  void reloadLoadedTextures();

  // returns number of textures within the manager
  unsigned int getSize() const;

  // prints infos for all textures within the manager
  void printTextureInfo() const;

private:
  ///
  static TextureManager* _theinstance;
  /// internal ID counter for unique IDs
  unsigned int _idCounter;

  /// map to hold the textures
  std::map<unsigned int, Texture*> _textures;

  /// map to hold filepath->id assoziations to share textures
  std::map<std::string, unsigned int> _shareMap;


  /// map to hold unique name->id assoziations
  std::map<std::string, unsigned int> _assignMap;


  /// class constructor
  TextureManager();
  /**
    Creates empty Texture with specific id object of given width and height
    This will overwrite existing textures with this id, so don't mess around with this.
  **/
  unsigned int createTexture(unsigned int id, unsigned int width, unsigned int height);
};


/// ostream operator

extern std::ostream& operator<< (std::ostream&, const TextureManager& );
}  // namespace gloost


#endif // H_GLOOST_TEXTUREMANAGER
