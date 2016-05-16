#ifndef KINECT_NEAREST_NEIGHBOUR_SEARCH_HPP
#define KINECT_NEAREST_NEIGHBOUR_SEARCH_HPP

#include <DataTypes.h>
#include <glm/gtc/type_precision.hpp>

#include <vector>
#include <memory>
#include <utility>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Search_traits_3.h>
#include <CGAL/Search_traits_adapter.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/property_map.h>

namespace kinect {
	
struct sample_t{
	sample_t(glm::fvec3 const& p, glm::uvec3 id)
	 :pos{p}
	 ,index{id}
	{}

	operator CGAL::Exact_predicates_inexact_constructions_kernel::Point_3() const {
		return CGAL::Exact_predicates_inexact_constructions_kernel::Point_3{pos.x, pos.y, pos.z};
	}

	glm::fvec3 pos;
  glm::uvec3 index;
};

class NearestNeighbourSearch{
public:
	NearestNeighbourSearch(const std::vector<sample_t>& spoints);

	std::vector<sample_t> search(glm::fvec3 const& cur_point, unsigned num_neighbours) const;

private:
	typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
	typedef Kernel::Point_3                                     Point_3;
	typedef CGAL::Search_traits_3<Kernel>                       Traits_base;
	typedef boost::tuple<Point_3,int>                           Point_and_int;
	typedef CGAL::Search_traits_adapter<Point_and_int,
	  CGAL::Nth_of_tuple_property_map<0, Point_and_int>,
	  Traits_base>                                              Traits;
	typedef CGAL::Orthogonal_k_neighbor_search<Traits>          K_neighbor_search;
	typedef K_neighbor_search::Tree                             Tree;
	typedef K_neighbor_search::Distance                         Distance;
		
	const std::vector<sample_t> m_samples;
	std::unique_ptr<Tree> m_tree;

};

}
#endif // #ifndef KINECT_NEAREST_NEIGHBOUR_SEARCH_HPP