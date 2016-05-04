#include "svgline.h"

#include <xmlattribute.h>

namespace minixml{
	
	svgline::svgline(float x1, float y1, float x2, float y2, const svgelementstyle& style):
	xmlelement("line")
	{
		addAttribute(new xmlattribute("id",xmlelement::getUniqueID("line").c_str()));
		addAttribute(new xmlattribute(style.getName(),style.getValueString()));
		
		addAttribute(new xmlattribute("x1",x1));
		addAttribute(new xmlattribute("y1",y1));
		addAttribute(new xmlattribute("x2",x2));
		addAttribute(new xmlattribute("y2",y2));
		
	}
	
	
	/*virtual*/
	svgline::~svgline()
	{}
	
}

