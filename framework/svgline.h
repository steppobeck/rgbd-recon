#ifndef SVGLINE_H
#define SVGLINE_H


#include <xmlelement.h>
#include <svgelementstyle.h>

namespace minixml{
	
	class svgline : public xmlelement{
		
	public:
		svgline(float x1, float y1, float x2, float y2, const svgelementstyle& style);
		/*virtual*/ ~svgline();
		
		
	};
	
}

#endif
