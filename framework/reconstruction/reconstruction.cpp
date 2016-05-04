#include "reconstruction.hpp"

#include "calibration_files.hpp"
#include "CalibVolume.h"

namespace kinect{

Reconstruction::Reconstruction(CalibrationFiles const& cfs, CalibVolume const* cv, gloost::BoundingBox const&  bbox)
 :m_cv(cv)
 ,m_tex_width{cfs.getWidth()}
 ,m_tex_height{cfs.getHeight()}
 ,m_num_kinects{cfs.num()}
 ,m_min_length{cfs.minLength()}
 ,m_bbox{bbox}
{}

}