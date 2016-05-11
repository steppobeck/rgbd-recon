#ifndef KINECT_NEAREST_NEIGHBOUR_SEARCH_HPP
#define KINECT_NEAREST_NEIGHBOUR_SEARCH_HPP

#include <DataTypes.h>
#include <glm/gtc/type_precision.hpp>

#include <vector>

namespace kinect {
	
struct sample_t{
	sample_t(glm::fvec3 const& p, glm::uvec3 id)
	 :pos{p}
	 ,index{id}
	{}

	glm::fvec3 pos;
  glm::uvec3 index;
};

class NearestNeighbourSearch{
public:
	NearestNeighbourSearch(const std::vector<sample_t>& spoints);
	~NearestNeighbourSearch();

	std::vector<sample_t> search(const sample_t& ipolant, unsigned num_neighbours) const;

private:
	const std::vector<sample_t> m_samples;
	void* m_tree;
};

}
#endif // #ifndef KINECT_NEAREST_NEIGHBOUR_SEARCH_HPP