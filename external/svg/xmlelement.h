#ifndef XMLELEMENT_H
#define XMLELEMENT_H



#include <vector>
#include <string>


namespace minixml{

class xmlattribute;



class xmlelement{

 public:
  xmlelement(const char* /*name*/);
  virtual ~xmlelement();


  const char* getName() const;
  void setName(const char* /*name*/);

  void setContent(const char* /*content*/);
  void addContent(const char* /*content*/);
  void addAttribute(xmlattribute* /*attribute*/);
  void addChild(xmlelement* /*element*/);

  const char* getContentString() const;
  int getContentInt() const;
  float getContentFloat() const;
  
  const xmlattribute* getAttributeByName(const char* /*name*/) const;
  xmlattribute* getAttributeByName(const char* /*name*/) ;
  const std::vector<xmlattribute*>& getAttributes() const;

  std::vector<xmlelement*> getChildrenByName(const char* /*name*/) const;
  const std::vector<xmlelement*> getChildren() const;

  void setEmpty();
  bool empty() const;

  std::string tostring(bool formatted = false) const;

 protected:
  static std::string getUniqueID(const char* prefix = 0);
  static unsigned int _uniqueID;


 private:
  std::string _name;
  std::vector<xmlattribute*> _attributes;
  std::vector<xmlelement*> _children;
  std::string _content;
  bool _empty;
};

}
#endif // #ifndef  XMLELEMENT_H
