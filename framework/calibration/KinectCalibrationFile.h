
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

namespace kinect
{
  //  Reads an Kinect kalibration file created with RGBDemo-0.5.0
  class KinectCalibrationFile : public mvt::CameraView {
    public:
    // class constructor
    KinectCalibrationFile(const std::string& filePath);

    // parses the KinectCalibrationFile (*.yml)
    virtual bool parse();
    virtual void printInfo() const;

    /*virtual*/ void updateMatrices();

    float getNear() const;
    float getFar() const;

    unsigned getWidth() const;
    unsigned getHeight() const;
    unsigned getWidthC() const;
    unsigned getHeightC() const;

    bool isCompressedRGB() const;
    bool isCompressedDepth() const;

  protected:
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
    gloost::Vector3& getWorldTranslation();

    void setWorldRotation(gloost::Matrix& r);
    void setWorldTranslation(gloost::Vector3& t);

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

    unsigned _iscompressedrgb;
    bool _iscompresseddepth;

  public:
    float min_length;

    gloost::Matrix _local_t;
    gloost::Matrix _local_r;

    gloost::Point3 pos_min;
    gloost::Point3 pos_max;

    gloost::Point3 neg_min;
    gloost::Point3 neg_max;

  private:
    void loadLocalTransform();

  public:
    static bool s_use_sensor;
    static bool s_use_proxy;
    static bool s_compress;
    static int s_compress_rgb;
    static float s_groundlevel;
    static int  s_bbxclip;
};


} // namespace kinect


#endif // H_KINECT_KINECTCALIBRATIONFILE


