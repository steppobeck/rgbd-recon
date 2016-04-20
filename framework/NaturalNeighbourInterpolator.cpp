#include "NaturalNeighbourInterpolator.h"



namespace kinect{

  extern std::ostream& operator<< (std::ostream& o, const nniSample& s){
    o << "s_pos: (" << s.s_pos.x << "," << s.s_pos.y << "," << s.s_pos.z << ") "
      << "s_pos_off: (" << s.s_pos_off.x << "," << s.s_pos_off.y << "," << s.s_pos_off.z << ") "
      << "s_tex_off: (" << s.s_tex_off.u << "," << s.s_tex_off.v << ")"
      << "s_quality: " << s.quality;
    return o;
  }
	
	
  NaturalNeighbourInterpolator::NaturalNeighbourInterpolator(const std::vector<nniSample>& samples)
    : m_dt(),
      m_vd()
  {

    for(unsigned i = 0; i < samples.size(); ++i){

      nniSample sp = samples[i];
      Point3 sp_dt(sp.s_pos.x, sp.s_pos.y, sp.s_pos.z);
      m_vd[m_dt.insert(sp_dt)] = sp;

    }

  }

  NaturalNeighbourInterpolator::~NaturalNeighbourInterpolator(){
  }
    
  bool
  NaturalNeighbourInterpolator::interpolate(nniSample& ipolant){


    Point3 ipos(ipolant.s_pos.x, ipolant.s_pos.y, ipolant.s_pos.z);

    std::vector< std::pair< Vertex_handle,NT> > coor_sibson;
    NT norm_coeff_sibson;

    sibson_natural_neighbor_coordinates_3(m_dt,ipos,
					  std::back_inserter(coor_sibson),
					  norm_coeff_sibson);

    

    if(0 == coor_sibson.size()){
      return false;
    }

    //std::cerr << "norm_coeff: " << norm_coeff_sibson << " num_neighbours "<< coor_sibson.size() << std::endl;


    std::vector< std::pair< Vertex_handle,NT> >::iterator it;
    unsigned c_idx = 0;
    //double sum = 0;

    xyz_d pos_off;
    pos_off.x = 0.0;pos_off.y = 0.0;pos_off.z = 0.0;
    uv_d tex_off;
    tex_off.u = 0.0;tex_off.v = 0.0;

    for(it = coor_sibson.begin() ; it != coor_sibson.end() ; ++it, ++c_idx){
      double contribution_i = it->second;
      nniSample s = m_vd[it->first];
      pos_off.x += contribution_i * s.s_pos_off.x;
      pos_off.y += contribution_i * s.s_pos_off.y;
      pos_off.z += contribution_i * s.s_pos_off.z;

      tex_off.u += contribution_i * s.s_tex_off.u;
      tex_off.v += contribution_i * s.s_tex_off.v;

      //sum += contribution_i;
      //std::cout << c_idx << " " << contribution_i << "% from " << it->first->point() << std::endl;
      //sum_x += it->second*(it->first->point().x());
      //sum_y += it->second*(it->first->point().y());
      //sum_z += it->second*(it->first->point().z());
    }
    //std::cerr << "sum: " << sum << std::endl;
    
    // normalize
    ipolant.s_pos_off.x = pos_off.x / norm_coeff_sibson;
    ipolant.s_pos_off.y = pos_off.y / norm_coeff_sibson;
    ipolant.s_pos_off.z = pos_off.z / norm_coeff_sibson;

    ipolant.s_tex_off.u = tex_off.u / norm_coeff_sibson;
    ipolant.s_tex_off.v = tex_off.v / norm_coeff_sibson;

    return true;//is_correct_natural_neighborhood(m_dt,ipos, coor_sibson.begin(), coor_sibson.end(), norm_coeff_sibson);
  }
    
}
