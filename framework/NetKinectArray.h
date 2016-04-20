#ifndef KINECT_NETKINECTARRAY_H
#define KINECT_NETKINECTARRAY_H




#include <GL/glew.h>
#include <GL/gl.h>

#include <ARTListener.h>
#include <Matrix.h>

#include <string>
#include <vector>
#include <atomic>

namespace boost{
  class thread;
  class mutex;
}

namespace mvt{


  class TextureArray;

}

namespace gloost{
  class Shader;
  class UniformSet;
}

namespace kinect{

  struct double_pbo{
    unsigned size;
    unsigned char* back;
    std::atomic<bool> needSwap;
    unsigned frontID;
    unsigned backID;
    std::vector<gloost::Matrix> current_poses;
    float* matrixdata_back;
    float* matrixdata_front;
    void swap(){
      unsigned tmp = frontID;
      frontID = backID;
      backID = tmp;

      float* tmpf = matrixdata_front;
      matrixdata_front = matrixdata_back;
      matrixdata_back = tmpf;
    }

  };

  class KinectCalibrationFile;

  class NetKinectArray{


  public:
    NetKinectArray(const char* config, bool readfromfile = false);

    NetKinectArray(std::vector<KinectCalibrationFile*>& calibs, bool readfromfile = false);

    virtual ~NetKinectArray();


    virtual void update(bool filter = true);

    void bilateralFilter();

    void bindToTextureUnits(GLenum start_texture_unit = GL_TEXTURE0);

    unsigned getWidth();
    unsigned getWidthC();

    unsigned getHeight();
    unsigned getHeightC();

    unsigned getNumLayers();

    std::vector<KinectCalibrationFile*>& getCalibs();

    std::vector<gloost::Matrix> current_poses;

    void reloadShader();

    void drawGeometry();
    void writeCurrentTexture(std::string prefix);
    void writeBMP(std::string, std::vector<unsigned char> const&, unsigned int offset, unsigned int bytesPerPixel);
    
    gloost::Matrix getArtlsensorMatrix(unsigned int sensorNumber) const;

    ARTListener* getARTL();

    bool isRecording();
    bool isPhoto();

    mvt::TextureArray* getDepthArrayBack();
    mvt::TextureArray* getDepthArray();

  protected:
    void readLoop();
    void readFromFiles();
    bool init();
    unsigned m_width;
    unsigned m_widthc;
    unsigned m_height;
    unsigned m_heightc;


    unsigned m_numLayers;
    std::vector<KinectCalibrationFile*> m_kinectcs;
    mvt::TextureArray* m_colorArray;
    mvt::TextureArray* m_depthArray;

    mvt::TextureArray*  m_colorArray_back;
    mvt::TextureArray*  m_depthArray_back;
    gloost::Shader*     m_shader_bf;
    gloost::UniformSet* m_uniforms_bf;
    unsigned m_fboID;
    unsigned m_gaussID;

    unsigned m_colorsize; // per frame
    unsigned m_depthsize; // per frame
    double_pbo m_colorsCPU3;
    double_pbo m_depthsCPU3;

    boost::mutex* m_mutex;
    boost::thread* m_readThread;
    bool m_running;
    std::string m_serverport;
    unsigned m_trigger;
    static bool s_glewInit;

    ARTListener* m_artl;

    bool m_isrecording;

    bool m_readfromfile;

    bool m_isphoto;

    std::string m_config;

  public:
    bool depth_compression_lex;
    float depth_compression_ratio;
  };


}


#endif // #ifndef KINECT_NETKINECTARRAY_H
