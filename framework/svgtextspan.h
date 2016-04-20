#ifndef SVGTEXTSPAN_H
#define SVGTEXTSPAN_H


#include <xmlelement.h>

#include <svgelementstyle.h>

namespace minixml{

class svgtextspan : public xmlelement{

 public:
  svgtextspan(const char* text, float x, float y, const svgelementstyle& style );
  /*virtual*/ ~svgtextspan();

};

}

#endif
