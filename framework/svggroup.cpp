#include "svggroup.h"
#include <xmlattribute.h>

namespace minixml{

/*static*/ unsigned int svggroup::_groupnumber = 0;




svggroup::svggroup():
  xmlelement("g")
{

  unsigned int gnum = nextGroupNumber();
  addAttribute(new xmlattribute("inkscape:label",std::string(std::string("Layer ") + xmlattribute::int2str(gnum)).c_str()));
  addAttribute(new xmlattribute("inkscape:groupmode","layer"));
  addAttribute(new xmlattribute("id",std::string(std::string("layer") + xmlattribute::int2str(gnum)).c_str()));
}


/*virtual*/
svggroup::~svggroup()
{}


/*static*/
unsigned int
svggroup::nextGroupNumber()
{
  return ++_groupnumber;
}
}
