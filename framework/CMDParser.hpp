#ifndef RGBD_CALIB_CMDPARSER_HPP
#define RGBD_CALIB_CMDPARSER_HPP

#include <string>
#include <vector>
#include <map>


class CMDParser{
 public:
  CMDParser(std::string arguments);
  ~CMDParser();

  void addOpt(std::string opt, int numValues, std::string optlong, std::string help = "");
  void showHelp();
  void init(int& argc, char** argv);

  int isOptSet(std::string opt);

  std::vector<int> getOptsInt(std::string);
  std::vector<float> getOptsFloat(std::string);
  std::vector<std::string> getOptsString(std::string);


  std::vector<std::string> getArgs() const;

 private:
  std::map<std::string,std::vector<std::string>* > _opts;
  std::map<std::string,int> _optsNumValues;
  std::map<std::string,std::string> _optslong;
  std::vector<std::string> _args;
  std::string _help;
  std::string _arguments;
};




#endif //#ifndef RGBD_CALIB_CMDPARSER_HPP
