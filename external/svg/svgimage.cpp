#include "svgimage.h"

#include <xmlattribute.h>

#include <string>

namespace minixml{

svgimage::svgimage(float width, float height, float x, float y, const char* filename):
  xmlelement("image")
{
  addAttribute(new xmlattribute("id",xmlelement::getUniqueID("image").c_str()));
  addAttribute(new xmlattribute("width",width));
  addAttribute(new xmlattribute("height",height));
  addAttribute(new xmlattribute("x",x));
  addAttribute(new xmlattribute("y",y));
  addAttribute(new xmlattribute("xlink:href",filename));

}


/*virtual*/
svgimage::~svgimage()
{}

}

