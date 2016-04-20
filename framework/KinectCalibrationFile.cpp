

/// kinect system includes
#include <KinectCalibrationFile.h>


#include <sensor.h>
#include <devicemanager.h>
#include <device.h>



/// cpp includes
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h>




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

  /*static*/ bool KinectCalibrationFile::s_use_sensor = true;
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
    _baumer_serial(),
    _iscompressedrgb(1),
    _iscompresseddepth(false),
    _sensor(0),
    _is_sensored(false),
    use_bf(true),
    min_length(0.0125),
    num_channels_rgb(3),
    depth_baseline(7.5e-02),
    depth_offset(1090),
    depth_meanfocal(580),
    _local_t(),
    _local_r(),
    pos_min(),
    pos_max(),
    neg_min(),
    neg_max(),
    _sensor_matrix(),
    _pcoords(),
    cv_xyz(0),
    cv_uv(0),
    cv_width(0),
    cv_height(0),
    cv_depth(0),
    cv_min_d(0),
    cv_max_d(0)

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
  \brief Class destructor
  \remarks ...
*/

KinectCalibrationFile::~KinectCalibrationFile()
{
	// insert your code here
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

    else if (token == "baumer_serial:")
    {
//      std::cerr << std::endl << "baumer_serial:";
      advanceToNextToken("[", infile);
      infile >> _baumer_serial;

    }

    else if(token == "depth_base_and_offset:"){
      advanceToNextToken("[", infile);
      depth_baseline = getNextTokenAsFloat(infile);
      depth_offset = getNextFloat(infile);
      
    }
    else if(token == "depth_base_and_offset_meanfocal:"){
      advanceToNextToken("[", infile);
      depth_baseline = getNextTokenAsFloat(infile);
      depth_offset = getNextTokenAsFloat(infile);
      depth_meanfocal = getNextFloat(infile);      
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
      float dummy  = getNextFloat(infile);

      // maybe override value
      if(s_compress_rgb == 0 || s_compress_rgb == 1 || s_compress_rgb == 5){
	_iscompressedrgb = (unsigned) s_compress_rgb;
      }

    }
    else if(token == "use_bf:"){
      std::cerr << std::endl << "use_bf:";
      advanceToNextToken("[", infile);
      use_bf = ((unsigned) getNextTokenAsFloat(infile) > 0.0 ? true : false);
      float dummy  = getNextFloat(infile);
    }
    else if(token == "min_length:"){
      std::cerr << std::endl << "min_length:";
      advanceToNextToken("[", infile);
      min_length = getNextTokenAsFloat(infile);
      float dummy  = getNextFloat(infile);
    }
    else if(token == "num_channels_rgb:"){
      std::cerr << std::endl << "num_channels_rgb:";
      advanceToNextToken("[", infile);
      num_channels_rgb = (unsigned) getNextTokenAsFloat(infile);
      float dummy  = getNextFloat(infile);
    }
    else if(token == "compress_depth:"){
      std::cerr << std::endl << "compress_depth:";
      advanceToNextToken("[", infile);
      _iscompresseddepth = (bool) ((unsigned) getNextTokenAsFloat(infile));
      float dummy  = getNextFloat(infile);
      
    }
    else if(token == "dtrack_sensor:"){
      std::cerr << std::endl << "dtrack_sensor:";
      advanceToNextToken("[", infile);
      unsigned sensor_id = (unsigned) getNextTokenAsFloat(infile);
      unsigned sensor_port  = (unsigned) getNextFloat(infile);
      _is_sensored = true;

      _iscompresseddepth = false;

      if(s_use_sensor){
	
	_sensor = new sensor::sensor(sensor::devicemanager::the()->get_dtrack(sensor_port, sensor::timevalue::const_050_ms), sensor_id);

	gloost::Matrix xmitterOffset;
	xmitterOffset.setIdentity();
	_sensor->setTransmitterOffset(xmitterOffset);
	gloost::Matrix receiverOffset;
	receiverOffset.setIdentity();
	_sensor->setReceiverOffset(receiverOffset);
      }
    }
    else if(token == "dtrack_proxy:"){
      std::cerr << std::endl << "dtrack_proxy:";
      advanceToNextToken("[", infile);
      unsigned dummyu = (int) getNextTokenAsFloat(infile);
      //float dummy  = getNextFloat(infile);
      std::string serverport;
      infile >> serverport;
      std::cerr << "using proxy on: "  << serverport << std::endl;
      std::string endpoint("tcp://" + serverport);



    }

  }

#if 0
  depth_meanfocal = (_depthFocalLength.u + _depthFocalLength.v)/2.0;
#endif

  infile.close();

  { // .sensor
    std::string i_filepath(_filePath.c_str());
    i_filepath.replace( i_filepath.end() - 3, i_filepath.end(), "sensor");
    _sensor_matrix.setIdentity();
    gloost::load(_sensor_matrix, i_filepath.c_str());
  }


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


  { // load cv_xyz
    if(cv_xyz){
      delete [] cv_xyz;
    }
    std::string fpath(_filePath.c_str());
    fpath.replace( fpath.end() - 3, fpath.end(), "cv_xyz");
    //std::cerr << "loading " << fpath << std::endl;

    
    if( access( fpath.c_str(), R_OK ) != -1 ) {
      FILE* f_xyz = fopen( fpath.c_str(), "rb");
      unsigned nbr = 0;
      nbr = fread(&cv_width, sizeof(unsigned), 1, f_xyz);
      nbr = fread(&cv_height, sizeof(unsigned), 1, f_xyz);
      nbr = fread(&cv_depth, sizeof(unsigned), 1, f_xyz);
      nbr = fread(&cv_min_d, sizeof(float), 1, f_xyz);
      nbr = fread(&cv_max_d, sizeof(float), 1, f_xyz);
      cv_xyz = new xyz[cv_width * cv_height * cv_depth];
      nbr = fread(cv_xyz, sizeof(xyz), cv_width * cv_height * cv_depth, f_xyz);
      fclose(f_xyz);
    }
  }

  { // load cv_uv;
    if(cv_uv){
      delete [] cv_uv;
    }
    std::string fpath(_filePath.c_str());
    fpath.replace( fpath.end() - 3, fpath.end(), "cv_uv");
    //std::cerr << "loading " << fpath << std::endl;

    if( access( fpath.c_str(), R_OK ) != -1 ) {
      FILE* f_uv = fopen( fpath.c_str(), "rb");
      unsigned nbr = 0;
      nbr = fread(&cv_width, sizeof(unsigned), 1, f_uv);
      nbr = fread(&cv_height, sizeof(unsigned), 1, f_uv);
      nbr = fread(&cv_depth, sizeof(unsigned), 1, f_uv);
      nbr = fread(&cv_min_d, sizeof(float), 1, f_uv);
      nbr = fread(&cv_max_d, sizeof(float), 1, f_uv);
      cv_uv = new uv[cv_width * cv_height * cv_depth];
      nbr = fread(cv_uv, sizeof(uv), cv_width * cv_height * cv_depth, f_uv);
      fclose(f_uv);
    }
  }



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

float*
KinectCalibrationFile::getIntrinsicRGB9(){
  return _intrinsic_rgb;
}


float*
KinectCalibrationFile::getDistortionRGB5(){
  return _distortion_rgb;
}


float*
KinectCalibrationFile::getIntrinsicD9(){
  return _intrinsic_d;
}


float*
KinectCalibrationFile::getDistortionD5(){
  return _distortion_d;
}

  namespace{
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

  {
    std::ifstream infile;
    std::string filepath(_filePath.c_str());
    filepath.replace( filepath.end() - 3, filepath.end(), "polygon");
    infile.open( filepath.c_str());
    if(infile){
      std::string token;
      while(infile >> token){
	if("coord" == token){
	  float x;
	  float y;
	  infile >> x >> y;
	  _pcoords.push_back(gloost::Vector2(x,y));
	}
      }
    }
    infile.close();
  }


}


void
KinectCalibrationFile::saveExtrinsic(){

  {
    // write matlab ".ext" file
    std::ofstream outfile;
    std::string i_filepath(_filePath.c_str());
    i_filepath.replace( i_filepath.end() - 3, i_filepath.end(), "ext");
    std::cerr << "KinectCalibrationFile::saveExtrinsic() " << this << " saving to " << i_filepath << std::endl;
    
    outfile.open( i_filepath.c_str());
    
    
    outfile << std::setprecision(20)
	    << _worldTranslation[0] << std::endl
	    << _worldTranslation[1] << std::endl
	    << _worldTranslation[2] << std::endl
	    << _worldRotation[0] << " " << _worldRotation[1] << " " << _worldRotation[2] << std::endl
	    << _worldRotation[4] << " " << _worldRotation[5] << " " << _worldRotation[6] << std::endl
	    << _worldRotation[8] << " " << _worldRotation[9] << " " << _worldRotation[10] << std::endl;
    
    outfile.close();
  }

  {
    // write matlab ".ext2" file
    std::ofstream outfile;
    std::string i_filepath(_filePath.c_str());
    i_filepath.replace( i_filepath.end() - 3, i_filepath.end(), "ext2");
    std::cerr << "KinectCalibrationFile::saveExtrinsic() " << this << " saving to " << i_filepath << std::endl;
    
    outfile.open( i_filepath.c_str());
    
    
    outfile << std::setprecision(20)
	    << _worldTranslation2[0] << std::endl
	    << _worldTranslation2[1] << std::endl
	    << _worldTranslation2[2] << std::endl
	    << _worldRotation2[0] << " " << _worldRotation2[1] << " " << _worldRotation2[2] << std::endl
	    << _worldRotation2[4] << " " << _worldRotation2[5] << " " << _worldRotation2[6] << std::endl
	    << _worldRotation2[8] << " " << _worldRotation2[9] << " " << _worldRotation2[10] << std::endl;
    
    outfile.close();
  }

  {
    // write matlab ".ext3" file
    std::ofstream outfile;
    std::string i_filepath(_filePath.c_str());
    i_filepath.replace( i_filepath.end() - 3, i_filepath.end(), "ext3");
    std::cerr << "KinectCalibrationFile::saveExtrinsic() " << this << " saving to " << i_filepath << std::endl;
    
    outfile.open( i_filepath.c_str());
    
    
    outfile << std::setprecision(20)
	    << _worldTranslation3[0] << std::endl
	    << _worldTranslation3[1] << std::endl
	    << _worldTranslation3[2] << std::endl
	    << _worldRotation3[0] << " " << _worldRotation3[1] << " " << _worldRotation3[2] << std::endl
	    << _worldRotation3[4] << " " << _worldRotation3[5] << " " << _worldRotation3[6] << std::endl
	    << _worldRotation3[8] << " " << _worldRotation3[9] << " " << _worldRotation3[10] << std::endl;
    
    outfile.close();
  }


  if(_sensor){

    std::string i_filepath(_filePath.c_str());
    i_filepath.replace( i_filepath.end() - 3, i_filepath.end(), "sensor");
    _sensor_matrix = _sensor->getMatrix();
    gloost::save(_sensor_matrix, i_filepath.c_str());
    std::cerr << "KinectCalibrationFile::saveExtrinsic() " << this << " saving to " << i_filepath << std::endl;
  }


}


void
KinectCalibrationFile::resetExtrinsic(bool saveover){

  _worldTranslation = gloost::Vector3(0.0,0.0,0.0);
  _worldRotation.setIdentity();
  
  _worldTranslation2 = gloost::Vector3(0.0,0.0,0.0);
  _worldRotation2.setIdentity();

  _worldTranslation3 = gloost::Vector3(0.0,0.0,0.0);
  _worldRotation3.setIdentity();
  
  _local_t.setIdentity();
  _local_r.setIdentity();

  if(saveover){
    saveExtrinsic();
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

  if(_sensor){
    gloost::Matrix s(_sensor->getMatrix());
    eye_d_to_world = s * _sensor_matrix.inverted() * eye_d_to_world;
  }



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
KinectCalibrationFile::getNear(){
  return _near;
}

float
KinectCalibrationFile::getFar(){
  return _far;
}


void
KinectCalibrationFile::setNear(float n){
  _near = n;
}

void
KinectCalibrationFile::setFar(float f){
  _far = f;
}



unsigned
KinectCalibrationFile::getWidth(){
  return _width;
}

unsigned
KinectCalibrationFile::getHeight(){
  return _height;
}

unsigned
  KinectCalibrationFile::getWidthC(){
  return _widthc;
}

unsigned
  KinectCalibrationFile::getHeightC(){
  return _heightc;
}


std::string&
KinectCalibrationFile::getBaumerSerial(){
  return _baumer_serial;
}

void
KinectCalibrationFile::applyCoordinateSystem(const CoordinateSystem& c, const gloost::Matrix& postMult){



  _worldTranslation[0] = -c._o[0];
  _worldTranslation[1] = -c._o[1];
  _worldTranslation[2] = -c._o[2];

  _worldRotation.setIdentity();

  _worldRotation[0] = c._ex[0];
  _worldRotation[1] = c._ex[1];
  _worldRotation[2] = c._ex[2];

  _worldRotation[4] = c._ey[0];
  _worldRotation[5] = c._ey[1];
  _worldRotation[6] = c._ey[2];

  _worldRotation[8] = c._ez[0];
  _worldRotation[9] = c._ez[1];
  _worldRotation[10]= c._ez[2];
  
  //_worldRotation.transpose();
  _worldRotation.invert();



  _worldTranslation2[0] = postMult[12];
  _worldTranslation2[1] = postMult[13];
  _worldTranslation2[2] = postMult[14];

  _worldRotation2 = postMult;

  _worldRotation2[12] = 0.0;
  _worldRotation2[13] = 0.0;
  _worldRotation2[14] = 0.0;


}

void
KinectCalibrationFile::applyCoordinateSystem3(const CoordinateSystem& c){

  std::cerr << this << " KinectCalibrationFile::applyCoordinateSystem3" << std::endl;

  _worldTranslation3[0] = -c._o[0];
  _worldTranslation3[1] = -c._o[1];
  _worldTranslation3[2] = -c._o[2];

  _worldRotation3.setIdentity();

  _worldRotation3[0] = c._ex[0];
  _worldRotation3[1] = c._ex[1];
  _worldRotation3[2] = c._ex[2];

  _worldRotation3[4] = c._ey[0];
  _worldRotation3[5] = c._ey[1];
  _worldRotation3[6] = c._ey[2];

  _worldRotation3[8] = c._ez[0];
  _worldRotation3[9] = c._ez[1];
  _worldRotation3[10]= c._ez[2];

  
  //_worldRotation3.transpose();
  _worldRotation3.invert();
}


  unsigned
  KinectCalibrationFile::isCompressedRGB(){
    return _iscompressedrgb;
  }

  bool
  KinectCalibrationFile::isCompressedDepth(){
    return _iscompresseddepth;
  }

  void
  KinectCalibrationFile::toggleCompressedDepth(){
    _iscompresseddepth = !_iscompresseddepth;
  }

  bool
  KinectCalibrationFile::hasSensor(){
    return _sensor != 0;
  }

  bool
  KinectCalibrationFile::isSensored(){
    return _is_sensored;
  }

  const std::vector<gloost::Vector2>&
  KinectCalibrationFile::getPolygonCoords(){
    return _pcoords;
  }

gloost::Point3
KinectCalibrationFile::calcPosEyeD(float depth, unsigned x, unsigned y){

  const float cx_d(getDepthPrincipalPoint().u);
  const float cy_d(getDepthPrincipalPoint().v);
    
  const float fx_d(getDepthFocalLength().u);
  const float fy_d(getDepthFocalLength().v);

  float X_d  = ((x - cx_d)/fx_d) * depth;
  float Y_d  = ((y - cy_d)/fy_d) * depth;
  return gloost::Point3(X_d, Y_d, depth);
}


gloost::Point3
KinectCalibrationFile::calcPosEyeDFloat(float depth, float x, float y){

  const float cx_d(getDepthPrincipalPoint().u);
  const float cy_d(getDepthPrincipalPoint().v);
    
  const float fx_d(getDepthFocalLength().u);
  const float fy_d(getDepthFocalLength().v);

  float X_d  = ((x - cx_d)/fx_d) * depth;
  float Y_d  = ((y - cy_d)/fy_d) * depth;
  return gloost::Point3(X_d, Y_d, depth);
}



gloost::Point3
KinectCalibrationFile::calcPosWS(float depth, float u, float v){

  const static float inv_u = (cv_width  * 1.0f)/getWidth();
  const static float inv_v = (cv_height * 1.0f)/getHeight();

  const float x = u * inv_u;
  const float y = v * inv_v;
  const float z = cv_depth * (  depth - cv_min_d)/(cv_max_d - cv_min_d);
  xyz pos = getTrilinear(cv_xyz, cv_width, cv_height, cv_depth, x , y , z );

  return gloost::Point3(pos.x, pos.y, pos.z);
}
 
uv
KinectCalibrationFile::calcPosUV(float depth, float u, float v){
  const static float inv_u = (cv_width  * 1.0f)/getWidth();
  const static float inv_v = (cv_height * 1.0f)/getHeight();

  const float x = u * inv_u;
  const float y = v * inv_v;
  const float z = cv_depth * (  depth - cv_min_d)/(cv_max_d - cv_min_d);
  
  uv coords_norm = getTrilinear(cv_uv, cv_width, cv_height, cv_depth, x , y , z );
  uv coords;
  coords.u = std::max(0.0f, std::min(coords_norm.u, 1.0f)) * _widthc;
  coords.v = std::max(0.0f, std::min(coords_norm.v, 1.0f)) * _heightc;
  return coords;
}

gloost::Point3
KinectCalibrationFile::calcPosWSUV(float depth, float u, float v, uv& coords){

  const static float inv_u = (cv_width  * 1.0f)/getWidth();
  const static float inv_v = (cv_height * 1.0f)/getHeight();

#if 0
  const float x = u * inv_u;
  const float y = v * inv_v;
  const float z = cv_depth * (  depth - cv_min_d)/(cv_max_d - cv_min_d);
  xyz pos = getTrilinear(cv_xyz, cv_width, cv_height, cv_depth, x , y , z );

  uv coords_norm = getTrilinear(cv_uv, cv_width, cv_height, cv_depth, x , y , z );
#else
  const unsigned x = std::max(0.0f, u * inv_u);
  const unsigned y = std::max(0.0f, v * inv_v);
  const unsigned z = std::max(0.0f, cv_depth * (  depth - cv_min_d)/(cv_max_d - cv_min_d));
  xyz pos = cv_xyz[z * cv_width * cv_height + y * cv_width + x];

  uv coords_norm = cv_uv[z * cv_width * cv_height + y * cv_width + x];
#endif

  coords.u = std::max(0.0f, std::min(coords_norm.u, 1.0f)) * _widthc;
  coords.v = std::max(0.0f, std::min(coords_norm.v, 1.0f)) * _heightc;

  return gloost::Point3(pos.x, pos.y, pos.z);

}


gloost::Point3
KinectCalibrationFile::calcPosWSNearest(float depth, float u, float v){
  const static float inv_u = (cv_width  * 1.0f)/getWidth();
  const static float inv_v = (cv_height * 1.0f)/getHeight();
  const unsigned x = std::max(0.0f, u * inv_u);
  const unsigned y = std::max(0.0f, v * inv_v);
  const unsigned z = std::max(0.0f, cv_depth * (  depth - cv_min_d)/(cv_max_d - cv_min_d));
  xyz pos = cv_xyz[z * cv_width * cv_height + y * cv_width + x];
  return gloost::Point3(pos.x, pos.y, pos.z);
}

uv
KinectCalibrationFile::calcPosUVNearest(float depth, float u, float v){
  const static float inv_u = (cv_width  * 1.0f)/getWidth();
  const static float inv_v = (cv_height * 1.0f)/getHeight();
  const unsigned x = std::max(0.0f, u * inv_u);
  const unsigned y = std::max(0.0f, v * inv_v);
  const unsigned z = std::max(0.0f, cv_depth * (  depth - cv_min_d)/(cv_max_d - cv_min_d));
  uv coords_norm = cv_uv[z * cv_width * cv_height + y * cv_width + x];
  uv coords;
  coords.u = std::max(0.0f, std::min(coords_norm.u, 1.0f)) * _widthc;
  coords.v = std::max(0.0f, std::min(coords_norm.v, 1.0f)) * _heightc;
  return coords;
}




  xyz
  KinectCalibrationFile::getTrilinear(xyz* data, unsigned width, unsigned height, unsigned depth, float x, float y, float z){

    // calculate weights and boundaries along x direction
    unsigned xa = std::floor(x);
    unsigned xb = std::ceil(x);
    float w_xb = x - xa;
    float w_xa = 1.0 - w_xb;

    // calculate weights and boundaries along y direction
    unsigned ya = std::floor(y);
    unsigned yb = std::ceil(y);
    float w_yb = y - ya;
    float w_ya = 1.0 - w_yb;

    // calculate weights and boundaries along z direction
    unsigned za = std::floor(z);
    unsigned zb = std::ceil(z);
    float w_zb = z - za;
    float w_za = 1.0 - w_zb;

    // calculate indices to access data
    const unsigned idmax = width * height * depth;
    unsigned id000 = std::min( za * width * height + ya * width + xa  , idmax);
    unsigned id100 = std::min( za * width * height + ya * width + xb  , idmax);
    unsigned id110 = std::min( za * width * height + yb * width + xb  , idmax);
    unsigned id010 = std::min( za * width * height + yb * width + xa  , idmax);

    unsigned id001 = std::min( zb * width * height + ya * width + xa  , idmax);
    unsigned id101 = std::min( zb * width * height + ya * width + xb  , idmax);
    unsigned id111 = std::min( zb * width * height + yb * width + xb  , idmax);
    unsigned id011 = std::min( zb * width * height + yb * width + xa  , idmax);



    // 1. interpolate between x direction: 4 times;
    xyz   tmp_000_100 = w_xa * data[id000] + w_xb * data[id100];
    xyz   tmp_010_110 = w_xa * data[id010] + w_xb * data[id110];
    xyz   tmp_001_101 = w_xa * data[id001] + w_xb * data[id101];
    xyz   tmp_011_111 = w_xa * data[id011] + w_xb * data[id111];

    // 2. interpolate between y direction: 2 times;

    xyz   tmp_A = w_ya * tmp_000_100 + w_yb * tmp_010_110;
    xyz   tmp_B = w_ya * tmp_001_101 + w_yb * tmp_011_111;

    xyz result = w_za * tmp_A + w_zb * tmp_B;

    return result;
  }


  uv
  KinectCalibrationFile::getTrilinear(uv* data, unsigned width, unsigned height, unsigned depth, float x, float y, float z){

    // calculate weights and boundaries along x direction
    unsigned xa = std::floor(x);
    unsigned xb = std::ceil(x);
    float w_xb = x - xa;
    float w_xa = 1.0 - w_xb;

    // calculate weights and boundaries along y direction
    unsigned ya = std::floor(y);
    unsigned yb = std::ceil(y);
    float w_yb = y - ya;
    float w_ya = 1.0 - w_yb;

    // calculate weights and boundaries along z direction
    unsigned za = std::floor(z);
    unsigned zb = std::ceil(z);
    float w_zb = z - za;
    float w_za = 1.0 - w_zb;

    // calculate indices to access data
    const unsigned idmax = width * height * depth;
    unsigned id000 = std::min( za * width * height + ya * width + xa  , idmax);
    unsigned id100 = std::min( za * width * height + ya * width + xb  , idmax);
    unsigned id110 = std::min( za * width * height + yb * width + xb  , idmax);
    unsigned id010 = std::min( za * width * height + yb * width + xa  , idmax);

    unsigned id001 = std::min( zb * width * height + ya * width + xa  , idmax);
    unsigned id101 = std::min( zb * width * height + ya * width + xb  , idmax);
    unsigned id111 = std::min( zb * width * height + yb * width + xb  , idmax);
    unsigned id011 = std::min( zb * width * height + yb * width + xa  , idmax);



    // 1. interpolate between x direction: 4 times;
    uv   tmp_000_100 = w_xa * data[id000] + w_xb * data[id100];
    uv   tmp_010_110 = w_xa * data[id010] + w_xb * data[id110];
    uv   tmp_001_101 = w_xa * data[id001] + w_xb * data[id101];
    uv   tmp_011_111 = w_xa * data[id011] + w_xb * data[id111];

    // 2. interpolate between y direction: 2 times;

    uv   tmp_A = w_ya * tmp_000_100 + w_yb * tmp_010_110;
    uv   tmp_B = w_ya * tmp_001_101 + w_yb * tmp_011_111;

    uv result = w_za * tmp_A + w_zb * tmp_B;

    return result;
  }




} // namespace kinect


