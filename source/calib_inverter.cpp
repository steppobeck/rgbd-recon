#include "calibration_inverter.hpp"
#include "BoundingBox.h"
#include "CMDParser.h"

#include <memory>

gloost::BoundingBox     g_bbox{};

std::unique_ptr<kinect::CalibrationInverter> g_inv;
float default_voxel_size = 0.007f;
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
  CMDParser p("ks_file");
  p.addOpt("s",1,"voxel_size", "set size of voxel in m (default 0.007)");

  p.init(argc,argv);

  float voxel_size = default_voxel_size;
  if (p.isOptSet("s")) {
    voxel_size = p.getOptsFloat("s")[0];
  }
  
  std::vector<std::string> args{p.getArgs()}; 

  std::string file_name{args[0]};
  std::string ext(file_name.substr(file_name.find_last_of(".") + 1));
  if (file_name.empty() || ext != "ks") {
    throw std::invalid_argument{"No .ks file specified"};
    exit(EXIT_FAILURE);
  }

  std::vector<std::string> calib_filenames;
  gloost::Point3 bbox_min{0.0f, 0.0f, 0.0f};
  gloost::Point3 bbox_max{0.0f, 0.0f, 0.0f};

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

  glm::fvec3 bbox_dimensions = glm::fvec3{g_bbox.getPMax()[0] - g_bbox.getPMin()[0],
                                          g_bbox.getPMax()[1] - g_bbox.getPMin()[1],
                                          g_bbox.getPMax()[2] - g_bbox.getPMin()[2]};
  glm::uvec3 volume_res{glm::ceil(bbox_dimensions / voxel_size)};          
  g_inv = std::unique_ptr<kinect::CalibrationInverter>{new kinect::CalibrationInverter(calib_filenames, g_bbox)};

  std::cout << "using resolution " << volume_res.x << ", " << volume_res.y << ", " << volume_res.z << std::endl;
  g_inv->calculateInverseVolumes(volume_res);
  g_inv->writeInverseVolumes(resource_path);

  return EXIT_SUCCESS;
}