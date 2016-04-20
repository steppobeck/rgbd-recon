#ifndef SVGPOLYLINE_H
#define SVGPOLYLINE_H


#include <xmlelement.h>
#include <svgelementstyle.h>

#include <vector>

namespace minixml{

class svgpolyline : public xmlelement{

 public:
	svgpolyline(std::vector<float> points, const svgelementstyle& style);
  /*virtual*/ ~svgpolyline();


};

}

#endif
