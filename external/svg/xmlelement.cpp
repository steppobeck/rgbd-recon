#include "xmlelement.h"


#include <xmlattribute.h>
#include <stdio.h>


namespace minixml{

/*static*/ unsigned int xmlelement::_uniqueID = 0;

xmlelement::xmlelement(const char* a)
  :_name(a),
   _attributes(),
   _children(),
   _content(),
   _empty(false)
{}

/*virtual*/
xmlelement::~xmlelement()
{
  for(unsigned int i = 0; i < _attributes.size(); ++i)
	delete _attributes[i];

  for(unsigned int i = 0; i < _children.size(); ++i)
        delete _children[i];

}


const char*
xmlelement::getName() const{
  return _name.c_str();
}

void
xmlelement::setName(const char* a){
  _name = a;
}

void 
xmlelement::setContent(const char* a){
  _content = a;
}

void 
xmlelement::addContent(const char* a){
  std::string c(a);
  _content += c;
}

void xmlelement::addAttribute(xmlattribute* a){
  _attributes.push_back(a);
}

void xmlelement::addChild(xmlelement* a){
  _children.push_back(a);
}

const char*
xmlelement::getContentString() const{
  return _content.c_str();
}


int
xmlelement::getContentInt() const{
  int v;
  sscanf(_content.c_str(),"%d",&v);
  return v;
}


float
xmlelement::getContentFloat() const{
  float v;
  sscanf(_content.c_str(),"%f",&v);
  return v;
}
  

const xmlattribute*
xmlelement::getAttributeByName(const char* a) const{
  std::string n(a);
  
  for(unsigned int i = 0; i < _attributes.size(); ++i)
    if(n == _attributes[i]->getName())
      return _attributes[i];

  return NULL;
}

xmlattribute*
xmlelement::getAttributeByName(const char* a) {
  std::string n(a);
  
  for(unsigned int i = 0; i < _attributes.size(); ++i)
    if(n == _attributes[i]->getName())
      return _attributes[i];

  return NULL;
}


const std::vector<xmlattribute*>&
xmlelement::getAttributes() const{
  return _attributes;
}


std::vector<xmlelement*>
xmlelement::getChildrenByName(const char* a) const{
  std::string n(a);
  std::vector<xmlelement*> vece;
  for(unsigned int i = 0; i < _children.size(); ++i)
    if(n == _children[i]->getName())
      vece.push_back(_children[i]);

  return vece;
}


const std::vector<xmlelement*>
xmlelement::getChildren() const{
  return _children;
}

void
xmlelement::setEmpty(){
  _empty = true;
}

bool
xmlelement::empty() const{
  return _empty;
}


std::string
xmlelement::tostring(bool formatted) const{
  
  std::string thestring("<" + _name);

  for(unsigned int a = 0; a < _attributes.size(); ++a)
    thestring += std::string(" ") + _attributes[a]->getName() + std::string("=\"") + _attributes[a]->getValueString() + std::string("\"");
  

  

  if(_children.size()){
    thestring += std::string(">");
    
    if(formatted)
      thestring += "\n";
    thestring += _content;
    for(unsigned int c = 0; c < _children.size(); ++c){
      thestring += _children[c]->tostring(formatted);
      if(formatted && (c + 1) < _children.size())
	thestring += "\n";
    }
    if(formatted)
      thestring += "\n";
    thestring += std::string("</") + _name + std::string(">");
  }else if(_content == ""){
    thestring += std::string("/>");
  }else{
    thestring += std::string(">");
    thestring += _content;
    thestring += std::string("</") + _name + std::string(">");
  }
  
  
  return thestring;
}



/*static*/ std::string
xmlelement::getUniqueID(const char* prefix)
{
  if(prefix)
    return std::string( std::string(prefix) + xmlattribute::int2str(++_uniqueID));
  return xmlattribute::int2str(++_uniqueID);
}
}

