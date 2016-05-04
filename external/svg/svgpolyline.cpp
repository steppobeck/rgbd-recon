#include <svgpolyline.h>
#include <xmlattribute.h>

#include <iostream>
#include <sstream>

namespace minixml{
	
	svgpolyline::svgpolyline(std::vector<float> points, const svgelementstyle& style):
	xmlelement("polyline")
	{
		addAttribute(new xmlattribute("id",xmlelement::getUniqueID("polyline").c_str()));
		addAttribute(new xmlattribute(style.getName(),style.getValueString()));
		
		std::ostringstream p ;
		
		for (unsigned i = 0; i != points.size(); ++i) {
			p << points[i] << ",";
			p << points[++i] << " ";
		}
		addAttribute(new xmlattribute("points",p.str().c_str()));
	}
	
	
	/*virtual*/
	svgpolyline::~svgpolyline()
	{}
	
}

