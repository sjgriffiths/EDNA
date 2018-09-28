/**
 * Astar.h
 * Implements A* search.
 *
 * @author Sam Griffiths
 */

#ifndef Astar_H
#define Astar_H

#include "Search.h"

namespace Search
{
	template <typename Node>
	Path Astar(const Node &start, const Node &goal, HeuristicFunc<Node> h)
	{
		//Associate nodes with their depth
		using ANode = std::pair<Node, size_t>;

		//Priority queue of nodes to be expanded
		std::priority_queue<ANode, std::vector<ANode>, std::function<bool(const ANode&, const ANode&)>> open
		(
			[&](const ANode &a, const ANode &b) { return h(a.first, goal) + a.second > h(b.first, goal) + b.second; }
		);
		open.push({ start, 0 });

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

			//Check for solution
			if (open.top().first == goal)
			{
				solution = &open.top().first;
				found = true;
			}
			else
			{
				//Get the next node to expand
				ANode n = open.top();
				open.pop();
				closed.insert(n.first);

				//Get the node's children
				for (const Edge<Node> &c : n.first.expand())
				{
					//Only keep new children
					if (closed.find(c.first) == closed.end() && !found)
					{
						open.push({ c.first, n.second + 1 });

						//Log the parent edge
						trace.insert({ c.first, { n.first, c.second } });
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
