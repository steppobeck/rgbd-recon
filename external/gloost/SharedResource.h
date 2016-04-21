
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



#ifndef GLOOST_SHARED_RESOURCE_H
#define GLOOST_SHARED_RESOURCE_H



/// gloost system includes



/// cpp includes
#include <string>



namespace gloost
{


  //  Base class for all shared resources implementing a reference counting mechanism

class SharedResource
{
	public:
    // class constructor
    SharedResource();

    // class destructor
	  virtual ~SharedResource();



	  // increments reference counter
	  virtual void takeReference();

	  // decrements reference counter
	  virtual void dropReference();

	  // sets reference counter to 0
    virtual void dropAllReferences();



		// checks if there is a reference
		virtual bool isReferenced() const;

		// returns Reference counter
		virtual int getReferenceCount() const;


		// returns the unique resource id of this shared resource
		unsigned int getSharedResourceId() const;



	private:

    // counts shared resources to assign a unique id to each of them
    static unsigned int _uniqueResourceIdCounter;

    // unique id of this resource
	  unsigned int _resourceId;

	  // counts the number of references to this resource
	  unsigned int _referenceCounter;

};


} // namespace gloost


#endif // GLOOST_SHARED_RESOURCE_H



