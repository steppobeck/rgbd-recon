
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
#include <MultiGlContext.h>
#include <gloostConfig.h>
//#include <MessageLog.h>



/// cpp includes
#include <string>
#include <iostream>



namespace gloost
{

/**
  \class MultiGlContext
  \brief Base class of all GL Resources like Shader or Texture which can live
  in up to 32 GL contexts. The methodes initInContext() and removeFromContext()
  must be implemented in your inherited classes

  \author Felix Weiszig
  \date   December 2009

  \remarks You have to specify the number of contexts your gloost lib can handle
  by setting the GLOOST_SYSTEM_NUM_RENDER_CONTEXTS constant within the gloostConfig.h.
  If you only use one context (wich is the normal case) you can set this value to 1
  to gain some speed.
*/


////////////////////////////////////////////////////////////////////////////////


/**

  \brief Class constructor
  \param numResourcesPerContext Number of resources per context your class shall handle.
*/

MultiGlContext::MultiGlContext(unsigned int numResourcesPerContext):
  SharedResource(),
  _handlesVectorForContexts(GLOOST_SYSTEM_NUM_RENDER_CONTEXTS, std::vector<unsigned int>(numResourcesPerContext, 0)),
  _numResourcesPerContext(numResourcesPerContext)
{
  // fill all resource handle of all contexts with 0
//  for (unsigned int i=0; i!=GLOOST_SYSTEM_NUM_RENDER_CONTEXTS; ++i)
//  {
//    for (unsigned int j=0; j!=numResourcesPerContext; ++j)
//    {
//      _handlesVectorForContexts[i][j] = 0;
//    }
//  }

}


////////////////////////////////////////////////////////////////////////////////


/**
  \brief Class destructor
*/

MultiGlContext::~MultiGlContext()
{
	// insert your code here
}


////////////////////////////////////////////////////////////////////////////////


/**
  \brief Sets a resource handle for a resource within a context
  \param handle The handle (normaly the OpenGL handle) of the resource
  \param contextId The id of the context this handle shall be associated with,
         0 or none if you have only one context
  \param resourceId The id of the resource, 0 or none if you have only one resource per context
*/


void
MultiGlContext::setResourceHandleForContext( unsigned int handle,
                                             unsigned int contextId,
                                             unsigned int resourceId)
{
  _handlesVectorForContexts[contextId][resourceId] = handle;
}


////////////////////////////////////////////////////////////////////////////////


/**
  \brief Returns the handle for a specified resource of a context
  \param contextId The id of the context the resource was generated for,
        0 or none if you have only one context
  \param resourceId The id of the resource, 0 or none if you have only
         one resource per context
*/

unsigned int&
MultiGlContext::getResourceHandleForContext(unsigned int contextId,
                                            unsigned int resourceId)
{
  return _handlesVectorForContexts[contextId][resourceId];
}


////////////////////////////////////////////////////////////////////////////////


/**

  \brief Returns the handle for a specified resource of a context
  \param contextId The id of the context the resource was generated for,
        0 or none if you have only one context
  \param resourceId The id of the resource, 0 or none if you have only
         one resource per context

*/


const unsigned int&
MultiGlContext::getResourceHandleForContext(unsigned int contextId,
                                            unsigned int resourceId) const
{
  return _handlesVectorForContexts[contextId][resourceId];
}


////////////////////////////////////////////////////////////////////////////////


/**
  \brief Returns the number of contexts this object lives in. This counts only
         contexts your resource was allready generated for.

  \remarks 1. This method is not very fast due to two nested loops.
  \remarks 2. The maximal number of contexts supported by your glost lib is
           defined in the GLOOST_SYSTEM_NUM_RENDER_CONTEXTS constant within the
           gloostConfig.h.
*/

unsigned int
MultiGlContext::getNumContexts() const
{
  unsigned int count = 0;

  for (unsigned int i=0; i!=_handlesVectorForContexts.size(); ++i)
  {
    for (unsigned int j=0; j!=_numResourcesPerContext; ++j)
    {
      if (_handlesVectorForContexts[i][j])
      {
        ++count;
        break;
      }
    }
  }

  return count;
}


////////////////////////////////////////////////////////////////////////////////


/**
  \brief Returns the number of resources per context this instance can have.
*/

unsigned int
MultiGlContext::getNumResourcesPerContext() const
{
  return _numResourcesPerContext;
}


////////////////////////////////////////////////////////////////////////////////


/**
  \brief Returns true if a handle exists within the context contextId
  \param contextId The id of the context, 0 or none if you have only one context
  \param resourceId The id of the resource, 0 or none if you have only
         one resource per context
*/


bool
MultiGlContext::handleInContextExists(unsigned int contextId, unsigned int resourceId) const
{
  return _handlesVectorForContexts[contextId][resourceId];
}


////////////////////////////////////////////////////////////////////////////////





} // namespace gloost


