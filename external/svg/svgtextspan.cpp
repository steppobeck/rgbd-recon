#include "svgtextspan.h" 

#include <xmlattribute.h>

namespace minixml{

svgtextspan::svgtextspan(const char* text, float x, float y, const svgelementstyle& style ):
  xmlelement("text")
{

  addAttribute(new xmlattribute("xml:space","preserve"));
  addAttribute(new xmlattribute(style.getName(),style.getValueString()));

  addAttribute(new xmlattribute("x",x));
  addAttribute(new xmlattribute("y",y));

  addAttribute(new xmlattribute("id",xmlelement::getUniqueID("text").c_str()));


  xmlelement* tspan = new xmlelement("tspan");

  tspan->addAttribute(new xmlattribute("sodipodi:role","line"));
  tspan->addAttribute(new xmlattribute("id",xmlelement::getUniqueID("tspan").c_str()));

  tspan->addAttribute(new xmlattribute("x",x));
  tspan->addAttribute(new xmlattribute("y",y));

  tspan->setContent(text);

  this->addChild(tspan);
}


/*virtual*/
svgtextspan::~svgtextspan()
{}

}
