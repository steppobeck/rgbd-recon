#ifndef SVGGROUP_H
#define SVGGROUP_H



#include <xmlelement.h>



namespace minixml{

class svggroup : public xmlelement{

 public:

  svggroup();
  /*virtual*/ ~svggroup();

 private:
  
  static unsigned int _groupnumber;
  static unsigned int nextGroupNumber();

};



}

#endif
