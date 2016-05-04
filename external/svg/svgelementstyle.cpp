#include "svgelementstyle.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

namespace minixml{
	
	svgelementstyle::svgelementstyle():
	xmlattribute("style",""),
	_atts()
	{
		init();
		update();
	}
	
	
	svgelementstyle::svgelementstyle(unsigned char r, unsigned char g, unsigned char b, unsigned char a):
	xmlattribute("style",""),
	_atts()
	{
		init();
		setFill(r,g,b);
		setFillOpacity( ((float) a)/255.0 );
	}
	
	/*virtual*/
	svgelementstyle::~svgelementstyle()
	{}
	
	void
	svgelementstyle::init()
	{
		
		_atts["opacity"]= "1.0";
		_atts["fill"] = "#000000";
		_atts["fill-opacity"] = "1.0";
		_atts["stroke"] = "none";
	}
	
	void
	svgelementstyle::update()
	{
		std::string v;
		std::map<std::string, std::string>::const_iterator i = _atts.begin();
		for(; i != _atts.end(); ++i)
		{
			v += i->first + ":" + i->second + ";";
		}
		
		setValue(v.c_str());
	}
	
	/*static*/ std::string
	svgelementstyle::rgb2hex(unsigned char r, unsigned char g, unsigned char b)
	{
		std::string hexcolor("#");
		char buffer[4];
		sprintf(buffer,"%x",r);
		if(strlen(buffer) < 2)
			hexcolor += "0";
		hexcolor += buffer;
		sprintf(buffer,"%x",g);
		if(strlen(buffer) < 2)
			hexcolor += "0";
		hexcolor += buffer;
		
		sprintf(buffer,"%x",b);
		if(strlen(buffer) < 2)
			hexcolor += "0";
		hexcolor += buffer;
		return hexcolor;
	}
	
	
	/*static*/ float
	svgelementstyle::clamp(float f)
	{
		if(f > 1.0)
			return 1.0;
		if(f < 0.0)
			return 0.0;
		return f;
	}
	
	
	void
	svgelementstyle::setFill(const char* hexcolor)
	{
		_atts["fill"] = hexcolor;
		update();
	}
	
	
	void
	svgelementstyle::setFill(unsigned char r, unsigned char g, unsigned char b)
	{
		_atts["fill"] = rgb2hex(r,g,b);
		update();
	}
	
	void
	svgelementstyle::setFillOpacity(float o)
	{
		_atts["fill-opacity"] = float2str(clamp(o));
		update();
	}
	
	
	void
	svgelementstyle::setOpacity(float o)
	{
		_atts["opacity"] = float2str(clamp(o));
		update();
	}
	
	
	void svgelementstyle::setStroke(const char* a)
	{
		_atts["stroke"] = a;
		update();
	}
	
	
	
	void
	svgelementstyle::setStrokeFill(const char* hexcolor)
	{
		_atts["stroke-fill"] = hexcolor;
		update();
	}
	
	
	void
	svgelementstyle::setStrokeFill(unsigned char r, unsigned char g, unsigned char b)
	{
		_atts["stroke-fill"] = rgb2hex(r,g,b);
		update();
	}
	
	
	void svgelementstyle::setStrokeWidth(float a)
	{
		_atts["stroke-width"] = float2str(fabs(a));
		update();
	}
	
	
	void
	svgelementstyle::setStrokeMiterlimit(float a)
	{
		_atts["stroke-miterlimit"] = float2str(fabs(a));
		update();
	}
	
	
	void
	svgelementstyle::setStrokeDasharray(const char* a)
	{
		_atts["stroke-dasharray"] = a;
		update();
	}
	
	
	void
	svgelementstyle::setStrokeDashoffset(float a)
	{
		_atts["stroke-dashoffset"] = float2str(fabs(a));
		update();
	}
	
	
	void
	svgelementstyle::setStrokeOpacity(float a)
	{
		_atts["stroke-opacity"] = float2str(clamp(a));
		update();
	}
	
	
	
	void
	svgelementstyle::setStrokeLineCap(const char* a)
	{
		_atts["stroke-linecap"] = a;
		update();
	}
	
	void
	svgelementstyle::setStrokeLineJoin(const char* a)
	{
		_atts["stroke-linejoin"] = a;
		update();
	}
	
	void
	svgelementstyle::setFontSizePX(unsigned int a)
	{
		_atts["font-size"] = xmlattribute::int2str(a) + "px";
		update();
	}
	
	void
	svgelementstyle::setFontStyle(const char* a)
	{
		_atts["font-style"] = a;
		update();
	}
	
	void
	svgelementstyle::setFontWeight(const char* a)
	{
		_atts["font-weight"] = a;
		update();
	}
	
	
	void
	svgelementstyle::setFontFamily(const char* a)
	{
		_atts["font-family"] = a;
		update();
	}
	
	
	void
	svgelementstyle::setTextAnchor(const char* a)
	{
		_atts["text-anchor"] = a;
		update();
	}
	
	
}
