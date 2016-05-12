#include "nearest_neighbour_search.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Search_traits_3.h>
#include <CGAL/Search_traits_adapter.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/property_map.h>
#include <boost/iterator/zip_iterator.hpp>
#include <utility>

namespace kinect {
  
NearestNeighbourSearch::NearestNeighbourSearch(const std::vector<sample_t>& samples)
 :m_samples(samples)
 ,m_tree()
{
  std::vector<Point_3> points;
  std::vector<size_t> indices;

  for(std::size_t i = 0; i < samples.size(); ++i){
    points.emplace_back(samples[i]);
    indices.emplace_back(i);
  }

  m_tree = std::unique_ptr<Tree>{new Tree(
    boost::make_zip_iterator(boost::make_tuple(points.begin(), indices.begin())),
    boost::make_zip_iterator(boost::make_tuple(points.end(), indices.end()))  
  )};
}

std::vector<sample_t>
NearestNeighbourSearch::search(glm::fvec3 const& curr_point, unsigned num_neighbours) const {

  K_neighbor_search search(*(m_tree.get()), Point_3{curr_point.x, curr_point.y, curr_point.z}, num_neighbours);

  std::vector<sample_t> result;
  for(K_neighbor_search::iterator it = search.begin(); it != search.end(); it++) {
    result.emplace_back(m_samples[boost::get<1>(it->first)]);
  }

  return result;
}

}