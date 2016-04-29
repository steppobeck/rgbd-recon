/// kinect system includes
#include <KinectCalibrationFile.h>

/// cpp includes
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h>
#include <stdexcept>

namespace gloost{

  void load(gloost::Matrix& m, const char* path){
    FILE* f = fopen(path, "rb");
    if(NULL == f)
      return;
#ifdef GLOOST_SYSTEM_MATH_WITH_FLOAT
    fread(m.data(), sizeof(float), 16, f);
#else
    fread(m.data(), sizeof(double), 16, f);
#endif
  }

  void save(gloost::Matrix& m, const char* path){
    FILE* f = fopen(path, "wb");
#ifdef GLOOST_SYSTEM_MATH_WITH_FLOAT
    fwrite(m.data(), sizeof(float), 16, f);
#else
    fwrite(m.data(), sizeof(double), 16, f);
#endif
  }

}

namespace kinect
{

/**
  \class KinectCalibrationFile

  \brief Reads an Kinect kalibration file created with RGBDemo-0.5.0

  \author Felix Weiszig
  \date   April 2011
  \remarks
*/

////////////////////////////////////////////////////////////////////////////////


/**
  \brief Class constructor

  \remarks ...
*/

  /*static*/ bool KinectCalibrationFile::s_use_proxy  = true;
  /*static*/ bool KinectCalibrationFile::s_compress   = false;
  /*static*/ int  KinectCalibrationFile::s_compress_rgb = -1;
  /*static*/ float  KinectCalibrationFile::s_groundlevel = -1.0f; // depr
  /*static*/ int  KinectCalibrationFile::s_bbxclip = 1;

KinectCalibrationFile::KinectCalibrationFile(const std::string& filePath)
  : CameraView(),
    _filePath(filePath),
    _serial(""),
    _colorFocalLength(),
    _colorPrincipalPoint(),
    _colorRadialDistortion(),
    _colorTangentialDistortion(),
    _depthFocalLength(),
    _depthPrincipalPoint(),
    _depthRadialDistortion(),
    _depthTangentialDistortion(),
    _relativeRotation(),
    _relativeTranslation(),
    _worldRotation(),
    _worldTranslation(),
    _worldRotation2(),
    _worldTranslation2(),
    _worldRotation3(),
    _worldTranslation3(),
    _intrinsic_rgb(),
    _distortion_rgb(),
    _intrinsic_d(),
    _distortion_d(),
    _near(0.3),
    _far(7.0),
    _width(0),
    _height(0),
    _widthc(0),
    _heightc(0),
    _iscompressedrgb(1),
    _iscompresseddepth(false),
    min_length(0.0125),
    _local_t(),
    _local_r(),
    pos_min(),
    pos_max(),
    neg_min(),
    neg_max()
{
#if 0
  if(s_compress_rgb == -1){ // i am the first to look if file exists rgbd_calib/compress.rgb
    std::string filename("rgbd_calib/compress.rgb");
    std::ifstream infile;
    infile.open(filename.c_str());
    if(infile){
      int test_val;
      infile >> test_val;
      infile.close();
      if(test_val == 0 || test_val == 1 || test_val == 5){
	s_compress_rgb = test_val;
	std::cerr << "KinectCalibrationFile setting compress_rgb to " << s_compress_rgb << std::endl;
      }
    }
    else{
      s_compress_rgb = -2;
    }
  }
#endif


  {
    std::string filename("rgbd_calib/groundlevel");
    std::ifstream infile;
    infile.open(filename.c_str());
    if(infile){
      float groundlevel = 0.0f;
      infile >> groundlevel;
      infile.close();
      s_groundlevel = groundlevel;
    }
  }

}

////////////////////////////////////////////////////////////////////////////////


/**
  \brief parses the KinectCalibrationFile (*.yml)
  \param ...
  \remarks ...
*/

bool
KinectCalibrationFile::parse()
{
  std::cerr << std::endl << "Message from KinectCalibrationFile::parse(): ";
  std::cerr << std::endl << "             parsing " << _filePath;
  std::cerr << std::endl;
  std::cerr << std::endl;

  _iscompresseddepth = s_compress;

  std::ifstream infile;
  infile.open( _filePath.c_str());


  std::string token;
  while(infile >> token)
  {

//    std::cerr << std::endl << token;

    // color
    if (token == "rgb_intrinsics:")
    {
//      std::cerr << std::endl << "rgb_intrinsics:";
      advanceToNextToken("[", infile);

      _colorFocalLength.u = getNextTokenAsFloat(infile);

      infile >> token;

      _colorPrincipalPoint.u = getNextTokenAsFloat(infile);

      infile >> token;

      _colorFocalLength.v    = getNextTokenAsFloat(infile);
      _colorPrincipalPoint.v = getNextTokenAsFloat(infile);



      _intrinsic_rgb[0] = _colorFocalLength.u;
      _intrinsic_rgb[1] = 0.0f;
      _intrinsic_rgb[2] = _colorPrincipalPoint.u;
      _intrinsic_rgb[3] = 0.0f;
      _intrinsic_rgb[4] = _colorFocalLength.v;
      _intrinsic_rgb[5] = _colorPrincipalPoint.v;
      _intrinsic_rgb[6] = 0.0f;
      _intrinsic_rgb[7] = 0.0f;
      _intrinsic_rgb[8] = 1.0f;

    }

    else if (token == "rgb_distortion:")
    {
//      std::cerr << std::endl << "rgb_distortion:";
      advanceToNextToken("[", infile);

      _colorRadialDistortion.x = getNextTokenAsFloat(infile);
      _colorRadialDistortion.y = getNextTokenAsFloat(infile);

      _colorTangentialDistortion.u = getNextTokenAsFloat(infile);
      _colorTangentialDistortion.v = getNextTokenAsFloat(infile);

      _colorRadialDistortion.z = getNextFloat(infile);

     
      _distortion_rgb[0] = _colorRadialDistortion.x;
      _distortion_rgb[1] = _colorRadialDistortion.y;
      _distortion_rgb[2] = _colorTangentialDistortion.u;
      _distortion_rgb[3] = _colorTangentialDistortion.v;
      _distortion_rgb[4] = _colorRadialDistortion.z;
    }




    // depth
    else if (token == "depth_intrinsics:")
    {
//      std::cerr << std::endl << "depth_intrinsics:";
      advanceToNextToken("[", infile);

      _depthFocalLength.u = getNextTokenAsFloat(infile);

      infile >> token;

      _depthPrincipalPoint.u = getNextTokenAsFloat(infile);

      infile >> token;

      _depthFocalLength.v = getNextTokenAsFloat(infile);
      _depthPrincipalPoint.v = getNextTokenAsFloat(infile);


      _intrinsic_d[0] = _depthFocalLength.u;
      _intrinsic_d[1] = 0.0f;
      _intrinsic_d[2] = _depthPrincipalPoint.u;
      _intrinsic_d[3] = 0.0f;
      _intrinsic_d[4] = _depthFocalLength.v;
      _intrinsic_d[5] = _depthPrincipalPoint.v;
      _intrinsic_d[6] = 0.0f;
      _intrinsic_d[7] = 0.0f;
      _intrinsic_d[8] = 1.0f;

    }

    else if (token == "depth_distortion:")
    {
//      std::cerr << std::endl << "depth_distortion:";
      advanceToNextToken("[", infile);

      _depthRadialDistortion.x = getNextTokenAsFloat(infile);
      _depthRadialDistortion.y = getNextTokenAsFloat(infile);

      _depthTangentialDistortion.u = getNextTokenAsFloat(infile);
      _depthTangentialDistortion.v = getNextTokenAsFloat(infile);

      _depthRadialDistortion.z = getNextFloat(infile);

      _distortion_d[0] = _depthRadialDistortion.x;
      _distortion_d[1] = _depthRadialDistortion.y;
      _distortion_d[2] = _depthTangentialDistortion.u;
      _distortion_d[3] = _depthTangentialDistortion.v;
      _distortion_d[4] = _depthRadialDistortion.z;
    }

    // relative transform
    else if (token == "R:")
    {
//      std::cerr << std::endl << "R:";
      advanceToNextToken("[", infile);


      _relativeRotation.setIdentity();

      _relativeRotation[0]  = getNextTokenAsFloat(infile);
      _relativeRotation[1]  = getNextTokenAsFloat(infile);
      _relativeRotation[2]  = getNextTokenAsFloat(infile);

      _relativeRotation[4]  = getNextTokenAsFloat(infile);
      _relativeRotation[5]  = getNextTokenAsFloat(infile);
      _relativeRotation[6]  = getNextTokenAsFloat(infile);

      _relativeRotation[8]  = getNextTokenAsFloat(infile);
      _relativeRotation[9]  = getNextTokenAsFloat(infile);
      _relativeRotation[10] = getNextFloat(infile);

      _relativeRotation[12] =  0.0;
      _relativeRotation[13] =  0.0;
      _relativeRotation[14] =  0.0;

    }

    else if (token == "T:")
    {
//      std::cerr << std::endl << "T:";
      advanceToNextToken("[", infile);

      _relativeTranslation[0] = getNextTokenAsFloat(infile);
      _relativeTranslation[1] = getNextTokenAsFloat(infile);
      _relativeTranslation[2] = getNextFloat(infile);
    }

    else if (token == "rgb_size:")
    {
//      std::cerr << std::endl << "rgb_size:";
      advanceToNextToken("[", infile);

      _widthc = getNextTokenAsFloat(infile);
      _heightc = getNextFloat(infile);
    }
    else if (token == "depth_size:")
    {
//      std::cerr << std::endl << "depth_size:";
      advanceToNextToken("[", infile);

      _width = getNextTokenAsFloat(infile);
      _height = getNextFloat(infile);
    }
    else if(token == "near_far:"){
      std::cerr << std::endl << "near_far:";
      advanceToNextToken("[", infile);
      _near = getNextTokenAsFloat(infile);
      _far  = getNextFloat(infile);
    }
    else if(token == "compress_rgb:"){
      std::cerr << std::endl << "compress_rgb:";
      advanceToNextToken("[", infile);
      _iscompressedrgb = (unsigned) getNextTokenAsFloat(infile);
      getNextFloat(infile);

      // maybe override value
      if(s_compress_rgb == 0 || s_compress_rgb == 1 || s_compress_rgb == 5){
	_iscompressedrgb = (unsigned) s_compress_rgb;
      }

    }
    else if(token == "min_length:"){
      std::cerr << std::endl << "min_length:";
      advanceToNextToken("[", infile);
      min_length = getNextTokenAsFloat(infile);
      getNextFloat(infile);
    }
    else if(token == "compress_depth:"){
      std::cerr << std::endl << "compress_depth:";
      advanceToNextToken("[", infile);
      _iscompresseddepth = (bool) ((unsigned) getNextTokenAsFloat(infile));
      getNextFloat(infile);
      
    }
    else {
      // throw std::invalid_argument{"token " + token + " not supported"};
    }

  }

  infile.close();

  { // .ext
    std::string e_filepath(_filePath.c_str());
    e_filepath.replace( e_filepath.end() - 3, e_filepath.end(), "ext");
    infile.open( e_filepath.c_str());
    if(infile){
      
      float token;
      infile >> token;
      _worldTranslation[0] = token;
      infile >> token;
      _worldTranslation[1] = token;
      infile >> token;
      _worldTranslation[2] = token;
    
      _worldRotation.setIdentity();
    
    
      infile >> token;	
      _worldRotation[0]  = token;
      infile >> token;
      _worldRotation[1]  = token;
      infile >> token;
      _worldRotation[2]  = token;
      
      infile >> token;	
      _worldRotation[4]  = token;
      infile >> token;
      _worldRotation[5]  = token;
      infile >> token;
      _worldRotation[6]  = token;
      
      infile >> token;
      _worldRotation[8]  = token;
      infile >> token;
      _worldRotation[9]  = token;
      infile >> token;
      _worldRotation[10] = token;
      
      _worldRotation[12] =  0.0;
      _worldRotation[13] =  0.0;
      _worldRotation[14] =  0.0;
    

      infile.close();
    }
    else{
      _worldRotation.setIdentity();
      _worldTranslation[0] = 0.0;
      _worldTranslation[1] = 0.0;
      _worldTranslation[2] = 0.0;
    }
  }


  { // .ext2
    std::string e_filepath(_filePath.c_str());
    e_filepath.replace( e_filepath.end() - 3, e_filepath.end(), "ext2");
    infile.open( e_filepath.c_str());
    if(infile){
      
      float token;
      infile >> token;
      _worldTranslation2[0] = token;
      infile >> token;
      _worldTranslation2[1] = token;
      infile >> token;
      _worldTranslation2[2] = token;
    
      _worldRotation2.setIdentity();
    
    
      infile >> token;	
      _worldRotation2[0]  = token;
      infile >> token;
      _worldRotation2[1]  = token;
      infile >> token;
      _worldRotation2[2]  = token;
      
      infile >> token;	
      _worldRotation2[4]  = token;
      infile >> token;
      _worldRotation2[5]  = token;
      infile >> token;
      _worldRotation2[6]  = token;
      
      infile >> token;
      _worldRotation2[8]  = token;
      infile >> token;
      _worldRotation2[9]  = token;
      infile >> token;
      _worldRotation2[10] = token;
      
      _worldRotation2[12] =  0.0;
      _worldRotation2[13] =  0.0;
      _worldRotation2[14] =  0.0;
    

      infile.close();
    }
    else{
      _worldRotation2.setIdentity();
      _worldTranslation2[0] = 0.0;
      _worldTranslation2[1] = 0.0;
      _worldTranslation2[2] = 0.0;
    }
  }

  { // .ext3
    std::string e_filepath(_filePath.c_str());
    e_filepath.replace( e_filepath.end() - 3, e_filepath.end(), "ext3");
    infile.open( e_filepath.c_str());
    if(infile){
      
      float token;
      infile >> token;
      _worldTranslation3[0] = token;
      infile >> token;
      _worldTranslation3[1] = token;
      infile >> token;
      _worldTranslation3[2] = token;
    
      _worldRotation3.setIdentity();
    
    
      infile >> token;	
      _worldRotation3[0]  = token;
      infile >> token;
      _worldRotation3[1]  = token;
      infile >> token;
      _worldRotation3[2]  = token;
      
      infile >> token;	
      _worldRotation3[4]  = token;
      infile >> token;
      _worldRotation3[5]  = token;
      infile >> token;
      _worldRotation3[6]  = token;
      
      infile >> token;
      _worldRotation3[8]  = token;
      infile >> token;
      _worldRotation3[9]  = token;
      infile >> token;
      _worldRotation3[10] = token;
      
      _worldRotation3[12] =  0.0;
      _worldRotation3[13] =  0.0;
      _worldRotation3[14] =  0.0;
    

      infile.close();
    }
    else{
      _worldRotation3.setIdentity();
      _worldTranslation3[0] = 0.0;
      _worldTranslation3[1] = 0.0;
      _worldTranslation3[2] = 0.0;
    }
  }


  { // .bbx
    std::string e_filepath(_filePath.c_str());
    e_filepath.replace( e_filepath.end() - 3, e_filepath.end(), "bbx");
    infile.open( e_filepath.c_str());
    if(infile){
      
      float token;

      infile >> token;
      pos_min[0] = token;
      infile >> token;
      pos_min[1] = token;
      infile >> token;
      pos_min[2] = token;

      infile >> token;
      pos_max[0] = token;
      infile >> token;
      pos_max[1] = token;
      infile >> token;
      pos_max[2] = token;

      infile >> token;
      neg_min[0] = token;
      infile >> token;
      neg_min[1] = token;
      infile >> token;
      neg_min[2] = token;

      infile >> token;
      neg_max[0] = token;
      infile >> token;
      neg_max[1] = token;
      infile >> token;
      neg_max[2] = token;

      std::cerr << "pos_min: " << pos_min << std::endl;
      std::cerr << "pos_max: " << pos_max << std::endl;

      std::cerr << "neg_min: " << neg_min << std::endl;
      std::cerr << "neg_max: " << neg_max << std::endl;

      infile.close();
    }
    else{
      pos_min = gloost::Point3(-100,-100,-100);
      pos_max = gloost::Point3( 100, 100, 100);

      neg_min = gloost::Point3(-100,-100,-100);
      neg_max = gloost::Point3(-100,-100,-100); // this is correct!

    }
  }

  loadLocalTransform();

  return true;
}

////////////////////////////////////////////////////////////////////////////////
void
KinectCalibrationFile::advanceToNextToken(const std::string& searchToken,
                                          std::ifstream& infile)
{
  std::string token;
  while(infile >> token)
  {
    if (token == searchToken)
    {
      return;
    }
  }
}


////////////////////////////////////////////////////////////////////////////////


float
KinectCalibrationFile::kommaStringToFloat(const std::string& token)
{
  return atof(std::string(token.substr(0, token.length() - 1)).c_str());
}


////////////////////////////////////////////////////////////////////////////////


float
KinectCalibrationFile::getNextTokenAsFloat(std::ifstream& infile)
{
  std::string token;
  infile >> token;
  //std::cerr << token << std::endl;
  return kommaStringToFloat(token);
}


float
KinectCalibrationFile::getNextFloat(std::ifstream& infile){
  std::string token;
  infile >> token;
  //std::cerr << token << std::endl;
  return atof(token.c_str());
}

////////////////////////////////////////////////////////////////////////////////


void
KinectCalibrationFile::printInfo() const
{

  std::cerr << std::endl << "### File: #############################################";
  std::cerr << std::endl << "  path:          " << _filePath;
  std::cerr << std::endl << "### Color camera: #####################################";
  std::cerr << std::endl << "  colorFocalLength:          " << _colorFocalLength;
  std::cerr << std::endl << "  colorPrincipalPoint:       " << _colorPrincipalPoint;
  std::cerr << std::endl << "  colorRadialDistortion:     " << _colorRadialDistortion;
  std::cerr << std::endl << "  colorTangentialDistortion: " << _colorTangentialDistortion;

  std::cerr << std::endl << "  intrinsic: " ;
  for(unsigned i = 0; i < 9; ++i)
    std::cerr << _intrinsic_rgb[i] << " ,";
  std::cerr << std::endl;

  std::cerr << std::endl << "  distortion: " ;
  for(unsigned i = 0; i < 5; ++i)
    std::cerr << _distortion_rgb[i] << " ,";
  std::cerr << std::endl;


  std::cerr << std::endl;

  std::cerr << std::endl << "### Depth camera: #####################################";
  std::cerr << std::endl << "  depthFocalLength:          " << _depthFocalLength;
  std::cerr << std::endl << "  depthPrincipalPoint:       " << _depthPrincipalPoint;
  std::cerr << std::endl << "  depthRadialDistortion:     " << _depthRadialDistortion;
  std::cerr << std::endl << "  depthTangentialDistortion: " << _depthTangentialDistortion;
  std::cerr << std::endl;

  std::cerr << std::endl << "  intrinsic: " ;
  for(unsigned i = 0; i < 9; ++i)
    std::cerr << _intrinsic_d[i] << " ,";
  std::cerr << std::endl;

  std::cerr << std::endl << "  distortion: " ;
  for(unsigned i = 0; i < 5; ++i)
    std::cerr << _distortion_d[i] << " ,";
  std::cerr << std::endl;


  std::cerr << std::endl << "### Relative transformation: #########################";
  std::cerr << std::endl << "  R: ";
  std::cerr << std::endl << _relativeRotation;
  std::cerr << std::endl;
  std::cerr << std::endl << "  T: ";
  std::cerr << std::endl << _relativeTranslation;
  std::cerr << std::endl;
  std::cerr << std::endl;

  std::cerr << std::endl << "### World transformation: #########################";
  std::cerr << std::endl << "  R: ";
  std::cerr << std::endl << _worldRotation;
  std::cerr << std::endl;
  std::cerr << std::endl << "  T: ";
  std::cerr << std::endl << _worldTranslation;
  std::cerr << std::endl;
  std::cerr << std::endl;



}


////////////////////////////////////////////////////////////////////////////////

gloost::vec2&
KinectCalibrationFile::getColorFocalLength(){
  return _colorFocalLength;
}

gloost::vec2&
KinectCalibrationFile::getColorPrincipalPoint(){
  return _colorPrincipalPoint;
}

gloost::vec3&
KinectCalibrationFile::getColorRadialDistortion(){
  return _colorRadialDistortion;
}

gloost::vec2&
KinectCalibrationFile::getColorTangentialDistortion(){
  return _colorTangentialDistortion;
}


gloost::vec2&
KinectCalibrationFile::getDepthFocalLength(){
  return _depthFocalLength;
}

gloost::vec2&
KinectCalibrationFile::getDepthPrincipalPoint(){
  return _depthPrincipalPoint;
}

gloost::vec3&
KinectCalibrationFile::getDepthRadialDistortion(){
  return _depthRadialDistortion;
}

gloost::vec2&
KinectCalibrationFile::getDepthTangentialDistortion(){
  return _depthTangentialDistortion;
}


gloost::Matrix&
KinectCalibrationFile::getRelativeRotation(){
  return _relativeRotation;
}

gloost::Vector3&
KinectCalibrationFile::getRelativeTranslation(){
  return _relativeTranslation;
}

gloost::Matrix&
KinectCalibrationFile::getWorldRotation(){
  return _worldRotation;
}

void
KinectCalibrationFile::setWorldRotation(gloost::Matrix& r){
  _worldRotation = r;
}

gloost::Vector3&
KinectCalibrationFile::getWorldTranslation(){
  return _worldTranslation;
}

void
KinectCalibrationFile::setWorldTranslation(gloost::Vector3& t){
  _worldTranslation = t;
}

namespace {
  float mydeg2rad(float d){
    return (d * M_PI / 180.0);
  }
}

void
KinectCalibrationFile::loadLocalTransform(){
  {
    std::ifstream infile;
    std::string filepath(_filePath.c_str());
    filepath.replace( filepath.end() - 3, filepath.end(), "local");
    infile.open( filepath.c_str());
    if(infile){
      float x,y,z,rx,ry,rz;
      infile >> x >> y >> z >> rx >> ry >> rz;
      _local_t.setIdentity();
      _local_r.setIdentity();
      _local_t.setTranslate(x, y, z);
      _local_r.setRotate(mydeg2rad(rx),mydeg2rad(ry),mydeg2rad(rz));
      
    }
    infile.close();
  }

  {
    std::ifstream infile;
    std::string filepath(_filePath.c_str());
    filepath.replace( filepath.end() - 3, filepath.end(), "serial");
    infile.open( filepath.c_str());
    if(infile){
      infile >> _serial;
      std::cerr << "using serial: " << _serial << std::endl;
    }
    infile.close();
  }
}

/*virtual*/ void
KinectCalibrationFile::updateMatrices(){

  gloost::Matrix relativeTransformMat(this->getRelativeRotation());

  gloost::Vector3 relativeTranslate(this->getRelativeTranslation());

  float fx_rgb = this->getColorFocalLength().u;
  float fy_rgb = this->getColorFocalLength().v;
  float cx_rgb = this->getColorPrincipalPoint().u;
  float cy_rgb = this->getColorPrincipalPoint().v;

  float fx_d = this->getDepthFocalLength().u;
  float fy_d = this->getDepthFocalLength().v;
  float cx_d = this->getDepthPrincipalPoint().u;
  float cy_d = this->getDepthPrincipalPoint().v;


  relativeTransformMat.transpose();
  relativeTransformMat.invert();
  relativeTranslate = -1.0 * relativeTranslate;


  /*

		vec2 pos_d = gl_Vertex.xy * texSize;
		float X_d  = ((pos_d.x - cx_d)/fx_d) * depth;
		float Y_d  = ((pos_d.y - cy_d)/fy_d) * depth;
		vec3 POS_d = vec3(X_d, Y_d, depth);

  */

  gloost::Matrix image_d_scale_pre;
  image_d_scale_pre.setIdentity();
  image_d_scale_pre.setScale(_width,_height,1.0);
  gloost::Matrix image_d_trans;
  image_d_trans.setIdentity();
  image_d_trans.setTranslate( -cx_d,-cy_d,0.0);
  gloost::Matrix image_d_scale_post;
  image_d_scale_post.setIdentity();
  image_d_scale_post.setScale(1.0/fx_d,1.0/fy_d,1.0);
  image_d_to_eye_d = image_d_scale_post * image_d_trans * image_d_scale_pre;



  gloost::Matrix trans;
  trans.setIdentity();
  trans.setTranslate(_worldTranslation[0],_worldTranslation[1],_worldTranslation[2]);
  gloost::Matrix rot(_worldRotation);
  eye_d_to_world = rot * trans;

  //eye_d_to_world.invert();

  gloost::Matrix trans2;
  trans2.setIdentity();
  trans2.setTranslate(_worldTranslation2[0],_worldTranslation2[1],_worldTranslation2[2]);
  gloost::Matrix rot2(_worldRotation2);
  
  gloost::Matrix trans3;
  trans3.setIdentity();
  trans3.setTranslate(_worldTranslation3[0],_worldTranslation3[1],_worldTranslation3[2]);
  gloost::Matrix rot3(_worldRotation3);

  eye_d_to_world = (trans2 * rot2) * eye_d_to_world;
  eye_d_to_world = (rot3 * trans3) * eye_d_to_world;


  gloost::Matrix w;

  eye_d_to_world = _local_r * _local_t * eye_d_to_world;

  eye_d_to_eye_rgb = relativeTransformMat;
  eye_d_to_eye_rgb.setTranslate(relativeTranslate[0],relativeTranslate[1],relativeTranslate[2]);
  

  /*
		vec4 POS_rgb = eye_d_to_eye_rgb * POS_d;		

		vec2 pos_rgb;
		pos_rgb.x = (POS_rgb.x/POS_rgb.z) * fx_rgb + cx_rgb;
		pos_rgb.y = (POS_rgb.y/POS_rgb.z) * fy_rgb + cy_rgb;


		tex_coord = pos_rgb * texSizeInv;

  */
  gloost::Matrix image_rgb_scale_pre;
  image_rgb_scale_pre.setIdentity();
  image_rgb_scale_pre.setScale(fx_rgb,fy_rgb,1.0);
  gloost::Matrix image_rgb_trans;
  image_rgb_trans.setIdentity();
  image_rgb_trans.setTranslate( cx_rgb,cy_rgb,0.0);
  gloost::Matrix image_rgb_scale_post;
  image_rgb_scale_post.setIdentity();
  image_rgb_scale_post.setScale(1.0/_widthc,1.0/_heightc,1.0);
  eye_rgb_to_image_rgb = image_rgb_scale_post * image_rgb_trans * image_rgb_scale_pre;
  





  eye_d_to_image_d = image_d_to_eye_d;
  eye_d_to_image_d.invert();

  gloost::Matrix eye_rgb_to_eye_d(eye_d_to_eye_rgb);
  eye_rgb_to_eye_d.invert();
  eye_rgb_to_world = eye_d_to_world * eye_rgb_to_eye_d;

  // update frustum depth
  const float near(_near);
  const float far(_far);

  m_near_lower_left[0] = ((0.0 - cx_d)/fx_d) * near;
  m_near_lower_left[1] = ((0.0 - cy_d)/fy_d) * near;
  m_near_lower_left[2] = near;

  m_near_lower_right[0] = ((_width - cx_d)/fx_d) * near;
  m_near_lower_right[1] = ((0.0 - cy_d)/fy_d) * near;
  m_near_lower_right[2] = near;

  m_near_upper_right[0] = ((_width - cx_d)/fx_d) * near;
  m_near_upper_right[1] = ((_height - cy_d)/fy_d) * near;
  m_near_upper_right[2] = near;

  m_near_upper_left[0] = ((0.0 - cx_d)/fx_d) * near;
  m_near_upper_left[1] = ((_height - cy_d)/fy_d) * near;
  m_near_upper_left[2] = near;

  m_far_lower_left[0] = ((0.0 - cx_d)/fx_d) * far;
  m_far_lower_left[1] = ((0.0 - cy_d)/fy_d) * far;
  m_far_lower_left[2] = far;

  m_far_lower_right[0] = ((_width - cx_d)/fx_d) * far;
  m_far_lower_right[1] = ((0.0 - cy_d)/fy_d) * far;
  m_far_lower_right[2] = far;

  m_far_upper_right[0] = ((_width - cx_d)/fx_d) * far;
  m_far_upper_right[1] = ((_height - cy_d)/fy_d) * far;
  m_far_upper_right[2] = far;

  m_far_upper_left[0] = ((0.0 - cx_d)/fx_d) * far;
  m_far_upper_left[1] = ((_height - cy_d)/fy_d) * far;
  m_far_upper_left[2] = far;

  // update frustum rgb

  m_near_lower_left_color[0] = ((0.0 - cx_rgb)/fx_rgb) * near;
  m_near_lower_left_color[1] = ((0.0 - cy_rgb)/fy_rgb) * near;
  m_near_lower_left_color[2] = near;

  m_near_lower_right_color[0] = ((_widthc - cx_rgb)/fx_rgb) * near;
  m_near_lower_right_color[1] = ((0.0 - cy_rgb)/fy_rgb) * near;
  m_near_lower_right_color[2] = near;

  m_near_upper_right_color[0] = ((_widthc - cx_rgb)/fx_rgb) * near;
  m_near_upper_right_color[1] = ((_heightc - cy_rgb)/fy_rgb) * near;
  m_near_upper_right_color[2] = near;

  m_near_upper_left_color[0] = ((0.0 - cx_rgb)/fx_rgb) * near;
  m_near_upper_left_color[1] = ((_heightc - cy_rgb)/fy_rgb) * near;
  m_near_upper_left_color[2] = near;

  m_far_lower_left_color[0] = ((0.0 - cx_rgb)/fx_rgb) * far;
  m_far_lower_left_color[1] = ((0.0 - cy_rgb)/fy_rgb) * far;
  m_far_lower_left_color[2] = far;

  m_far_lower_right_color[0] = ((_widthc - cx_rgb)/fx_rgb) * far;
  m_far_lower_right_color[1] = ((0.0 - cy_rgb)/fy_rgb) * far;
  m_far_lower_right_color[2] = far;

  m_far_upper_right_color[0] = ((_widthc - cx_rgb)/fx_rgb) * far;
  m_far_upper_right_color[1] = ((_heightc - cy_rgb)/fy_rgb) * far;
  m_far_upper_right_color[2] = far;

  m_far_upper_left_color[0] = ((0.0 - cx_rgb)/fx_rgb) * far;
  m_far_upper_left_color[1] = ((_heightc - cy_rgb)/fy_rgb) * far;
  m_far_upper_left_color[2] = far;

}


float
KinectCalibrationFile::getNear() const {
  return _near;
}

float
KinectCalibrationFile::getFar() const {
  return _far;
}

unsigned
KinectCalibrationFile::getWidth() const {
  return _width;
}

unsigned
KinectCalibrationFile::getHeight() const {
  return _height;
}

unsigned
  KinectCalibrationFile::getWidthC() const {
  return _widthc;
}

unsigned
  KinectCalibrationFile::getHeightC() const {
  return _heightc;
}

  bool
  KinectCalibrationFile::isCompressedRGB() const {
    return _iscompressedrgb;
  }

  bool
  KinectCalibrationFile::isCompressedDepth() const {
    return _iscompresseddepth;
  }
} // namespace kinect