
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
#include <TextureManager.h>
/// cpp includes
#include <iostream>
#include <vector>
namespace gloost
{
/**
 \class TextureManager
 \brief Texture container and factory. Use this class to create, manage and share your Texture
         resources within your application.
 \author Felix Weiszig
 \date   December 2009
 \remarks Consult the step_04_Textures_and_TextureManager
           tutorial located in <i><gloost>/tutorials/essentials/</i> to learn more.
           <br>This class is a singleton. To get a reference to the instance write:
           \code
           gloost::TextureManager* texManager = gloost::TextureManager::getInstance();
           \endcode
           To load a texture write:
           \code
           // you can do it like this
           unsigned int texId = texManager->createTexture("Image.png");
           // or like this
           unsigned int texId = gloost::TextureManager::getInstance()->createTexture("Image.png");
           \endcode
           To get the Texture instance for the id try:
           \code
           // This will increment the reference counter
           gloost::Texture* tex = texManager->getTextureReference(texId);
           // This will NOT increment the reference counter
           gloost::Texture* tex = texManager->getTextureWithoutRefcount(texId);
           \endcode
           <br>gloost::Texture is using SDL_image to load the images. In the near Future SDL_image
           will be replaced by the free image lib.
*/



/*static*/  TextureManager* TextureManager::_theinstance = 0;



///////////////////////////////////////////////////////////////////////////////


/**
 \brief Class constructor
*/

TextureManager::TextureManager():
    _idCounter(0),
    _textures(),
    _shareMap(),
    _assignMap()
{
   // create a default texture at id 0
   createTexture(512, 512);
}

///////////////////////////////////////////////////////////////////////////////


  /// class destructor

TextureManager::~TextureManager()
{
  getTextureWithoutRefcount(0)->dropReference();
  cleanUp();
}


///////////////////////////////////////////////////////////////////////////////


  /// get an instance to the TextureManager

TextureManager*
TextureManager::getInstance()
{
  if(!_theinstance)
  {
    _theinstance = new TextureManager();
  }
  return _theinstance;
}


//////////////////////////////////////////////////////////////////////////////////////////


  /// Creates empty Texture with given width and height and returns a unique id (refCount +)

unsigned int
TextureManager::createTexture(unsigned int width, unsigned int height)
{
  // generate a new texture
  _textures[_idCounter] = new Texture(width, height);
  // increment the refCounter +1
  _textures[_idCounter]->takeReference();
  ++_idCounter;
  return _idCounter-1;
}


//////////////////////////////////////////////////////////////////////////////////////////


		/**
		  Creates empty Texture with specific id object of given width and height
		  This will overwrite existing textures with this id, so don't mess around with this
		  (refCount +)
		**/


unsigned int
TextureManager::createTexture(unsigned int id, unsigned int width, unsigned int height)
{
  // generate a new texture
  _textures[id] = new Texture(width, height);
  // increment the refCounter +1
  _textures[id]->takeReference();
  return id;
}


//////////////////////////////////////////////////////////////////////////////////////////


  /// load a texture (automatic configuration of the GL_Texture_2Ds format and type) (tested: jpg, png24, png32)

unsigned int
TextureManager::createTexture(std::string file_name, bool share)
{
  // if this texture is shared we first look in the map if this texture
  // has been allready loaded
  if (share)
  {
      std::map<std::string, unsigned int>::iterator pos = _shareMap.find(file_name);
      if (pos != _shareMap.end())
      {
#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_MESSAGES
        std::cout<< std::endl;
        std::cout << std::endl << "Message from TextureManager::createTexture:" ;
        std::cout<< std::endl << "          Image with filename \"" << file_name << "\" is shared";
        std::cout<< std::endl << "          ID: " << (*pos).second;
        std::flush(std::cout);
#endif
        /// increment the reference counter
        getTextureReference((*pos).second);
        return (*pos).second;
      }
  }
  // we take us a new unique id...
  unsigned int newId = _idCounter;
  // ... and increment the counter
  ++_idCounter;
  // generate a new texture
  _textures[newId] = new Texture(file_name);
  // increment the refCounter +1
  _textures[newId]->takeReference();
  // add the texture to the share lookup map
  if (share)
  {
    _textures[newId]->setShared(true);
    _shareMap[file_name] = newId;
  }
  return newId;
}


//////////////////////////////////////////////////////////////////////////////////////////


  /// Add existing Texture to be managed by the TextureManager (refCount +)

unsigned int
TextureManager::addTexture(Texture* texture)
{
  _textures[_idCounter] = texture;
  _textures[_idCounter]->takeReference();
  ++_idCounter;
  return _idCounter-1;
}


//////////////////////////////////////////////////////////////////////////////////////////


  /// returns reference to gloost::Texture object and increments the reference counter (refCount +)

Texture*
TextureManager::getTextureReference(unsigned int id)
{
  std::map<unsigned int, Texture*>::iterator pos = _textures.find(id);
  if (pos != _textures.end())
  {
    // (refCount +)
    (*pos).second->takeReference();
    return (*pos).second;
  }
#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_ERRORS
  std::cout<< std::endl;
  std::cout<< std::endl << "ERROR in TextureManager::getTextureReference():";
  std::cout<< std::endl << "         Could not find existing texture with id = " << id << " !";
  std::cout<< std::endl << "         Will create a BLACK default Texture with size " << 512 << "x" << 512;
  std::cout<< std::endl << "         and the same id you asked for. So your applcation wont crash.";
  std::cout<< std::endl << "         FIX YOUR CODE NOW !!!";
  std::flush(std::cout);
#endif
  /*
    This is buggy! If the id is bigger than _idCounter...
  */
  /// create a help texture
  createTexture(id, 512, 512);
  //_textures[id]->fill(1.0, 0.0, 1.0);
  // (refCount +)
  _textures[id]->takeReference();
  return _textures[id];
}


//////////////////////////////////////////////////////////////////////////////////////////


  /// returns reference to gloost::Texture object WITHOUT increments the reference counter

Texture*
TextureManager::getTextureWithoutRefcount(unsigned int id)
{
  std::map<unsigned int, Texture*>::iterator pos = _textures.find(id);
  if (pos != _textures.end())
  {
    return (*pos).second;
  }
#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_ERRORS
  std::cout<< std::endl;
  std::cout<< std::endl << "ERROR in TextureManager::getTextureReference():";
  std::cout<< std::endl << "         Could not find existing texture with id = " << id << " !";
  std::cout<< std::endl << "         You will get the TextureManagers default Texture";
  std::cout<< std::endl << "         so your applcation won't crash.";
  std::cout<< std::endl << "         FIX YOUR CODE NOW !!!";
  std::flush(std::cout);
#endif
  /// create a help texture
  //  createTexture(id, 512, 512);
  // (refCount +)
  _textures[0]->takeReference();
  return _textures[0];
}


//////////////////////////////////////////////////////////////////////////////////////////


  /// drops reference to gloost::Texture object

bool
TextureManager::dropReference(unsigned int id)
{
  std::map<unsigned int, Texture*>::iterator pos = _textures.find(id);
  if (pos != _textures.end())
  {
    _textures[id]->dropReference();
    return 1;
  }
#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_WARNINGS
  std::cout<< std::endl;
  std::cout<< std::endl << "Warning from TextureManager::dropReference():";
  std::cout<< std::endl << "             Could not find a texture with id = " << id << "!";
  std::cout<< std::endl;
  std::flush(std::cout);
#endif
  return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////


  /// unloads all unused textures

void
TextureManager::cleanUp()
{
#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_MESSAGES
  std::cout<< std::endl;
  std::cout<< std::endl << "Message from TextureManager::cleanup():";
  std::cout<< std::endl << "             Removing all Textures with refCount == 0 from gfx-card";
  std::cout<< std::endl << "             and main memory.";
  std::cout<< std::endl;
  std::flush(std::cout);
#endif
  std::map<unsigned int, Texture*>::iterator index = _textures.begin();
  std::map<unsigned int, Texture*>::iterator end   = _textures.end();
  // we don't delete the default texture
  ++index;
  // vector to memorise the ids of the textures we delete
  std::vector<unsigned int> _delIds;
  for ( ; index != end; )
  {
    if ( !(*index).second->isReferenced() && (*index).first != 0)
    {
      std::map<unsigned int, Texture*>::iterator delPos = index;
      ++index;
      // if this texture was shared, we have to remove it from the shareMap
      if ((*delPos).second->isShared())
      {
        std::map<std::string, unsigned int>::iterator shared_index = _shareMap.find((*delPos).second->getFileName());
        std::map<std::string, unsigned int>::iterator shared_end   = _shareMap.end();
        if (shared_index != shared_end)
        {
          _shareMap.erase(shared_index);
        }
      }
      _delIds.push_back((*delPos).first);
      /// delete texture object within the TextureManager
      delete (*delPos).second;
      /// remove pointer from TextureManager
      _textures.erase(delPos);
    }
    else
    {
      ++index;
    }
  }
#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_MESSAGES
  std::cout<< std::endl;
  std::cout<< std::endl << "Message from TextureManager::cleanUp():";
  std::cout<< std::endl << "             Cleanup done... ";
  std::cout<< std::endl << "             Following (" << _delIds.size() << ") Textures are gone:";
  for(unsigned int i=0; i!=_delIds.size(); ++i)
  {
    std::cout<< std::endl << "                          -> " << _delIds[i];
  }
  std::cout<< std::endl;
  std::cout<< std::endl << "             Textures left:  " << _textures.size();
  std::cout<< std::endl << "             Shared entries: " << _shareMap.size();
  std::cout<< std::endl;
  std::flush(std::cout);
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////


  /// reloads all loaded textures (maybe for test uses)

void
TextureManager::reloadLoadedTextures()
{
  std::cout<< std::endl;
  std::cout<< std::endl << "Warning from TextureManager::reloadLoadedTextures()";
  std::cout<< std::endl << "          reloadLoadedTextures() is not implemented !";
  std::cout<< std::endl << "          Why not??? I'm lazy like hell, thats why!";
  std::cout<< std::endl;
  std::flush(std::cout);
}


//////////////////////////////////////////////////////////////////////////////////////////


  /// returns number of textures within the manager

unsigned int
TextureManager::getSize() const
{
  return _textures.size();
}


//////////////////////////////////////////////////////////////////////////////////////////


  /// prints infos for all textures within the manager

void
TextureManager::printTextureInfo() const
{
  std::map<unsigned int, Texture*>::const_iterator index = _textures.begin();
  std::map<unsigned int, Texture*>::const_iterator end   = _textures.end();
  for ( ; index != end; ++index)
  {
    std::cout << std::endl << (*index).first << " :";
    std::cout << (*(*index).second);
  }
}


//////////////////////////////////////////////////////////////////////////////////////////


/* extern */
std::ostream&
operator<< (std::ostream& os, const TextureManager& tm)
{
  os << std::endl << "TextureManager" << std::endl << "{";
  os << std::endl << "    size:    " << tm.getSize();
  os << std::endl;
  tm.printTextureInfo();
  os << std::endl;
  os << std::endl << "} // TextureManager" << std::endl;
  std::flush(os);
  return os;
}


///////////////////////////////////////////////////////////////////////////////


} // namespace gloost
