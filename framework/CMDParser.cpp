#include "CMDParser.hpp"
#include <iostream>
#include <stdlib.h>

CMDParser::CMDParser(std::string arguments):
  _opts(),
  _optsNumValues(),
  _optslong(),
  _args(),
  _help(),
  _arguments(arguments){
  addOpt("h", -1,"help" ,/*cchar**/"show this help message and exit");
}


  
CMDParser::~CMDParser(){
  std::map<std::string, std::vector<std::string>* >::iterator optIt = _opts.begin();
  for(; optIt != _opts.end(); ++optIt){
    if((*optIt).second){
      delete (*optIt).second;
    }
  }
}


void
CMDParser::addOpt(std::string opt, int numValues, std::string optlong, std::string help){
  
  _opts[opt] = 0;
  _optsNumValues[opt] = numValues;
  _optslong[opt] = optlong;


  _help += "\t-" + opt + ", --" + optlong;


  if(numValues>0)
    for (int i=0; i<numValues; ++i)
      _help += " <value>";
  if(numValues == 0)
    _help += " <value0> ... <valueN>";
  _help += "\n\t\t" + help + "\n";
}


void
CMDParser::showHelp(){
  std::cout << _help;
  exit(0);
}


void
CMDParser::init(int& argc, char** argv){
  std::string progname = argv[0];
  _help = "usage: " + progname + " [options] " + _arguments + "\n" + "options can be:" + "\n" + _help;


  for(int i = 1; i < argc; ++i){
    std::string arg = argv[i];
    if(arg == "--help" || arg == "-h")
      showHelp();
    int foundArg = 0;
    std::map<std::string, std::vector<std::string>* >::iterator optIt = _opts.begin();
    for(; optIt != _opts.end(); ++optIt){
      if( ("--" + _optslong[(*optIt).first]) == arg || ("-" + (*optIt).first)== arg){
	foundArg = 1;
	int numValues = _optsNumValues[(*optIt).first];
	if(numValues < 0){
	  _opts[(*optIt).first] = new std::vector<std::string>;
	}
	else if(numValues > 0){
	  _opts[(*optIt).first] = new std::vector<std::string>;
	  for(int j = 1; j <= numValues; ++j){
	    if(i + j < argc){
	      (_opts[(*optIt).first])->push_back(argv[j+i]);
	    }
	    else
	      showHelp();
	  }
	  i += numValues;
	}
	else{ // arbritraty values not implemented yet
	  std::cout << "ERROR: arbritraty values not implemented yet" << std::endl;
	  exit(0);
	}
      }
    }
    if(!foundArg)
      _args.push_back(arg);
    
  }
  if(_args.empty() && (_arguments != ""))
    showHelp();
}

int
CMDParser::isOptSet(std::string opt){
  if(_opts[opt] != 0)
    return 1;
  return 0;
}


std::vector<int>
CMDParser::getOptsInt(std::string opt) {
  std::vector<int> opts;
  if(isOptSet(opt))
    for(unsigned int i = 0; i < _opts[opt]->size(); ++i)
      opts.push_back(atoi((*_opts[opt])[i].c_str()));
  return opts;
}
 
std::vector<float>
CMDParser::getOptsFloat(std::string opt) {
  std::vector<float> opts;
  if(isOptSet(opt))
    for(unsigned int i = 0; i < _opts[opt]->size(); ++i)
      opts.push_back( (float) atof((*_opts[opt])[i].c_str()));
  return opts;
}

std::vector<std::string>
CMDParser::getOptsString(std::string opt) {
  std::vector<std::string> opts;
  if(isOptSet(opt))
    for(unsigned int i = 0; i < _opts[opt]->size(); ++i)
      opts.push_back((*_opts[opt])[i]);
  return opts;
}

std::vector<std::string>
CMDParser::getArgs() const{
  return _args;
}

