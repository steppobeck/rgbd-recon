#ifndef NATURAL_NEIGHBOUR_INTERPOLATOR_H
#define NATURAL_NEIGHBOUR_INTERPOLATOR_H

#include <DataTypes.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/natural_neighbor_coordinates_3.h>

#include <vector>
#include <map>

namespace kinect{


  struct nniSample{
    xyz s_pos;
    xyz s_pos_off;
    uv  s_tex_off;
    float quality;
  };

	extern std::ostream& operator<< (std::ostream& o, const nniSample& s);
	
  class NaturalNeighbourInterpolator{


    typedef double NT; //Number Type
    typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
    typedef K::Point_3 Point3;
    typedef K::Vector_3 Vector3;
    typedef K::Sphere_3 Sphere_3;
    typedef CGAL::Delaunay_triangulation_3<K, CGAL::Fast_location> Dh;

    typedef Dh::Facet Facet;
    typedef Dh::Vertex_handle Vertex_handle;
    typedef Dh::Cell_handle Cell_handle;
    typedef Dh::Finite_vertices_iterator Finite_vertices_iterator;
    typedef Dh::Vertex_iterator Vertex_iterator;
    typedef Dh::Facet_circulator Facet_circulator;
    typedef Dh::Cell_iterator Cell_iterator;

    typedef K::Construct_circumcenter_3 Construct_circumcenter_3;


  public:
    NaturalNeighbourInterpolator(const std::vector<nniSample>& samples);
    ~NaturalNeighbourInterpolator();

    bool interpolate(nniSample& ipolant);

  private:
    Dh m_dt;
    std::map<Vertex_handle, nniSample> m_vd;
  };

}

#endif // #ifndef NATURAL_NEIGHBOUR_INTERPOLATOR_H
