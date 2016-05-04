#ifndef KINECT_CALIBVOLUME_H
#define KINECT_CALIBVOLUME_H

#include <DataTypes.h>

#include <Point3.h>
#include <Matrix.h>

#include <string>
#include <vector>

namespace boost{
  class thread;
  class mutex;
}


namespace kinect{

  class KinectCalibrationFile;

  class CalibVolume{

  public:
    CalibVolume(const std::vector<KinectCalibrationFile*>& calibs);
    virtual ~CalibVolume();

    void drawSamplePoints();
    bool reload();
    void save();
    
    void saveSamplePoints(const char* bfname, unsigned stride);
    void loadSamplePoints(const char* bfname);
    void evaluate(const char* bfname);

	  void calcStats();  

    void prepareOffsets();

    void dumpSamplePoints();
    void clearSamples();
    void applySamples();
    virtual void applySamplesByThreadIDW   (const unsigned cv_width, const unsigned cv_height, const unsigned cv_depth, const unsigned i, const unsigned tid, const unsigned numthreads);

    virtual void applySamplesByThreadIDWAbsolute   (const unsigned cv_width, const unsigned cv_height, const unsigned cv_depth, const unsigned i, const unsigned tid, const unsigned numthreads);


    virtual void applySamplesByThreadBucket(const unsigned cv_width, const unsigned cv_height, const unsigned cv_depth, const unsigned i, const unsigned tid, const unsigned numthreads);
    void applySamplesNNI(bool doapply);
    void applySamplesNNIAbsolute(bool doapply);

  protected:
    std::vector<KinectCalibrationFile*> m_calibs;
  public:
    std::vector<std::string> m_cv_xyz_filenames;
    std::vector<std::string> m_cv_uv_filenames;
    std::vector<unsigned> m_cv_xyz_ids;
    std::vector<unsigned> m_cv_uv_ids;
    std::vector<unsigned> m_cv_widths;
    std::vector<unsigned> m_cv_heights;
    std::vector<unsigned> m_cv_depths;
    std::vector<float> m_cv_min_ds;
    std::vector<float> m_cv_max_ds;
    std::vector<xyz*> m_cv_xyzs;
    std::vector<uv*> m_cv_uvs;
    std::vector<bool*> m_cv_valids;
    bool bucket;
    unsigned num_neighbours;

  public:
    float getBilinear(float* data, unsigned width, unsigned height, float x, float y);

    xyz getTrilinear(xyz* data, unsigned width, unsigned height, unsigned depth, float x, float y, float z);
  protected:
    uv getTrilinear(uv* data, unsigned width, unsigned height, unsigned depth, float x, float y, float z);
    
    float gauss(float x, float sigma, float mean);
    gloost::Matrix m_poseoffset;
    unsigned m_cb_width;
    unsigned m_cb_height;
    std::vector<gloost::Point3> m_cb_points_local;
    bool m_running;
    std::vector<std::vector<samplePoint> >* m_sps_front;
    std::vector<std::vector<samplePoint> >* m_sps_back;
    std::vector<std::vector<samplePoint> >* m_sps;
    std::vector<std::vector<uv> >* m_errors;
    
    boost::mutex* m_mutex;
    unsigned m_frame_count;
    bool do_swap;
  public:
    bool calib_mode;
    static std::string serverendpoint;
  };


}

#endif // #ifndef KINECT_CALIBVOLUME_H

