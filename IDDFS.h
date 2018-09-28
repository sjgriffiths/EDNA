/**
 * IDDFS.h
 * Implements iterative deepening depth-first search.
 *
 * @author Sam Griffiths
 */

#ifndef IDDFS_H
#define IDDFS_H

#include "Search.h"

namespace Search
{
	template <typename Node>
	Path IDDFS(const Node &start, const Node &goal)
	{
		size_t depthLimit = 0;
		Path path;

		while (path.empty())
			path = DFS(start, goal, ++depthLimit);

		return path;
	}
}

#endif
