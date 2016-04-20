#include "xmldocument.h"


#include <xmlelement.h>
#include <xmlattribute.h>




#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <stack>
#include <stdio.h>

namespace minixml{

xmldocument::xmldocument():
  _filename("nofile.xml"),
  _documentroot(NULL),
  _elements()
{}

xmldocument::xmldocument(const char* a)
  :_filename(a),
   _documentroot(NULL),
   _elements()
{}


xmldocument::~xmldocument()
{
   delete _documentroot;
}


#define OUTSIDE          0
#define INSIDEELEMENT    1
#define INSIDEATTRIBUTE  2
#define INSIDEVALUE      3

bool
xmldocument::open(){

  struct stat fs;
  if(stat(_filename.c_str(),&fs))
    return false;
  off_t filesize = fs.st_size;
  off_t bytesread = 0;

  FILE* fd = fopen(_filename.c_str(),"r");
  unsigned char token;
  int state = OUTSIDE;
  std::string elementname;
  std::string attributename;
  std::string value;
  xmlelement* el = NULL;

  
  while(bytesread < filesize){
    bytesread += fread(&token, sizeof(char), sizeof(token), fd);
    
    switch(state){
    case OUTSIDE:
      {
	if(token == '<'){
	  bytesread += fread(&token, sizeof(char), sizeof(token), fd);
	  
	  if(token != '!' && token != '?'){
	    state = INSIDEELEMENT;
	    if(token == '/'){
	      bytesread += fread(&token, sizeof(char), sizeof(token), fd);
	    }
	    elementname = char2string(token);
	    el = new xmlelement("");
	  }
	}
      }
      break;

    case INSIDEELEMENT:
      {
	if(token == ' '){
	  state = INSIDEATTRIBUTE;
	  attributename.clear();
	  //std::cerr << "..............element: " << elementname << std::endl;
	  
	  el->setName(elementname.c_str());
	  _elements.push_back(el);
	}
	else if(token == '>'){
	  state = OUTSIDE;
	  //std::cerr << "Element: " << elementname << std::endl;
	  el->setEmpty();
	  el->setName(elementname.c_str());
	  _elements.push_back(el);

	}
	else{
	  elementname += char2string(token);
	}
      }
      break;

    case INSIDEATTRIBUTE:
      {
	if(token == '/' || token == '>'){
	  state = OUTSIDE;
	  
	  //std::cerr << "   ..........attribute: " << attributename;

	}
	else if(token == '='){
	  state = INSIDEVALUE;
	  bytesread += fread(&token, sizeof(char), sizeof(token), fd);
	  bytesread += fread(&token, sizeof(char), sizeof(token), fd);
	  value = char2string(token);
	  //std::cerr << "   attribute: " << attributename;

	}
	else{
	  attributename += char2string(token);
	}
	
      }
      break;
    case INSIDEVALUE:
      {
	if(token == '"'){
	  bytesread += fread(&token, sizeof(char), sizeof(token), fd);
	  if(token != ' '){
	    state = OUTSIDE;
	    //std::cerr << ".................="<< value << std::endl;
	    el->addAttribute(new xmlattribute(attributename.c_str(), value.c_str()));
	    attributename.clear();
	    if(token == '/')
	      el->setEmpty();
	  }
	  else{
	    state = INSIDEATTRIBUTE;
	    //std::cerr << "="<< value << std::endl;
	    el->addAttribute(new xmlattribute(attributename.c_str(), value.c_str()));
	    attributename.clear();
	    
	  }
	}
	else{
	  value += char2string(token);
	}
      }
      break;
    }
  }
  fclose(fd);

  if(!_elements.size())
    return 0;


  _documentroot = _elements[0];
  std::stack<xmlelement*> stk;
  stk.push(_documentroot);

  //std::cerr << "----------------------" << std::endl;
  for(unsigned int i = 1;i < _elements.size(); ++i){
    //std::cerr << _elements[i]->getName() << "("<< _elements[i]->empty()<< ")"<< std::endl;
    
    if(!stk.empty()){
      xmlelement* top = stk.top();
      if(std::string(top->getName()) != std::string(_elements[i]->getName()) && !top->empty()){
	top->addChild(_elements[i]);
	//std::cerr << top->getName() << "("<< top << ") has child " << _elements[i]->getName() << std::endl;
	if(!_elements[i]->empty())
	  stk.push(_elements[i]);
      }
      else{
	stk.pop();
      }
    }
    

    std::vector<xmlattribute*> ats = _elements[i]->getAttributes();
    for(unsigned int a = 0; a < ats.size(); ++a){
      //std::cerr << "\t" << ats[a]->getName() << "=\"" << ats[a]->getValueString() << "\""<< std::endl;
    }
    
  }
  //std::cerr << "----------------------" << std::endl;


  std::vector<xmlelement* > tmp;
  for(unsigned int i = 0; i < _elements.size(); ++i){
    if(_elements[i]->getAttributes().size() || !_elements[i]->empty() )
      tmp.push_back(_elements[i]);
  }
  //std::cerr << "!!!!! killed " << _elements.size() - tmp.size() << std::endl;
  _elements = tmp;
  
    
  return true;
}



const char*
xmldocument::getFileName() const{

  return _filename.c_str();
}


const xmlelement* const
xmldocument::getDocumentRoot() const{
  return _documentroot;
}

xmlelement* const
xmldocument::getDocumentRoot() {
  return _documentroot;
}

std::vector<xmlelement*>
xmldocument::getElementsByName(const char* a) const{
  std::vector<xmlelement*> vece;
  std::string n(a);

    for(unsigned int i = 0; i < _elements.size(); ++i)
    if(n == _elements[i]->getName())
      vece.push_back(_elements[i]);

  return vece;
}

std::string
xmldocument::char2string(char a){
  
  char buffer[2];
  buffer[0] = a;
  buffer[1] = '\0';
  return std::string(buffer);
}

}

