#include "svgrect.h"

#include <xmlattribute.h>

namespace minixml{

svgrect::svgrect(float width, float height, float x, float y, const svgelementstyle& style):
  xmlelement("rect")
{
  addAttribute(new xmlattribute("id",xmlelement::getUniqueID("rect").c_str()));
  addAttribute(new xmlattribute(style.getName(),style.getValueString()));

  addAttribute(new xmlattribute("width",width));
  addAttribute(new xmlattribute("height",height));
  addAttribute(new xmlattribute("x",x));
  addAttribute(new xmlattribute("y",y));

}


/*virtual*/
svgrect::~svgrect()
{}

}

