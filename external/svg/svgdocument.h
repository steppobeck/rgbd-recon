#ifndef SVGDOCUMENT_H
#define SVGDOCUMENT_H




#include <xmldocument.h>

namespace minixml{

class svgdocument : public xmldocument{

 public:


  svgdocument();
  svgdocument(unsigned int width, unsigned int height);
  svgdocument(const char* filename);
  /*virtual*/ ~svgdocument();




  void setWidth(unsigned int);
  void setHeight(unsigned int);

  
  void setPageColor(const char* hexcolor);
  void setPageColor(unsigned char r, unsigned char g, unsigned char b);
  void setPageOpacity(float a);

  void setBorderColor(const char* hexcolor);
  void setBorderColor(unsigned char r, unsigned char g, unsigned char b);
  void setBorderOpacity(float a);

  void write(const char* filename);


 protected:

  void createEmpty();

};

}

#endif
