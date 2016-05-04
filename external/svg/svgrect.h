#ifndef SVGRECT_H
#define SVGRECT_H


#include <xmlelement.h>

#include <svgelementstyle.h>

namespace minixml{

class svgrect : public xmlelement{

 public:
  svgrect(float width, float height, float x, float y, const svgelementstyle& style);
  /*virtual*/ ~svgrect();


};

}

#endif
