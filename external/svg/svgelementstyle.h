#ifndef SVGELEMENTSTYLE_H
#define SVGELEMENTSTYLE_H



#include <xmlattribute.h>
#include <map>

namespace minixml{
	
	class svgelementstyle : public xmlattribute{
		
	public:
		svgelementstyle();
		svgelementstyle(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
		/*virtual*/ ~svgelementstyle();
		
		
		/*
		 opacity:0.80346821;fill:#888a85;fill-opacity:0.64571428;stroke:none;stroke-width:50;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1
		 
		 font-size:64px;font-style:normal;font-weight:normal;fill:black;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1;font-family:Bitstream Vera Serif
		 */
		
		void setFill(const char* hexcolor);
		void setFill(unsigned char r, unsigned char g, unsigned char b);
		
		void setFillOpacity(float o);
		void setOpacity(float o);
		
		void setStroke(const char* a);
		void setStrokeFill(const char* hexcolor);
		void setStrokeFill(unsigned char r, unsigned char g, unsigned char b);
		void setStrokeWidth(float a);
		void setStrokeMiterlimit(float a);
		void setStrokeDasharray(const char* a);
		void setStrokeDashoffset(float a);
		void setStrokeOpacity(float a);
		
		
		void setStrokeLineCap(const char* a);
		void setStrokeLineJoin(const char* a);
		
		void setFontSizePX(unsigned int a);
		void setFontStyle(const char* a);
		void setFontWeight(const char* a);
		void setFontFamily(const char* a);
		void setTextAnchor(const char* a);
		
		
		
		
		
		static std::string rgb2hex(unsigned char r, unsigned char g, unsigned char b);
		static float clamp(float f);
		
	private:
		
		void init();
		void update();
		
		std::map<std::string, std::string> _atts;
		
	};
	
}
#endif
