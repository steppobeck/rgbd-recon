#include "CalibVolume.h"

#include <KinectCalibrationFile.h>

#include <NaturalNeighbourInterpolator.h>

#include <timevalue.h>


#include <svgdocument.h>
#include <svgrect.h>
#include <svgtextspan.h>
#include <svgline.h>

#include <gloostHelper.h>

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>

#include <zmq.hpp>

#include <algorithm>
#include <limits>       // std::numeric_limits

#include <fstream>

// on achill   "tcp://141.54.147.22:7001"
// on orpheus  "tcp://141.54.147.58:7001"
// on boreas   "tcp://141.54.147.32:7001"
//#define SERVERENDPOINT "tcp://141.54.147.22:7001"
#define LCDSIDE


namespace{



  void convertTo8Bit(float* in, std::uint8_t** out, unsigned w, unsigned h){
    
    float max_v = 0.0;
    for(unsigned idx = 0; idx != w*h; ++idx){
      max_v = std::max(in[idx],max_v);
    }
    if(max_v != 0.0){
      for(unsigned idx = 0; idx != w*h; ++idx){
	const float n = in[idx]/max_v;
	std::uint8_t v = (std::uint8_t) (255.0 * n);
	(*out)[idx] = v;
      }
    }

  }

  bool addCross(minixml::svgdocument& doc, float x, float y, float size, float strokeWidth, std::uint8_t r, std::uint8_t g, std::uint8_t b){

    // define the svg element style
    minixml::svgelementstyle lineStyle(r, g, b);
    lineStyle.setStroke(minixml::svgelementstyle::rgb2hex(r, g, b).c_str());
    lineStyle.setStrokeWidth(strokeWidth);
    
    float halfSize = size * 0.5f;
    
    // horizontal line
    doc.getDocumentRoot()->addChild(new minixml::svgline(x-halfSize, y, x+halfSize, y, lineStyle));
    
    // vertical line
    doc.getDocumentRoot()->addChild(new minixml::svgline(x, y-halfSize, x, y+halfSize, lineStyle));
    
    return true;
  }



  bool fillGrid(minixml::svgdocument& doc, unsigned dwidth, unsigned dheight, unsigned width, unsigned height, unsigned stride){
    
    const float sx = (dwidth * 1.0)/width;
    const float sy = (dheight * 1.0)/height;
    
    // horizonal
    for(unsigned y = stride; y < height; ++y){
      if(y % stride == 0){
	// define the svg element style
	minixml::svgelementstyle lineStyle(180, 180, 180);
	lineStyle.setStroke(minixml::svgelementstyle::rgb2hex(180, 180, 180).c_str());
	lineStyle.setStrokeWidth(0.5);
	doc.getDocumentRoot()->addChild(new minixml::svgline(0.0, y * sy, dwidth, y * sy, lineStyle));
      }
    }

    // verical
    for(unsigned x = stride; x < width; ++x){
      if(x % stride == 0){
	// define the svg element style
	minixml::svgelementstyle lineStyle(180, 180, 180);
	lineStyle.setStroke(minixml::svgelementstyle::rgb2hex(180, 180, 180).c_str());
	lineStyle.setStrokeWidth(0.5);
	doc.getDocumentRoot()->addChild(new minixml::svgline(x * sx, 0.0, x * sx, dheight, lineStyle));
      }
    }
    
    return true;
  }
  
}



namespace kinect{

  /*static*/ std::string CalibVolume::serverendpoint("tcp://141.54.147.22:7001");

  CalibVolume::CalibVolume(const std::vector<KinectCalibrationFile*>& calibs):
    m_calibs(calibs),
    m_cv_xyz_filenames(),
    m_cv_uv_filenames(),
    m_cv_xyz_ids(),
    m_cv_uv_ids(),
    m_cv_widths(),
    m_cv_heights(),
    m_cv_depths(),
    m_cv_min_ds(),
    m_cv_max_ds(),
    m_cv_xyzs(),
    m_cv_uvs(),
    m_cv_valids(),
    bucket(false),
    num_neighbours(10),
    m_poseoffset(),
    m_cb_width(7),
    m_cb_height(5),
    m_cb_points_local(),
    m_running(true),
    m_sps_front(0),
    m_sps_back(0),
    m_sps(0),
    m_errors(0),
    m_start_texture_unit(0),
    m_mutex(new boost::mutex),
    m_frame_count(0),
    do_swap(false),
    calib_mode(false)
  {
    std::ifstream iff("poseoffset", std::ifstream::binary);
    iff.read((char*) m_poseoffset.data(), sizeof(gloost::Matrix));
    iff.close();
    std::cerr << "loaded pose offset " << m_poseoffset << std::endl;


    m_sps_front = new std::vector<std::vector<struct samplePoint> >;
    m_sps_back = new std::vector<std::vector<struct samplePoint> >;
    m_sps = new std::vector<std::vector<struct samplePoint> >;
    m_errors = new std::vector<std::vector<struct uv> >;
    

    for(unsigned i = 0; i < calibs.size(); ++i){
    	std::string basefile = calibs[i]->_filePath;
    	basefile.replace( basefile.end() - 3, basefile.end(), "");
    	m_cv_xyz_filenames.push_back(basefile + "cv_xyz");
    	m_cv_uv_filenames.push_back(basefile + "cv_uv");
    	m_sps_front->push_back(std::vector<struct samplePoint>() );
    	m_sps_back->push_back(std::vector<struct samplePoint>() );
    	m_sps->push_back(std::vector<struct samplePoint>() );
    	m_errors->push_back(std::vector<struct uv>() );
    }

    // configure local 3D Points on chessboard here:
    for(unsigned y = 0; y < m_cb_height; ++y){
    	for(unsigned x = 0; x < m_cb_width; ++x){
    	  m_cb_points_local.push_back(gloost::Point3(y * 0.075, x * 0.075,0.0));
    	}
    }
  }

  /*virtual*/
  CalibVolume::~CalibVolume(){
    for(unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i){
      delete [] m_cv_xyzs[i];
      delete [] m_cv_uvs[i];
      delete [] m_cv_valids[i];
      glDeleteTextures(1, &(m_cv_xyz_ids[i]));
      glDeleteTextures(1, &(m_cv_uv_ids[i]));
    }
  }

  void
  CalibVolume::prepareOffsets(){

    for(unsigned i = 0; i < (*m_sps)[0].size(); ++i){
      const unsigned cv_width = m_cv_widths[0];
      const unsigned cv_height = m_cv_heights[0];
      const unsigned cv_depth = m_cv_depths[0];
      const float x = cv_width *  ( (*m_sps)[0][i].tex_depth.u)/ m_calibs[0]->getWidth();
      const float y = cv_height *  ( (*m_sps)[0][i].tex_depth.v)/ m_calibs[0]->getHeight();
      const float z = cv_depth * (  (*m_sps)[0][i].depth - m_cv_min_ds[0])/(m_cv_max_ds[0] - m_cv_min_ds[0]);

      xyz pos = getTrilinear(m_cv_xyzs[0], cv_width, cv_height, cv_depth, x , y , z );
      uv  tex = getTrilinear(m_cv_uvs[0], cv_width, cv_height, cv_depth, x , y , z );
      
      
      (*m_sps)[0][i].pos_offset.x = (*m_sps)[0][i].pos_real[0] - pos.x;
      (*m_sps)[0][i].pos_offset.y = (*m_sps)[0][i].pos_real[1] - pos.y;
      (*m_sps)[0][i].pos_offset.z = (*m_sps)[0][i].pos_real[2] - pos.z;
      
      (*m_sps)[0][i].tex_offset.u = (*m_sps)[0][i].tex_color.u/m_calibs[0]->getWidthC() - tex.u;
      (*m_sps)[0][i].tex_offset.v = (*m_sps)[0][i].tex_color.v/m_calibs[0]->getHeightC() - tex.v;

      (*m_errors)[0][i].u = gloost::Vector3((*m_sps)[0][i].pos_offset.x,(*m_sps)[0][i].pos_offset.y,(*m_sps)[0][i].pos_offset.z).length();
      (*m_errors)[0][i].v = gloost::Vector3((*m_sps)[0][i].tex_offset.u * m_calibs[0]->getWidthC(),(*m_sps)[0][i].tex_offset.v * m_calibs[0]->getHeightC(),0.0).length();
    }
  }

  void
  CalibVolume::drawSamplePoints(){

    {
      glPushAttrib(GL_ALL_ATTRIB_BITS);
      glDisable(GL_DEPTH_TEST);

      glPointSize(4.0);
      glColor3f(1.0,0.0,1.0);      
      glBegin(GL_POINTS);
      for(unsigned i = 0; i < m_sps->size(); ++i){
	for(unsigned j = 0; j < (*m_sps)[i].size(); ++j){
	  gloost::Point3 sps = (*m_sps)[i][j].pos_real;
	  glVertex3fv(sps.data());
	}
      }
      glEnd();

      glPopAttrib();
    }
  }

  bool
  CalibVolume::reload(){
    
    for(unsigned i = 0; i < m_cv_xyzs.size(); ++i){
      if(0 != m_cv_xyzs[i]){
	delete [] m_cv_xyzs[i];
	delete [] m_cv_uvs[i];
	delete [] m_cv_valids[i];
	glDeleteTextures(1, &(m_cv_xyz_ids[i]));
	glDeleteTextures(1, &(m_cv_uv_ids[i]));
      }
    }
    m_cv_xyz_ids.clear();
    m_cv_uv_ids.clear();
    m_cv_widths.clear();
    m_cv_heights.clear();
    m_cv_depths.clear();
    m_cv_min_ds.clear();
    m_cv_max_ds.clear();
    m_cv_xyzs.clear();
    m_cv_uvs.clear();
    m_cv_valids.clear();



    for(unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i){
      
      unsigned cv_width;
      unsigned cv_height;
      unsigned cv_depth;
      float    cv_min_d;
      float    cv_max_d;
      unsigned cv_xyz_id;
      unsigned cv_uv_id;

      std::cerr << "loading " << m_cv_xyz_filenames[i] << std::endl;
      FILE* f_xyz = fopen( m_cv_xyz_filenames[i].c_str(), "rb");
      fread(&cv_width, sizeof(unsigned), 1, f_xyz);
      fread(&cv_height, sizeof(unsigned), 1, f_xyz);
      fread(&cv_depth, sizeof(unsigned), 1, f_xyz);
      fread(&cv_min_d, sizeof(float), 1, f_xyz);
      fread(&cv_max_d, sizeof(float), 1, f_xyz);
      xyz* cv_xyz = new xyz[cv_width * cv_height * cv_depth];
      fread(cv_xyz, sizeof(xyz), cv_width * cv_height * cv_depth, f_xyz);
      fclose(f_xyz);
    
      std::cerr << "loading " << m_cv_uv_filenames[i] << std::endl;
      FILE* f = fopen( m_cv_uv_filenames[i].c_str(), "rb");
      fread(&cv_width, sizeof(unsigned), 1, f);
      fread(&cv_height, sizeof(unsigned), 1, f);
      fread(&cv_depth, sizeof(unsigned), 1, f);
      fread(&cv_min_d, sizeof(float), 1, f);
      fread(&cv_max_d, sizeof(float), 1, f);
      uv*  cv_uv  = new uv [cv_width * cv_height * cv_depth];
      fread(cv_uv, sizeof(uv), cv_width * cv_height * cv_depth, f);
      fclose(f);
      
      std::cerr << "generating textures" << std::endl;
      glGenTextures(1, &cv_xyz_id);
      
      glBindTexture(GL_TEXTURE_3D, cv_xyz_id);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
      glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB32F, cv_width, cv_height, cv_depth, 0, GL_RGB, GL_FLOAT, (byte*) cv_xyz);


      glGenTextures(1, &cv_uv_id);
      glBindTexture(GL_TEXTURE_3D, cv_uv_id);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
      glTexImage3D(GL_TEXTURE_3D, 0, GL_RG32F, cv_width, cv_height, cv_depth, 0, GL_RG, GL_FLOAT, (byte*) cv_uv);

      std::cerr << "done generating textures" << std::endl;


      m_cv_xyz_ids.push_back(cv_xyz_id);
      m_cv_uv_ids.push_back(cv_uv_id);
      m_cv_widths.push_back(cv_width);
      m_cv_heights.push_back(cv_height);
      m_cv_depths.push_back(cv_depth);
      m_cv_min_ds.push_back(cv_min_d);
      m_cv_max_ds.push_back(cv_max_d);
      m_cv_xyzs.push_back(cv_xyz);
      m_cv_uvs.push_back(cv_uv);
      bool*  cv_valid  = new bool [cv_width * cv_height * cv_depth];
      m_cv_valids.push_back(cv_valid);
    }

    std::cerr << "reload done" << std::endl;

    return true;
  }

  void
  CalibVolume::bindToTextureUnits(unsigned start_texture_unit) {
    for(unsigned layer = 0; layer < m_calibs.size(); ++layer){
      glActiveTexture(GL_TEXTURE0 + start_texture_unit + layer * 2);
      glBindTexture(GL_TEXTURE_3D, m_cv_xyz_ids[layer]);
      glActiveTexture(GL_TEXTURE0 + start_texture_unit + layer * 2 + 1);
      glBindTexture(GL_TEXTURE_3D, m_cv_uv_ids[layer]);
    }
    glActiveTexture(GL_TEXTURE0);
    m_start_texture_unit = start_texture_unit;    
  }

  unsigned CalibVolume::getStartTextureUnit() const {
    return m_start_texture_unit;
  }

  void
  CalibVolume::save(){

    for(unsigned i = 0; i < m_cv_xyz_filenames.size(); ++i){

      std::cerr << "CalibVolume::save() saving: " << m_cv_xyz_filenames[i] << " and " << m_cv_uv_filenames[i] << std::endl;      
      
      FILE* f_xyz = fopen( m_cv_xyz_filenames[i].c_str(), "wb");
      fwrite(&(m_cv_widths[i]), sizeof(unsigned), 1, f_xyz);
      fwrite(&(m_cv_heights[i]), sizeof(unsigned), 1, f_xyz);
      fwrite(&(m_cv_depths[i]), sizeof(unsigned), 1, f_xyz);
      fwrite(&(m_cv_min_ds[i]), sizeof(float), 1, f_xyz);
      fwrite(&(m_cv_max_ds[i]), sizeof(float), 1, f_xyz);
      fwrite((m_cv_xyzs[i]), sizeof(xyz), m_cv_widths[i] * m_cv_heights[i] * m_cv_depths[i], f_xyz);
      fclose(f_xyz);
      
      FILE* f = fopen( m_cv_uv_filenames[i].c_str(), "wb");
      fwrite(&(m_cv_widths[i]), sizeof(unsigned), 1, f);
      fwrite(&(m_cv_heights[i]), sizeof(unsigned), 1, f);
      fwrite(&(m_cv_depths[i]), sizeof(unsigned), 1, f);
      fwrite(&(m_cv_min_ds[i]), sizeof(float), 1, f);
      fwrite(&(m_cv_max_ds[i]), sizeof(float), 1, f);
      fwrite((m_cv_uvs[i]), sizeof(uv), m_cv_widths[i] * m_cv_heights[i] * m_cv_depths[i], f);
      fclose(f);
    }
  }

  void
  CalibVolume::saveSamplePoints(const char* bfname, unsigned stride){

    std::vector<samplePoint> samples_c;
    std::vector<samplePoint> samples_r;

    bool fill_c = false;
    unsigned counter = 0;
    // fill from (*m_sps)[0] regarding stride into _c and _r
    for(unsigned s = 0; s < (*m_sps)[0].size(); ++s){

      // toggle counter
      if(counter % stride == 0){
	fill_c = !fill_c;
      }
      //fill_c = true;
      if(fill_c){
	samples_c.push_back((*m_sps)[0][s]);
      }
      else{
	samples_r.push_back((*m_sps)[0][s]);
      }

      ++counter;
    }

    std::cerr << "(*m_sps)[0].size(): " << (*m_sps)[0].size() << std::endl;
    std::cerr << "samples_c.size(): " << samples_c.size() << std::endl;
    std::cerr << "samples_r.size(): " << samples_r.size() << std::endl;

    // calculate 3D bbx of samples_c in volume coordinates
    gloost::Point3 min_bbx(std::numeric_limits<float>::max(),std::numeric_limits<float>::max(),std::numeric_limits<float>::max());
    gloost::Point3 max_bbx(std::numeric_limits<float>::min(),std::numeric_limits<float>::min(),std::numeric_limits<float>::min());
    for(unsigned s = 0; s < samples_c.size(); ++s){

      const float s_x = m_cv_widths[0] *  ( samples_c[s].tex_depth.u)/ m_calibs[0]->getWidth();
      const float s_y = m_cv_heights[0] *  ( samples_c[s].tex_depth.v)/ m_calibs[0]->getHeight();
      const float s_z = m_cv_depths[0] * (  samples_c[s].depth - m_cv_min_ds[0])/(m_cv_max_ds[0] - m_cv_min_ds[0]);

      min_bbx[0] = std::min(min_bbx[0],s_x);
      min_bbx[1] = std::min(min_bbx[1],s_y);
      min_bbx[2] = std::min(min_bbx[2],s_z);

      max_bbx[0] = std::max(max_bbx[0],s_x);
      max_bbx[1] = std::max(max_bbx[1],s_y);
      max_bbx[2] = std::max(max_bbx[2],s_z);
    }

    std::cerr << "min_bbx: " << min_bbx << std::endl;
    std::cerr << "max_bbx: " << max_bbx << std::endl;


    // save samples_c, samples_r and samples_bbx to binary file using fwrite?!
    FILE* s_c = fopen( (std::string(bfname) + std::string("_c")).c_str(), "wb");
    const unsigned num_c(samples_c.size());
    fwrite(&num_c, sizeof(unsigned), 1, s_c);
    fwrite(&(samples_c.front()), sizeof(samplePoint), num_c, s_c);
    fclose(s_c);

    FILE* s_r = fopen( (std::string(bfname) + std::string("_r")).c_str(), "wb");
    const unsigned num_r(samples_r.size());
    fwrite(&num_r, sizeof(unsigned), 1, s_r);
    fwrite(&(samples_r.front()), sizeof(samplePoint), num_r, s_r);
    fclose(s_r);


    FILE* s_bbx = fopen( (std::string(bfname) + std::string("_bbx")).c_str(), "wb");
    fwrite(min_bbx.data(), sizeof(gloost::Point3), 1, s_bbx);
    fwrite(max_bbx.data(), sizeof(gloost::Point3), 1, s_bbx);
    fclose(s_bbx);

  }

  void
  CalibVolume::loadSamplePoints(const char* bfname){

    (*m_sps)[0].clear();
    (*m_errors)[0].clear();

    
    FILE* s_c = fopen( (std::string(bfname) + std::string("_c")).c_str(), "rb");
    unsigned num_c;
    fread(&num_c, sizeof(unsigned), 1, s_c);

    (*m_sps)[0].resize(num_c);
    (*m_errors)[0].resize(num_c);

    fread(&((*m_sps)[0].front()), sizeof(samplePoint), num_c, s_c);
    fclose(s_c);
  }

  void
  CalibVolume::calcStats(){
		
    std::vector<double> err_3D;
    std::vector<double> err_2D;
    for(auto const& e : (*m_errors)[0]){
      err_3D.push_back(e.u);
      err_2D.push_back(e.v);
    }
		
    {
      const double sum = std::accumulate(err_3D.begin(), err_3D.end(), 0.0);
      const double mean = sum / err_3D.size();
      
      const double sq_sum = std::inner_product(err_3D.begin(), err_3D.end(), err_3D.begin(), 0.0);
      const double stdev = std::sqrt(sq_sum / err_3D.size() - mean * mean);
      
      std::cout << "mean3D: " << mean << " stdev: " << stdev  << std::endl;
    }
    
    {
      const double sum = std::accumulate(err_2D.begin(), err_2D.end(), 0.0);
      const double mean = sum / err_2D.size();
      
      const double sq_sum = std::inner_product(err_2D.begin(), err_2D.end(), err_2D.begin(), 0.0);
      const double stdev = std::sqrt(sq_sum / err_2D.size() - mean * mean);
      
      std::cout << "mean2D: " << mean << " stdev: " << stdev  << std::endl;
    }	
  }

  void
  CalibVolume::evaluate(const char* bfname){

    std::vector<samplePoint> samples_r;
    
    FILE* s_r = fopen( (std::string(bfname) + std::string("_r")).c_str(), "rb");
    unsigned num_r;
    fread(&num_r, sizeof(unsigned), 1, s_r);

    samples_r.resize(num_r);

    fread(&(samples_r.front()), sizeof(samplePoint), num_r, s_r);
    fclose(s_r);


    std::vector<samplePoint> samples_c;
    
    FILE* s_c = fopen( (std::string(bfname) + std::string("_c")).c_str(), "rb");
    unsigned num_c;
    fread(&num_c, sizeof(unsigned), 1, s_c);

    samples_c.resize(num_c);

    fread(&(samples_c.front()), sizeof(samplePoint), num_c, s_c);
    fclose(s_c);



    gloost::Point3 min_bbx;
    gloost::Point3 max_bbx;
    FILE* s_bbx = fopen( (std::string(bfname) + std::string("_bbx")).c_str(), "rb");
    fread((void *) &min_bbx[0], sizeof(gloost::Point3), 1, s_bbx);
    fread((void *) &max_bbx[0], sizeof(gloost::Point3), 1, s_bbx);
    fclose(s_bbx);


    // sort samples_r according to distance from nearest 5 samples_c

    std::vector<double> err_3D;
    std::vector<double> err_2D;
    
    //std::cout << "Begin dumping errors for CalibVolume" << std::endl;
    for(unsigned j = 0; j < samples_r.size(); ++j){
      
      const unsigned cv_width = m_cv_widths[0];
      const unsigned cv_height = m_cv_heights[0];
      const unsigned cv_depth = m_cv_depths[0];
      const float x = cv_width *  ( samples_r[j].tex_depth.u)/ m_calibs[0]->getWidth();
      const float y = cv_height *  ( samples_r[j].tex_depth.v)/ m_calibs[0]->getHeight();
      float z = cv_depth * (  samples_r[j].depth - m_cv_min_ds[0])/(m_cv_max_ds[0] - m_cv_min_ds[0]);
      xyz pos = getTrilinear(m_cv_xyzs[0], cv_width, cv_height, cv_depth, x , y , z );
      uv  tex = getTrilinear(m_cv_uvs[0], cv_width, cv_height, cv_depth, x , y , z );
	
	
      samples_r[j].pos_offset.x = samples_r[j].pos_real[0] - pos.x;
      samples_r[j].pos_offset.y = samples_r[j].pos_real[1] - pos.y;
      samples_r[j].pos_offset.z = samples_r[j].pos_real[2] - pos.z;
      
      samples_r[j].tex_offset.u = samples_r[j].tex_color.u/m_calibs[0]->getWidthC() - tex.u;
      samples_r[j].tex_offset.v = samples_r[j].tex_color.v/m_calibs[0]->getHeightC() - tex.v;
      
      uv err;	  
      err.u = gloost::Vector3(samples_r[j].pos_offset.x,samples_r[j].pos_offset.y,samples_r[j].pos_offset.z).length();
      err.v = gloost::Vector3(samples_r[j].tex_offset.u * m_calibs[0]->getWidthC(),samples_r[j].tex_offset.v * m_calibs[0]->getHeightC(),0.0).length();
      std::cout <<  err.u << ";" << err.v << std::endl; 
      
      err_3D.push_back(err.u);
      err_2D.push_back(err.v);

    }
    //std::cout << "End dumping errors for CalibVolume" << std::endl;

    {
      const double sum = std::accumulate(err_3D.begin(), err_3D.end(), 0.0);
      const double mean = sum / err_3D.size();
    
      const double sq_sum = std::inner_product(err_3D.begin(), err_3D.end(), err_3D.begin(), 0.0);
      const double stdev = std::sqrt(sq_sum / err_3D.size() - mean * mean);

      std::cout << "mean3D: " << mean << " stdev: " << stdev  << std::endl;
    }

    {
      const double sum = std::accumulate(err_2D.begin(), err_2D.end(), 0.0);
      const double mean = sum / err_2D.size();
    
      const double sq_sum = std::inner_product(err_2D.begin(), err_2D.end(), err_2D.begin(), 0.0);
      const double stdev = std::sqrt(sq_sum / err_2D.size() - mean * mean);

      std::cout << "mean2D: " << mean << " stdev: " << stdev  << std::endl;
    }


  }

  float
  CalibVolume::getBilinear(float* data, unsigned width, unsigned height, float x, float y){


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

    // calculate indices to access data
    const unsigned idmax = width * height;
    unsigned id00 = std::min( ya * width + xa  , idmax);
    unsigned id10 = std::min( ya * width + xb  , idmax);
    unsigned id01 = std::min( yb * width + xa  , idmax);
    unsigned id11 = std::min( yb * width + xb  , idmax);

    // 1. interpolate between x direction;
    float tmp_ya = w_xa * data[id00] + w_xb * data[id10];
    float tmp_yb = w_xa * data[id01] + w_xb * data[id11];
    // 2. interpolate between y direction;
    float result = w_ya * tmp_ya + w_yb * tmp_yb;
    return result;
  }



  xyz
  CalibVolume::getTrilinear(xyz* data, unsigned width, unsigned height, unsigned depth, float x, float y, float z){

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
  CalibVolume::getTrilinear(uv* data, unsigned width, unsigned height, unsigned depth, float x, float y, float z){

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



  float
  CalibVolume::gauss(float x, float sigma, float mean){
    return (1.0f/(sigma*sqrt(2.0f * M_PI))) * exp( -0.5f * ((x-mean)/sigma) * ((x-mean)/sigma) );
  }




  void
  CalibVolume::dumpSamplePoints(){

    
    std::ofstream ofs;
    ofs.open ("overlapping_test.txt", std::ofstream::out | std::ofstream::app);
    
    

    std::cerr << "CalibVolume::dumpSamplePoints()" << std::endl;

    for(unsigned i = 0; i < m_errors->size(); ++i){
      std::cout << "Begin dumping errors for CalibVolume " << i << std::endl;
      for(unsigned j = 0; j < (*m_errors)[i].size(); ++j){





	const unsigned cv_width = m_cv_widths[i];
	const unsigned cv_height = m_cv_heights[i];
	const unsigned cv_depth = m_cv_depths[i];
	const float x = cv_width *  ( (*m_sps)[i][j].tex_depth.u)/ m_calibs[i]->getWidth();
	const float y = cv_height *  ( (*m_sps)[i][j].tex_depth.v)/ m_calibs[i]->getHeight();
	float z = cv_depth * (  (*m_sps)[i][j].depth - m_cv_min_ds[i])/(m_cv_max_ds[i] - m_cv_min_ds[i]);
	xyz pos = getTrilinear(m_cv_xyzs[i], cv_width, cv_height, cv_depth, x , y , z );
	uv  tex = getTrilinear(m_cv_uvs[i], cv_width, cv_height, cv_depth, x , y , z );
	
	std::cerr << j << " pos3D: " << pos.x << " " << pos.y << " " << pos.z << std::endl;
	ofs << j << " pos3D: " << pos.x << " " << pos.y << " " << pos.z << std::endl;
	(*m_sps)[i][j].pos_offset.x = (*m_sps)[i][j].pos_real[0] - pos.x;
	(*m_sps)[i][j].pos_offset.y = (*m_sps)[i][j].pos_real[1] - pos.y;
	(*m_sps)[i][j].pos_offset.z = (*m_sps)[i][j].pos_real[2] - pos.z;
	
	(*m_sps)[i][j].tex_offset.u = (*m_sps)[i][j].tex_color.u/m_calibs[i]->getWidthC() - tex.u;
	(*m_sps)[i][j].tex_offset.v = (*m_sps)[i][j].tex_color.v/m_calibs[i]->getHeightC() - tex.v;

	uv err;	  
	err.u = gloost::Vector3((*m_sps)[i][j].pos_offset.x,(*m_sps)[i][j].pos_offset.y,(*m_sps)[i][j].pos_offset.z).length();
	err.v = gloost::Vector3((*m_sps)[i][j].tex_offset.u * m_calibs[i]->getWidthC(),(*m_sps)[i][j].tex_offset.v * m_calibs[i]->getHeightC(),0.0).length();

	
	std::cerr << j << " error3D: " << (*m_errors)[i][j].u << " " << err.u << " error2D: " << (*m_errors)[i][j].v << " " <<  err.v << std::endl; 
	ofs << j << " error3D: " << (*m_errors)[i][j].u << " " << err.u << " error2D: " << (*m_errors)[i][j].v << " " <<  err.v << std::endl; 
	//std::cout <<  (*m_errors)[i][j].u << ";" << err.u << ";" << (*m_errors)[i][j].v << ";" <<  err.v << std::endl; 
	

      }
      std::cout << "End dumping errors for CalibVolume " << i << std::endl;
    }

    ofs.close();

  }

  void
  CalibVolume::clearSamples(){
    for(unsigned i = 0; i < m_sps->size(); ++i){
      (*m_sps)[i].clear();
      (*m_errors)[i].clear();
    }
    reload();
  }


  void
  CalibVolume::applySamples(){


    for(unsigned i = 0; i < m_sps->size(); ++i){


      const unsigned cv_width = m_cv_widths[i];
      const unsigned cv_height = m_cv_heights[i];
      const unsigned cv_depth = m_cv_depths[i];

      std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! applying " << (*m_sps)[i/*kinect*/].size() << " for volume" << std::endl;

      boost::thread_group threadGroup;
      const unsigned numthreads = 16;
      for (unsigned tid = 0; tid < numthreads; ++tid){
	if(bucket)
	  threadGroup.create_thread(boost::bind(&CalibVolume::applySamplesByThreadBucket,this,cv_width, cv_height, cv_depth, i, tid, numthreads));
	else
	  threadGroup.create_thread(boost::bind(&CalibVolume::applySamplesByThreadIDW,this,cv_width, cv_height, cv_depth, i, tid, numthreads));
      }
      threadGroup.join_all();


      glBindTexture(GL_TEXTURE_3D, m_cv_xyz_ids[i]);
      glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB32F, cv_width, cv_height, cv_depth, 0, GL_RGB, GL_FLOAT, (byte*) m_cv_xyzs[i]);
      
      glBindTexture(GL_TEXTURE_3D, m_cv_uv_ids[i]);
      glTexImage3D(GL_TEXTURE_3D, 0, GL_RG32F, cv_width, cv_height, cv_depth, 0, GL_RG, GL_FLOAT, (byte*) m_cv_uvs[i]);
      
    }

    //save();

  }


  void
  CalibVolume::applySamplesNNI(bool doapply){


    std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! applying " << (*m_sps)[0].size() << " for volume" << std::endl;

    std::vector<nniSample> nnisamples;

    for(unsigned s = 0; s < (*m_sps)[0].size(); ++s){
      

      nniSample nnis;
    
      nnis.s_pos_off = (*m_sps)[0][s/*sample point*/].pos_offset;
      nnis.s_tex_off  = (*m_sps)[0][s/*sample point*/].tex_offset;
      // calculate distance from volume pos to sample pos
      
      nnis.s_pos.x = m_cv_widths[0] *  ( (*m_sps)[0][s].tex_depth.u)/ m_calibs[0]->getWidth();
      nnis.s_pos.y = m_cv_heights[0] *  ( (*m_sps)[0][s].tex_depth.v)/ m_calibs[0]->getHeight();
      nnis.s_pos.z = m_cv_depths[0] * (  (*m_sps)[0][s].depth - m_cv_min_ds[0])/(m_cv_max_ds[0] - m_cv_min_ds[0]);
     
      nnisamples.push_back(nnis);
    }

    // shuffle
    std::shuffle(std::begin(nnisamples), std::end(nnisamples), std::default_random_engine());

    NaturalNeighbourInterpolator nni(nnisamples);


    const unsigned cv_width = m_cv_widths[0];
    const unsigned cv_height = m_cv_heights[0];
    const unsigned cv_depth = m_cv_depths[0];
  
    for(unsigned z = 0; z < cv_depth; ++z){
      //std::cerr << "tid: having " << ++having << " from " << cv_depth / numthreads <<  std::endl; 
      for(unsigned y = 0; y < cv_height; ++y){
	for(unsigned x = 0; x < cv_width; ++x){
	  
	  const unsigned cv_index = (z * cv_width * cv_height) + (y * cv_width) + x;

	  nniSample ipolant;
	  ipolant.s_pos.x = x;
	  ipolant.s_pos.y = y;
	  ipolant.s_pos.z = z;
	  
	  ipolant.s_pos_off.x = 0.0;
	  ipolant.s_pos_off.y = 0.0;
	  ipolant.s_pos_off.z = 0.0;

	  ipolant.s_tex_off.u = 0.0;
	  ipolant.s_tex_off.v = 0.0;

	  bool s_valid = nni.interpolate(ipolant);
	  
	  xyz pos3D = m_cv_xyzs[0][cv_index];
	  uv tex = m_cv_uvs[0][cv_index];

	  xyz pos3D_new = pos3D + ipolant.s_pos_off;
	  uv tex_new = tex + ipolant.s_tex_off;

	  if(doapply){
	    m_cv_xyzs[0][cv_index] = pos3D_new;
	    m_cv_uvs[0][cv_index] = tex_new;
	  }
	  m_cv_valids[0][cv_index] = s_valid;



	}
      }
    }




  }




  void
  CalibVolume::applySamplesNNIAbsolute(bool doapply){
    std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! applying " << (*m_sps)[0].size() << " for volume" << std::endl;

    std::vector<nniSample> nnisamples;

    for(unsigned s = 0; s < (*m_sps)[0].size(); ++s){
      nniSample nnis;
      xyz pos_real;
      pos_real.x = (*m_sps)[0][s/*sample point*/].pos_real[0];
      pos_real.y = (*m_sps)[0][s/*sample point*/].pos_real[1];
      pos_real.z = (*m_sps)[0][s/*sample point*/].pos_real[2];

      nnis.s_pos_off = pos_real;
      nnis.s_tex_off  = (*m_sps)[0][s/*sample point*/].tex_color;
      // calculate distance from volume pos to sample pos
      
      nnis.s_pos.x = m_cv_widths[0] *  ( (*m_sps)[0][s].tex_depth.u)/ m_calibs[0]->getWidth();
      nnis.s_pos.y = m_cv_heights[0] *  ( (*m_sps)[0][s].tex_depth.v)/ m_calibs[0]->getHeight();
      nnis.s_pos.z = m_cv_depths[0] * (  (*m_sps)[0][s].depth - m_cv_min_ds[0])/(m_cv_max_ds[0] - m_cv_min_ds[0]);
     
      nnisamples.push_back(nnis);
    }

    // shuffle
    std::shuffle(std::begin(nnisamples), std::end(nnisamples), std::default_random_engine());

    NaturalNeighbourInterpolator nni(nnisamples);

    const unsigned cv_width = m_cv_widths[0];
    const unsigned cv_height = m_cv_heights[0];
    const unsigned cv_depth = m_cv_depths[0];
  
    for(unsigned z = 0; z < cv_depth; ++z){
      //std::cerr << "tid: having " << ++having << " from " << cv_depth / numthreads <<  std::endl; 
      for(unsigned y = 0; y < cv_height; ++y){
      	for(unsigned x = 0; x < cv_width; ++x){
      	  
      	  const unsigned cv_index = (z * cv_width * cv_height) + (y * cv_width) + x;

      	  nniSample ipolant;
      	  ipolant.s_pos.x = x;
      	  ipolant.s_pos.y = y;
      	  ipolant.s_pos.z = z;
      	  
      	  ipolant.s_pos_off.x = 0.0;
      	  ipolant.s_pos_off.y = 0.0;
      	  ipolant.s_pos_off.z = 0.0;

      	  ipolant.s_tex_off.u = 0.0;
      	  ipolant.s_tex_off.v = 0.0;

      	  bool s_valid = nni.interpolate(ipolant);

      	  if(doapply){
      	    m_cv_xyzs[0][cv_index] = ipolant.s_pos_off;
      	    m_cv_uvs[0][cv_index] = ipolant.s_tex_off;
      	  }
      	  m_cv_valids[0][cv_index] = s_valid;
      	}
      }
    }
  }

  void
  CalibVolume::applySamplesByThreadIDW(const unsigned cv_width, const unsigned cv_height, const unsigned cv_depth, const unsigned i, const unsigned tid, const unsigned numthreads){

    gloost::Vector3 diameter(cv_width, cv_height, cv_depth);
    const float max_influence_dist = diameter.length();
    const float sigma = max_influence_dist * 1.0/3.3;
    const float mean = 0;
    const float norm = 1.0f/gauss(0.0f, sigma, mean);
    
    unsigned having = 0;
  
    for(unsigned z = tid; z < cv_depth; z += numthreads){
      std::cerr << "tid: having " << ++having << " from " << cv_depth / numthreads <<  std::endl; 
      for(unsigned y = 0; y < cv_height; ++y){
	for(unsigned x = 0; x < cv_width; ++x){
	  
	  const unsigned cv_index = (z * cv_width * cv_height) + (y * cv_width) + x;
	  
	  gloost::Point3 vol_pos(x,y,z);
	  
	  xyz pos3D = m_cv_xyzs[i][cv_index];
	  uv tex = m_cv_uvs[i][cv_index];
	  
	  double weight_d = 0.0;
	  xyz_d pos_offset;
	  pos_offset.x = 0.0;pos_offset.y = 0.0;pos_offset.z = 0.0;
	  uv_d tex_offset;
	  tex_offset.u = 0.0;tex_offset.v = 0.0;
	  

	  // CandidateSample(const float w, const xyz& p_off, const uv& t_off)
	  std::vector<CandidateSample> c_000;


	  for(unsigned s = 0; s < (*m_sps)[i/*kinect*/].size(); ++s){
	    
	    const xyz s_pos_off = (*m_sps)[i/*kinect*/][s/*sample point*/].pos_offset;
	    const uv s_tex_off  = (*m_sps)[i/*kinect*/][s/*sample point*/].tex_offset;
	    // calculate distance from volume pos to sample pos
	    
	    const float s_x = cv_width *  ( (*m_sps)[i][s].tex_depth.u)/ m_calibs[i]->getWidth();
	    const float s_y = cv_height *  ( (*m_sps)[i][s].tex_depth.v)/ m_calibs[i]->getHeight();
	    const float s_z = cv_depth * (  (*m_sps)[i][s].depth - m_cv_min_ds[i])/(m_cv_max_ds[i] - m_cv_min_ds[i]);
	    gloost::Point3 s_pos(s_x,s_y,s_z);
	    const float influence_dist = std::min(max_influence_dist, (vol_pos - s_pos).length());
	    const float s_weight = gauss(influence_dist, sigma, mean) * norm;

	    CandidateSample cs(s_weight, s_pos_off, s_tex_off);
	    c_000.push_back(cs);

	    /*
	    weight_d += s_weight;
	    pos_offset = pos_offset + s_weight * s_pos_off;
	    tex_offset = tex_offset + s_weight * s_tex_off;
	    */
	  }
	  // sort buckets
	  std::sort(c_000.begin(), c_000.end());
	  
	  // check sorting
#if 0
	  for(unsigned cs_idx = 0; cs_idx < c_000.size() ; ++cs_idx){
	    std::cerr << cs_idx << " " << c_000[cs_idx].weight << std::endl;
	  }
#endif


	  const unsigned last_cs = num_neighbours;
	  for(unsigned cs_idx = 0; cs_idx < c_000.size() && cs_idx < last_cs; ++cs_idx){
	    weight_d += c_000[cs_idx].weight;
	    pos_offset = pos_offset + c_000[cs_idx].weight * c_000[cs_idx].pos_off;
	    tex_offset = tex_offset + c_000[cs_idx].weight * c_000[cs_idx].tex_off;
	  }

	  if(weight_d > 0.01){
	    xyz pos_offset_f;
	    pos_offset_f.x = pos_offset.x/weight_d;
	    pos_offset_f.y = pos_offset.y/weight_d;
	    pos_offset_f.z = pos_offset.z/weight_d;
	    uv tex_offset_f;
	    tex_offset_f.u = tex_offset.u/weight_d;
	    tex_offset_f.v = tex_offset.v/weight_d;
	    xyz pos3D_new = pos3D + pos_offset_f;
	    uv tex_new = tex + tex_offset_f;
	    m_cv_xyzs[i][cv_index] = pos3D_new;
	    m_cv_uvs[i][cv_index] = tex_new;
	    //m_cv_valids[0][cv_index] = true;
	  }
	  else{
	    m_cv_valids[0][cv_index] = false;
	  }



	}
      }
    }

#if 0

    gloost::Vector3 diameter(cv_width, cv_height, cv_depth);
    const float max_influence_dist = diameter.length();
    const float sigma = max_influence_dist * 1.0/3.3;
    const float mean = 0;
    const float norm = 1.0f/gauss(0.0f, sigma, mean);
      
    for(unsigned z = tid; z < cv_depth; z += numthreads){
      for(unsigned y = 0; y < cv_height; ++y){
	for(unsigned x = 0; x < cv_width; ++x){
	  
	  const unsigned cv_index = (z * cv_width * cv_height) + (y * cv_width) + x;
	  
	  gloost::Point3 vol_pos(x,y,z);
	  
	  xyz pos3D = m_cv_xyzs[i][cv_index];
	  uv tex = m_cv_uvs[i][cv_index];
	  
	  double weight_d = 0.0;
	  xyz_d pos_offset;
	  pos_offset.x = 0.0;pos_offset.y = 0.0;pos_offset.z = 0.0;
	  uv_d tex_offset;
	  tex_offset.u = 0.0;tex_offset.v = 0.0;
	  
	  for(unsigned s = 0; s < (*m_sps)[i/*kinect*/].size(); ++s){
	    
	    const xyz s_pos_off = (*m_sps)[i/*kinect*/][s/*sample point*/].pos_offset;
	    const uv s_tex_off  = (*m_sps)[i/*kinect*/][s/*sample point*/].tex_offset;
	    // calculate distance from volume pos to sample pos
	    
	    const float s_x = cv_width *  ( (*m_sps)[i][s].tex_depth.u)/ m_calibs[i]->getWidth();
	    const float s_y = cv_height *  ( (*m_sps)[i][s].tex_depth.v)/ m_calibs[i]->getHeight();
	    const float s_z = cv_depth * (  (*m_sps)[i][s].depth - m_cv_min_ds[i])/(m_cv_max_ds[i] - m_cv_min_ds[i]);
	    gloost::Point3 s_pos(s_x,s_y,s_z);
	    const float influence_dist = std::min(max_influence_dist, (vol_pos - s_pos).length());
	    const float s_weight = gauss(influence_dist, sigma, mean) * norm;
	    
	    weight_d += s_weight;
	    pos_offset = pos_offset + s_weight * s_pos_off;
	    tex_offset = tex_offset + s_weight * s_tex_off;
	  }
	  
	  if(weight_d > 0.01){
	    xyz pos_offset_f;
	    pos_offset_f.x = pos_offset.x/weight_d;
	    pos_offset_f.y = pos_offset.y/weight_d;
	    pos_offset_f.z = pos_offset.z/weight_d;
	    uv tex_offset_f;
	    tex_offset_f.u = tex_offset.u/weight_d;
	    tex_offset_f.v = tex_offset.v/weight_d;
	    xyz pos3D_new = pos3D + pos_offset_f;
	    uv tex_new = tex + tex_offset_f;
	    m_cv_xyzs[i][cv_index] = pos3D_new;
	    m_cv_uvs[i][cv_index] = tex_new;
	    //m_cv_valids[0][cv_index] = true;
	  }
	  else{
	    m_cv_valids[0][cv_index] = false;
	  }


	}
      }
    }
#endif


  }

  void
  CalibVolume::applySamplesByThreadIDWAbsolute(const unsigned cv_width, const unsigned cv_height, const unsigned cv_depth, const unsigned i, const unsigned tid, const unsigned numthreads){

    gloost::Vector3 diameter(cv_width, cv_height, cv_depth);
    const float max_influence_dist = diameter.length();
    const float sigma = max_influence_dist * 1.0/3.3;
    const float mean = 0;
    const float norm = 1.0f/gauss(0.0f, sigma, mean);
    
    for(unsigned z = tid; z < cv_depth; z += numthreads){
      //std::cerr << "tid: having " << ++having << " from " << cv_depth / numthreads <<  std::endl; 
      for(unsigned y = 0; y < cv_height; ++y){
      	for(unsigned x = 0; x < cv_width; ++x){
      	  
      	  const unsigned cv_index = (z * cv_width * cv_height) + (y * cv_width) + x;
      	  
      	  gloost::Point3 vol_pos(x,y,z);
      	  
      	  double weight_d = 0.0;
      	  xyz_d pos_offset;
      	  pos_offset.x = 0.0;pos_offset.y = 0.0;pos_offset.z = 0.0;
      	  uv_d tex_offset;
      	  tex_offset.u = 0.0;tex_offset.v = 0.0;
      	  

      	  // CandidateSample(const float w, const xyz& p_off, const uv& t_off)
      	  std::vector<CandidateSample> c_000;


      	  for(unsigned s = 0; s < (*m_sps)[i/*kinect*/].size(); ++s){
      	    
      	    xyz pos_real;
      	    pos_real.x = (*m_sps)[i/*kinect*/][s/*sample point*/].pos_real[0];
      	    pos_real.y = (*m_sps)[i/*kinect*/][s/*sample point*/].pos_real[1];
      	    pos_real.z = (*m_sps)[i/*kinect*/][s/*sample point*/].pos_real[2];

      	    const xyz s_pos_off = pos_real;
      	    const uv s_tex_off  = (*m_sps)[i/*kinect*/][s/*sample point*/].tex_color;
      	    // calculate distance from volume pos to sample pos
      	    
      	    const float s_x = cv_width *  ( (*m_sps)[i][s].tex_depth.u)/ m_calibs[i]->getWidth();
      	    const float s_y = cv_height *  ( (*m_sps)[i][s].tex_depth.v)/ m_calibs[i]->getHeight();
      	    const float s_z = cv_depth * (  (*m_sps)[i][s].depth - m_cv_min_ds[i])/(m_cv_max_ds[i] - m_cv_min_ds[i]);
      	    gloost::Point3 s_pos(s_x,s_y,s_z);
      	    const float influence_dist = std::min(max_influence_dist, (vol_pos - s_pos).length());
      	    const float s_weight = gauss(influence_dist, sigma, mean) * norm;

      	    CandidateSample cs(s_weight, s_pos_off, s_tex_off);
      	    c_000.push_back(cs);

      	    /*
      	    weight_d += s_weight;
      	    pos_offset = pos_offset + s_weight * s_pos_off;
      	    tex_offset = tex_offset + s_weight * s_tex_off;
      	    */
      	  }
      	  // sort buckets
      	  std::sort(c_000.begin(), c_000.end());

      	  const unsigned last_cs = num_neighbours;
      	  for(unsigned cs_idx = 0; cs_idx < c_000.size() && cs_idx < last_cs; ++cs_idx){
      	    weight_d += c_000[cs_idx].weight;
      	    pos_offset = pos_offset + c_000[cs_idx].weight * c_000[cs_idx].pos_off;
      	    tex_offset = tex_offset + c_000[cs_idx].weight * c_000[cs_idx].tex_off;
      	  }

      	  if(weight_d > 0.01){
      	    xyz pos_offset_f;
      	    pos_offset_f.x = pos_offset.x/weight_d;
      	    pos_offset_f.y = pos_offset.y/weight_d;
      	    pos_offset_f.z = pos_offset.z/weight_d;
      	    uv tex_offset_f;
      	    tex_offset_f.u = tex_offset.u/weight_d;
      	    tex_offset_f.v = tex_offset.v/weight_d;
      	    

      	    m_cv_xyzs[i][cv_index] = pos_offset_f;
      	    m_cv_uvs[i][cv_index] = tex_offset_f;

      	    //m_cv_valids[0][cv_index] = true;
      	  }
      	  else{
      	    m_cv_valids[0][cv_index] = false;
      	  }
      	}
      }
    }
  }

  void
  CalibVolume::applySamplesByThreadBucket(const unsigned cv_width, const unsigned cv_height, const unsigned cv_depth, const unsigned i, const unsigned tid, const unsigned numthreads){

    gloost::Vector3 diameter(cv_width, cv_height, cv_depth);
    const float max_influence_dist = diameter.length();
    const float sigma = max_influence_dist * 1.0/3.3;
    const float mean = 0;
    const float norm = 1.0f/gauss(0.0f, sigma, mean);
    
    for(unsigned z = tid; z < cv_depth; z += numthreads){
      //std::cerr << "tid: having " << ++having << " from " << cv_depth / numthreads <<  std::endl; 
      for(unsigned y = 0; y < cv_height; ++y){
      	for(unsigned x = 0; x < cv_width; ++x){
      	  
      	  const unsigned cv_index = (z * cv_width * cv_height) + (y * cv_width) + x;
      	  
      	  gloost::Point3 vol_pos(x,y,z);
      	  
      	  xyz pos3D = m_cv_xyzs[i][cv_index];
      	  uv tex = m_cv_uvs[i][cv_index];
      	  
      	  double weight_d = 0.0;
      	  xyz_d pos_offset;
      	  pos_offset.x = 0.0;pos_offset.y = 0.0;pos_offset.z = 0.0;
      	  uv_d tex_offset;
      	  tex_offset.u = 0.0;tex_offset.v = 0.0;
      	  
      	  // CandidateSample(const float w, const xyz& p_off, const uv& t_off)
      	  std::vector<CandidateSample> c_000;
      	  std::vector<CandidateSample> c_100;
      	  std::vector<CandidateSample> c_110;
      	  std::vector<CandidateSample> c_010;
      	  std::vector<CandidateSample> c_001;
      	  std::vector<CandidateSample> c_101;
      	  std::vector<CandidateSample> c_111;
      	  std::vector<CandidateSample> c_011;

      	  for(unsigned s = 0; s < (*m_sps)[i/*kinect*/].size(); ++s){
      	    
      	    const xyz s_pos_off = (*m_sps)[i/*kinect*/][s/*sample point*/].pos_offset;
      	    const uv s_tex_off  = (*m_sps)[i/*kinect*/][s/*sample point*/].tex_offset;
      	    // calculate distance from volume pos to sample pos
      	    
      	    const float s_x = cv_width *  ( (*m_sps)[i][s].tex_depth.u)/ m_calibs[i]->getWidth();
      	    const float s_y = cv_height *  ( (*m_sps)[i][s].tex_depth.v)/ m_calibs[i]->getHeight();
      	    const float s_z = cv_depth * (  (*m_sps)[i][s].depth - m_cv_min_ds[i])/(m_cv_max_ds[i] - m_cv_min_ds[i]);
      	    gloost::Point3 s_pos(s_x,s_y,s_z);
      	    const float influence_dist = std::min(max_influence_dist, (vol_pos - s_pos).length());
      	    const float s_weight = gauss(influence_dist, sigma, mean) * norm;

      	    CandidateSample cs(s_weight, s_pos_off, s_tex_off);
      	    if( (s_x <= x) && (s_y <= y) && (s_z <= z) ){
      	      c_000.push_back(cs);
      	    }
      	    else if( (s_x > x) && (s_y < y) && (s_z < z) ){
      	      c_100.push_back(cs);
      	    }
      	    else if( (s_x > x) && (s_y > y) && (s_z < z) ){
      	      c_110.push_back(cs);
      	    }
      	    else if( (s_x > x) && (s_y > y) && (s_z > z) ){
      	      c_111.push_back(cs);
      	    }
      	    else if( (s_x > x) && (s_y < y) && (s_z > z) ){
      	      c_101.push_back(cs);
      	    }
      	    else if( (s_x < x) && (s_y < y) && (s_z > z) ){
      	      c_001.push_back(cs);
      	    }
      	    else if( (s_x < x) && (s_y > y) && (s_z < z) ){
      	      c_010.push_back(cs);
      	    }
      	    else if( (s_x < x) && (s_y > y) && (s_z > z) ){
      	      c_011.push_back(cs);
      	    }
      	    else{
      	      std::cerr << "error sorting into buckets failed" << std::endl;
      	    }


      	    /*
      	    weight_d += s_weight;
      	    pos_offset = pos_offset + s_weight * s_pos_off;
      	    tex_offset = tex_offset + s_weight * s_tex_off;
      	    */
      	  }
      	  // sort buckets
      	  std::sort(c_000.begin(), c_000.end());
      	  std::sort(c_001.begin(), c_001.end());
      	  std::sort(c_010.begin(), c_010.end());
      	  std::sort(c_011.begin(), c_011.end());
      	  std::sort(c_100.begin(), c_100.end());
      	  std::sort(c_101.begin(), c_101.end());
      	  std::sort(c_110.begin(), c_110.end());
      	  std::sort(c_111.begin(), c_111.end());
      	  
      	  // check sorting
      #if 0
      	  for(unsigned cs_idx = 0; cs_idx < c_000.size() ; ++cs_idx){
      	    std::cerr << cs_idx << " " << c_000[cs_idx].weight << std::endl;
      	  }
      #endif

      	  // gather first from each bucket
      	  const unsigned last_cs = 1;
      	  for(unsigned cs_idx = 0; cs_idx < c_000.size() && cs_idx < last_cs; ++cs_idx){
      	    weight_d += c_000[cs_idx].weight;
      	    pos_offset = pos_offset + c_000[cs_idx].weight * c_000[cs_idx].pos_off;
      	    tex_offset = tex_offset + c_000[cs_idx].weight * c_000[cs_idx].tex_off;
      	  }
      	  for(unsigned cs_idx = 0; cs_idx < c_001.size() && cs_idx < last_cs; ++cs_idx){
      	    weight_d += c_001[cs_idx].weight;
      	    pos_offset = pos_offset + c_001[cs_idx].weight * c_001[cs_idx].pos_off;
      	    tex_offset = tex_offset + c_001[cs_idx].weight * c_001[cs_idx].tex_off;
      	  }
      	  for(unsigned cs_idx = 0; cs_idx < c_010.size() && cs_idx < last_cs; ++cs_idx){
      	    weight_d += c_010[cs_idx].weight;
      	    pos_offset = pos_offset + c_010[cs_idx].weight * c_010[cs_idx].pos_off;
      	    tex_offset = tex_offset + c_010[cs_idx].weight * c_010[cs_idx].tex_off;
      	  }
      	  for(unsigned cs_idx = 0; cs_idx < c_011.size() && cs_idx < last_cs; ++cs_idx){
      	    weight_d += c_011[cs_idx].weight;
      	    pos_offset = pos_offset + c_011[cs_idx].weight * c_011[cs_idx].pos_off;
      	    tex_offset = tex_offset + c_011[cs_idx].weight * c_011[cs_idx].tex_off;
      	  }
      	  for(unsigned cs_idx = 0; cs_idx < c_100.size() && cs_idx < last_cs; ++cs_idx){
      	    weight_d += c_100[cs_idx].weight;
      	    pos_offset = pos_offset + c_100[cs_idx].weight * c_100[cs_idx].pos_off;
      	    tex_offset = tex_offset + c_100[cs_idx].weight * c_100[cs_idx].tex_off;
      	  }
      	  for(unsigned cs_idx = 0; cs_idx < c_101.size() && cs_idx < last_cs; ++cs_idx){
      	    weight_d += c_101[cs_idx].weight;
      	    pos_offset = pos_offset + c_101[cs_idx].weight * c_101[cs_idx].pos_off;
      	    tex_offset = tex_offset + c_101[cs_idx].weight * c_101[cs_idx].tex_off;
      	  }
      	  for(unsigned cs_idx = 0; cs_idx < c_110.size() && cs_idx < last_cs; ++cs_idx){
      	    weight_d += c_110[cs_idx].weight;
      	    pos_offset = pos_offset + c_110[cs_idx].weight * c_110[cs_idx].pos_off;
      	    tex_offset = tex_offset + c_110[cs_idx].weight * c_110[cs_idx].tex_off;
      	  }
      	  for(unsigned cs_idx = 0; cs_idx < c_111.size() && cs_idx < last_cs; ++cs_idx){
      	    weight_d += c_111[cs_idx].weight;
      	    pos_offset = pos_offset + c_111[cs_idx].weight * c_111[cs_idx].pos_off;
      	    tex_offset = tex_offset + c_111[cs_idx].weight * c_111[cs_idx].tex_off;
      	  }

      	  if(weight_d > 0.01){
      	    xyz pos_offset_f;
      	    pos_offset_f.x = pos_offset.x/weight_d;
      	    pos_offset_f.y = pos_offset.y/weight_d;
      	    pos_offset_f.z = pos_offset.z/weight_d;
      	    uv tex_offset_f;
      	    tex_offset_f.u = tex_offset.u/weight_d;
      	    tex_offset_f.v = tex_offset.v/weight_d;
      	    xyz pos3D_new = pos3D + pos_offset_f;
      	    uv tex_new = tex + tex_offset_f;
      	    m_cv_xyzs[i][cv_index] = pos3D_new;
      	    m_cv_uvs[i][cv_index] = tex_new;
      	    //m_cv_valids[0][cv_index] = true;
      	  }
      	  else{
      	    m_cv_valids[0][cv_index] = false;
      	  }
      	}
      }
    }
  }
}