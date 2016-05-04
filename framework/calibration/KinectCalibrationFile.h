
#ifndef H_KINECT_KINECTCALIBRATIONFILE
#define H_KINECT_KINECTCALIBRATIONFILE

#include <CameraView.h>
#include <DataTypes.h>

/// gloost system includes
#include <gloostConfig.h>
#include <gloostMath.h>
#include <UniformTypes.h>
#include <Vector3.h>
#include <Vector2.h>
#include <Matrix.h>
#include <Plane3.h>

/// cpp includes
#include <string>
#include <vector>

namespace sensor {
  class sensor;

}

namespace kinect
{


  //  Reads an Kinect kalibration file created with RGBDemo-0.5.0

  class KinectCalibrationFile : public mvt::CameraView {
	public:

    // class constructor
    KinectCalibrationFile(const std::string& filePath);

    // class destructor
    virtual ~KinectCalibrationFile();


    // parses the KinectCalibrationFile (*.yml)
    virtual bool parse();


    gloost::vec2& getColorFocalLength();
    gloost::vec2& getColorPrincipalPoint();
    gloost::vec3& getColorRadialDistortion();
    gloost::vec2& getColorTangentialDistortion();


    gloost::vec2& getDepthFocalLength();
    gloost::vec2& getDepthPrincipalPoint();
    gloost::vec3& getDepthRadialDistortion();
    gloost::vec2& getDepthTangentialDistortion();


    gloost::Matrix&  getRelativeRotation();
    gloost::Vector3& getRelativeTranslation();

    gloost::Matrix&  getWorldRotation();
    void setWorldRotation(gloost::Matrix& r);
    gloost::Vector3& getWorldTranslation();
    void setWorldTranslation(gloost::Vector3& t);

    

    float* getIntrinsicRGB9();
    float* getDistortionRGB5();

    float* getIntrinsicD9();
    float* getDistortionD5();



    virtual void printInfo() const;

    virtual void saveExtrinsic();

    /*virtual*/ void updateMatrices();


    float getNear();
    float getFar();

    void setNear(float n);
    void setFar(float f);

    unsigned getWidth();
    unsigned getHeight();
    unsigned getWidthC();
    unsigned getHeightC();


    std::string& getBaumerSerial();

    void applyCoordinateSystem(const CoordinateSystem& c, const gloost::Matrix& postMult);
    void applyCoordinateSystem3(const CoordinateSystem& c);


    unsigned isCompressedRGB();
    bool isCompressedDepth();
    void toggleCompressedDepth();

    void resetExtrinsic(bool saveover);

  protected:


    void advanceToNextToken(const std::string& searchToken,
                            std::ifstream& infile);
    
    float kommaStringToFloat(const std::string& token);

    float getNextTokenAsFloat(std::ifstream& infile);
    float getNextFloat(std::ifstream& infile);

  public:
    std::string _filePath;
    std::string _serial;
  protected:
    gloost::vec2 _colorFocalLength;
    gloost::vec2 _colorPrincipalPoint;
    gloost::vec3 _colorRadialDistortion;
    gloost::vec2 _colorTangentialDistortion;

    gloost::vec2 _depthFocalLength;
    gloost::vec2 _depthPrincipalPoint;
    gloost::vec3 _depthRadialDistortion;
    gloost::vec2 _depthTangentialDistortion;

    gloost::Matrix  _relativeRotation;
    gloost::Vector3 _relativeTranslation;

    gloost::Matrix  _worldRotation;
    gloost::Vector3 _worldTranslation;
    gloost::Matrix  _worldRotation2;
    gloost::Vector3 _worldTranslation2;
    gloost::Matrix  _worldRotation3;
    gloost::Vector3 _worldTranslation3;
    float _intrinsic_rgb[9];
    float _distortion_rgb[5];

    float _intrinsic_d[9];
    float _distortion_d[5];


    float _near;
    float _far;
    unsigned _width;
    unsigned _height;
    unsigned _widthc;
    unsigned _heightc;
    std::string _baumer_serial;

    unsigned _iscompressedrgb;
    bool _iscompresseddepth;

    sensor::sensor* _sensor;
    bool _is_sensored;


  public:
    bool use_bf;
    float min_length;
    unsigned num_channels_rgb;
    double depth_baseline; // 7.5e-02;
    double depth_offset;   // 1090;
    double depth_meanfocal;//580;

    gloost::Matrix _local_t;
    gloost::Matrix _local_r;

    void loadLocalTransform();


    gloost::Point3 pos_min;
    gloost::Point3 pos_max;

    gloost::Point3 neg_min;
    gloost::Point3 neg_max;

  private:
    gloost::Matrix _sensor_matrix;
  public:
    static bool s_use_sensor;
    static bool s_use_proxy;
    static bool s_compress;

  private:
    std::vector<gloost::Vector2> _pcoords;
  public:
    const std::vector<gloost::Vector2>& getPolygonCoords();
    static int s_compress_rgb;


    static float s_groundlevel;
    static int  s_bbxclip;
    gloost::Point3 calcPosEyeD(float d, unsigned x, unsigned y);
    gloost::Point3 calcPosEyeDFloat(float d, float x, float y);

    gloost::Point3 calcPosWS(float d, float u, float v);

    uv calcPosUV(float d, float u, float v);

    gloost::Point3 calcPosWSUV(float d, float u, float v, uv& coords);

    gloost::Point3 calcPosWSNearest(float d, float u, float v);
    uv calcPosUVNearest(float d, float u, float v);

    xyz* cv_xyz;
    uv* cv_uv;
    unsigned cv_width;
    unsigned cv_height;
    unsigned cv_depth;
    float    cv_min_d;
    float    cv_max_d;
    xyz getTrilinear(xyz* data, unsigned width, unsigned height, unsigned depth, float x, float y, float z);
    uv getTrilinear(uv* data, unsigned width, unsigned height, unsigned depth, float x, float y, float z);

};


} // namespace kinect


#endif // H_KINECT_KINECTCALIBRATIONFILE


