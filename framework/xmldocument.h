#ifndef XMLDOCUMENT_H
#define XMLDOCUMENT_H




#include <vector>
#include <string>

namespace minixml{

class xmlelement;


class xmldocument{

 public:

  xmldocument();
  xmldocument(const char* /*filename*/);
  virtual ~xmldocument();
  bool open();

  const char* getFileName() const;
  const xmlelement* const getDocumentRoot() const;
  xmlelement* const getDocumentRoot();
  virtual std::vector<xmlelement*> getElementsByName(const char* /*name*/) const;

 protected:
  std::string char2string(char /*token*/);
  std::string _filename;
  xmlelement* _documentroot;
  std::vector<xmlelement*> _elements;
};

}
#endif // #ifndef  XMLDOCUMENT_H
