#include <CMDParser.h>
#include <iostream>

int main(int argc, char* argv[]){
  CMDParser p("...");
  p.init(argc,argv);

  return 0;
}

