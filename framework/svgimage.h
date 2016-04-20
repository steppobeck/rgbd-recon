#ifndef SVGIMAGE_H
#define SVGIMAGE_H


#include <xmlelement.h>

#include <svgelementstyle.h>

namespace minixml{

class svgimage : public xmlelement{

 public:
  svgimage(float width, float height, float x, float y, const char* filename);
  /*virtual*/ ~svgimage();


};

}

#endif
