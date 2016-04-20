#include "svgdocument.h"

#include <xmlelement.h>
#include <xmlattribute.h>
#include <svgelementstyle.h>

#include <fstream>

namespace minixml{

svgdocument::svgdocument():
  xmldocument()
{
  createEmpty();
}

svgdocument::svgdocument(unsigned int width, unsigned int height):
  xmldocument()
{
  createEmpty();
  setWidth(width);
  setHeight(height);
}

svgdocument::svgdocument(const char* filename):
  xmldocument(filename)
{}


svgdocument::~svgdocument()
{}


void
svgdocument::createEmpty()
{
  _documentroot = new xmlelement("svg");
  _elements.push_back(_documentroot);


  _documentroot->addAttribute(new xmlattribute("xmlns:dc","http://purl.org/dc/elements/1.1/"));
  _documentroot->addAttribute(new xmlattribute("xmlns:cc","http://web.resource.org/cc/"));
  _documentroot->addAttribute(new xmlattribute("xmlns:rdf","http://www.w3.org/1999/02/22-rdf-syntax-ns#"));
  _documentroot->addAttribute(new xmlattribute("xmlns:svg","http://www.w3.org/2000/svg"));
  _documentroot->addAttribute(new xmlattribute("xmlns","http://www.w3.org/2000/svg"));
  _documentroot->addAttribute(new xmlattribute("xmlns:xlink","http://www.w3.org/1999/xlink"));
  _documentroot->addAttribute(new xmlattribute("xmlns:sodipodi","http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd"));
  _documentroot->addAttribute(new xmlattribute("xmlns:inkscape","http://www.inkscape.org/namespaces/inkscape"));
  _documentroot->addAttribute(new xmlattribute("width","640"));
  _documentroot->addAttribute(new xmlattribute("height","480"));
  _documentroot->addAttribute(new xmlattribute("id","svg2"));
  _documentroot->addAttribute(new xmlattribute("sodipodi:version","0.32"));
  _documentroot->addAttribute(new xmlattribute("inkscape:version","0.44.1"));
  _documentroot->addAttribute(new xmlattribute("version","1.0"));
  _documentroot->addAttribute(new xmlattribute("sodipodi:docbase",""));
  _documentroot->addAttribute(new xmlattribute("sodipodi:docname",""));
  _documentroot->addAttribute(new xmlattribute("inkscape:export-filename",""));
  _documentroot->addAttribute(new xmlattribute("inkscape:export-xdpi",""));
  _documentroot->addAttribute(new xmlattribute("inkscape:export-ydpi",""));

  /*
   xmlns:dc="http://purl.org/dc/elements/1.1/"
   xmlns:cc="http://web.resource.org/cc/"
   xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
   xmlns:svg="http://www.w3.org/2000/svg"
   xmlns="http://www.w3.org/2000/svg"
   xmlns:sodipodi="http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd"
   xmlns:inkscape="http://www.inkscape.org/namespaces/inkscape"
   width="640"
   height="480"
   id="svg2"
   sodipodi:version="0.32"
   inkscape:version="0.44.1"
   version="1.0"
   sodipodi:docbase=""
   sodipodi:docname=""
   inkscape:export-filename=""
   inkscape:export-xdpi="90"
   inkscape:export-ydpi="90"

  */

  xmlelement* sodipodinamedview = new xmlelement("sodipodi:namedview");
  _documentroot->addChild(sodipodinamedview);

  sodipodinamedview->addAttribute(new xmlattribute("inkscape:pageopacity","0.0"));
  sodipodinamedview->addAttribute(new xmlattribute("pagecolor","#FFFFFF"));
  sodipodinamedview->addAttribute(new xmlattribute("borderopacity","1.0"));
  sodipodinamedview->addAttribute(new xmlattribute("bordercolor","#666666"));

}


void
svgdocument::setWidth(unsigned int a)
{
  _documentroot->getAttributeByName("width")->setValue((int) a);
}

void
svgdocument::setHeight(unsigned int a)
{
  _documentroot->getAttributeByName("height")->setValue((int) a);
}


void
svgdocument::setPageColor(const char* hexcolor)
{
  _documentroot->getChildrenByName("sodipodi:namedview")[0]->getAttributeByName("pagecolor")->setValue(hexcolor);
}


void
svgdocument::setPageColor(unsigned char r, unsigned char g, unsigned char b)
{
  _documentroot->getChildrenByName("sodipodi:namedview")[0]->getAttributeByName("pagecolor")->setValue(svgelementstyle::rgb2hex(r,g,b).c_str());
}


void
svgdocument::setPageOpacity(float a)
{
  _documentroot->getChildrenByName("sodipodi:namedview")[0]->getAttributeByName("inkscape:pageopacity")->setValue(svgelementstyle::clamp(a));
}



void
svgdocument::setBorderColor(const char* hexcolor)
{
  _documentroot->getChildrenByName("sodipodi:namedview")[0]->getAttributeByName("bordercolor")->setValue(hexcolor);
}


void
svgdocument::setBorderColor(unsigned char r, unsigned char g, unsigned char b)
{
  _documentroot->getChildrenByName("sodipodi:namedview")[0]->getAttributeByName("bordercolor")->setValue(svgelementstyle::rgb2hex(r,g,b).c_str());
}


void
svgdocument::setBorderOpacity(float a)
{
  _documentroot->getChildrenByName("sodipodi:namedview")[0]->getAttributeByName("borderopacity")->setValue(svgelementstyle::clamp(a));
}



void
svgdocument::write(const char* filename)
{
  std::ofstream f;
  f.open(filename);

  f << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
    << std::endl
    << "<!-- Created with svgdocuemt (http://www.pixelstars.de) -->"
    << std::endl
    << "<!-- ...hopefully works with inkscape (http://www.inkscape.org) -->"
    << std::endl;


  _documentroot->getAttributeByName("sodipodi:docname")->setValue(filename);

  f << _documentroot->tostring(true);

  f.close();
}
}
