/**
 * CubeNode.h
 * Configures the Cube class as a searchable node type.
 *
 * @author Sam Griffiths
 */

#pragma once

#include "Cube.h"
#include "Search.h"

struct CubeNode
{
	Cube cube;


	CubeNode(Cube cube) : cube(cube) {}

	std::vector<Search::Edge<CubeNode>> expand() const;

	friend bool operator==(const CubeNode &lhs, const CubeNode &rhs);
	friend std::ostream& operator<<(std::ostream &os, const CubeNode &cubeNode);
};

template<>
struct std::hash<CubeNode>
{
	size_t operator()(const CubeNode &n) const noexcept
	{
		return std::hash<Cube>()(n.cube);
	}
};
