#include "nearest_neighbour_search.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Search_traits_3.h>
#include <CGAL/Search_traits_adapter.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/property_map.h>
#include <boost/iterator/zip_iterator.hpp>
#include <utility>

namespace kinect {

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3                                     Point_3;
typedef boost::tuple<Point_3,int>                           Point_and_int;
typedef CGAL::Search_traits_3<Kernel>                       Traits_base;
typedef CGAL::Search_traits_adapter<Point_and_int,
  CGAL::Nth_of_tuple_property_map<0, Point_and_int>,
  Traits_base>                                              Traits;
typedef CGAL::Orthogonal_k_neighbor_search<Traits>          K_neighbor_search;
typedef K_neighbor_search::Tree                             Tree;
typedef K_neighbor_search::Distance                         Distance;
  
NearestNeighbourSearch::NearestNeighbourSearch(const std::vector<sample_t>& samples)
 :m_samples(samples)
 ,m_tree()
{
  std::vector<Point_3> points;
  std::vector<size_t> indices;
  size_t idx = 0;

  for(const auto& sample : m_samples){
    points.push_back(Point_3(sample.pos.x, sample.pos.y, sample.pos.z));
    indices.push_back(idx);
    ++idx;
  }

  m_tree = new Tree(
    boost::make_zip_iterator(boost::make_tuple(points.begin(), indices.begin())),
    boost::make_zip_iterator(boost::make_tuple(points.end(), indices.end()))  
  );
}

NearestNeighbourSearch::~NearestNeighbourSearch() {
  // delete m_tree;
}

std::vector<sample_t>
NearestNeighbourSearch::search(const sample_t& ipolant,unsigned num_neighbours) const{
  std::vector<sample_t> result;

  Point_3 query_point(ipolant.pos.x, ipolant.pos.y, ipolant.pos.z);
  K_neighbor_search search(*(reinterpret_cast<Tree*>(m_tree)), query_point, num_neighbours);

  for(K_neighbor_search::iterator it = search.begin(); it != search.end(); it++) {
    result.push_back(m_samples[boost::get<1>(it->first)]);
  }

  return result;
}

}