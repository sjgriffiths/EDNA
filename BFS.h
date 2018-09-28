/**
 * BFS.h
 * Implements breadth-first search.
 *
 * @author Sam Griffiths
 */

#ifndef BFS_H
#define BFS_H

#include "Search.h"

namespace Search
{
	template <typename Node>
	Path BFS(const Node &start, const Node &goal)
	{
		//Queue of nodes to be expanded
		std::deque<Node> open;
		open.push_back(start);

		//Set of nodes already visited
		std::unordered_set<Node> closed;

		//Map of node to its parent edge
		std::unordered_map<Node, Edge<Node>> trace;

		//Pointer to solution node, once found
		const Node *solution = nullptr;

		//Only start searching if the goal isn't already reached
		bool found = false;
		if (start == goal)
		{
			solution = &goal;
			found = true;
		}

		while (!found && !open.empty())
		{
			//DEBUG - Print open queue
			DEBUG( printCollection(open); )

			//Get the next node to expand
			Node n = open.front();
			open.pop_front();
			closed.insert(n);

			//Get the node's children
			for (const Edge<Node> &c : n.expand())
			{
				//Only keep new children
				if (closed.find(c.first) == closed.end() && !found)
				{
					open.push_back(c.first);

					//Log the parent edge
					trace.insert({ open.back(), { n, c.second } });

					//Check for solution
					if (open.back() == goal)
					{
						solution = &open.back();
						found = true;
					}
				}
			}
		}

		//Construct solution path
		Path path;
		while (trace.find(*solution) != trace.end())
		{
			path.push_back(trace.at(*solution).second);
			solution = &trace.at(*solution).first;
		}

		std::reverse(path.begin(), path.end());
		return path;
	}
}

#endif
