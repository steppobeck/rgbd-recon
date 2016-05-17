#include "calibration_inverter.hpp"
#include "BoundingBox.h"
#include "CMDParser.h"

#include <memory>

gloost::BoundingBox     g_bbox{};

std::unique_ptr<kinect::CalibrationInverter> g_inv;

//////////////////////////////////////////////////////////////////////////////////////////
void init(std::vector<std::string> args){
  std::string file_name{args[0]};
  std::string ext(file_name.substr(file_name.find_last_of(".") + 1));
  if (file_name.empty() || ext != "ks") {
    throw std::invalid_argument{"No .ks file specified"};
  }

  std::vector<std::string> calib_filenames;
  gloost::Point3 bbox_min{-1.0f ,0.0f, -1.0f};
  gloost::Point3 bbox_max{ 1.0f ,2.2f, 1.0f};

  std::string resource_path = file_name.substr(0, file_name.find_last_of("/\\")) + '/';
  std::ifstream in(file_name);
  std::string token;
  while(in >> token){
    if (token == "kinect") {
      in >> token;
      // detect absolute path
      if (token[0] == '/' || token[1] == ':') {
        calib_filenames.push_back(token);
      }
      else {
        calib_filenames.push_back(resource_path + token);
      }
    }
    else if (token == "bbx") {
      in >> bbox_min[0];
      in >> bbox_min[1];
      in >> bbox_min[2];
      in >> bbox_max[0];
      in >> bbox_max[1];
      in >> bbox_max[2];
    }
  }
  in.close();
  // update bounding box dimensions with read values
  g_bbox.setPMin(bbox_min);
  g_bbox.setPMax(bbox_max);

  g_inv = std::unique_ptr<kinect::CalibrationInverter>{new kinect::CalibrationInverter(calib_filenames, g_bbox)};
  glm::uvec3 volume_res{128,256,128};
  g_inv->calculateInverseVolumes(volume_res);
  g_inv->writeInverseVolumes(resource_path);
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
  CMDParser p("ks_file ...");
  p.init(argc,argv);
  
  init(p.getArgs());

  return EXIT_SUCCESS;
}