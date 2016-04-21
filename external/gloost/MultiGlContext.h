
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



#ifndef GLOOST_MULTI_GL_CONTEXT_H
#define GLOOST_MULTI_GL_CONTEXT_H



/// gloost system includes
#include <gloostMath.h>
#include <SharedResource.h>


/// cpp includes
#include <string>
#include <vector>



namespace gloost
{


  // Base class of all GL Resources like Shader or Texture which can live in up to 32 GL contexts

class MultiGlContext : public SharedResource
{
	public:

    // class constructor
    MultiGlContext(unsigned int numResourcesPerContext = GLOOST_SYSTEM_NUM_RENDER_CONTEXTS);

    // class destructor
	  virtual ~MultiGlContext();



	  // initialize the resource within a context
	  virtual bool initInContext (unsigned int contextId = 0) = 0;


	  // remove a resource from a context
	  virtual void removeFromContext(unsigned int contextId = 0) = 0;



    // set a handle for a context and a resource Id
    void setResourceHandleForContext(unsigned int handle,
                                     unsigned int contextId,
                                     unsigned int resourceId = 0);


    // set a handle for a context and a resource Id
    unsigned int& getResourceHandleForContext( unsigned int contextId,
                                               unsigned int resourceId = 0);

    // set a handle for a context and a resource Id
    const unsigned int& getResourceHandleForContext( unsigned int contextId,
                                                     unsigned int resourceId = 0) const;



    // get the number of contexts this object lives in
    unsigned int getNumContexts() const;

    // get the number of resources per context
    unsigned int getNumResourcesPerContext() const;



    // returns true if the handle exists within the context contextId
    bool handleInContextExists(unsigned int contextId, unsigned int resourceId = 0) const;



	protected:

    /// contextId <-> GL Handle, of the resource within this GL Context
    std::vector<std::vector<unsigned int> > _handlesVectorForContexts;

    /// number of resources per context of this instance
    unsigned int _numResourcesPerContext;

};


} // namespace gloost


#endif // GLOOST_MULTI_GL_CONTEXT_H


