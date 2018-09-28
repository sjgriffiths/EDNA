/**
 * CubeNode.cpp
 * Configures the Cube class as a searchable node type.
 *
 * @author Sam Griffiths
*/

#include "CubeNode.h"

std::vector<Search::Edge<CubeNode>> CubeNode::expand() const
{
	std::vector<Search::Edge<CubeNode>> nodes;
	nodes.reserve(18);

	nodes.emplace_back(cube.twist('U', '+'), "U+");
	nodes.emplace_back(cube.twist('U', '-'), "U-");
	nodes.emplace_back(cube.twist('U', '2'), "U2");

	nodes.emplace_back(cube.twist('D', '+'), "D+");
	nodes.emplace_back(cube.twist('D', '-'), "D-");
	nodes.emplace_back(cube.twist('D', '2'), "D2");

	nodes.emplace_back(cube.twist('R', '+'), "R+");
	nodes.emplace_back(cube.twist('R', '-'), "R-");
	nodes.emplace_back(cube.twist('R', '2'), "R2");

	nodes.emplace_back(cube.twist('L', '+'), "L+");
	nodes.emplace_back(cube.twist('L', '-'), "L-");
	nodes.emplace_back(cube.twist('L', '2'), "L2");

	nodes.emplace_back(cube.twist('F', '+'), "F+");
	nodes.emplace_back(cube.twist('F', '-'), "F-");
	nodes.emplace_back(cube.twist('F', '2'), "F2");

	nodes.emplace_back(cube.twist('B', '+'), "B+");
	nodes.emplace_back(cube.twist('B', '-'), "B-");
	nodes.emplace_back(cube.twist('B', '2'), "B2");

	return nodes;
}

bool operator==(const CubeNode &rhs, const CubeNode &lhs)
{
	return rhs.cube == lhs.cube;
}

std::ostream& operator<<(std::ostream &os, const CubeNode &cubeNode)
{
	return os << cubeNode.cube;
}
