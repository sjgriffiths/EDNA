/**
 * PHS.h
 * Implements pure heuristic search (greedy best-first).
 *
 * @author Sam Griffiths
 */

#ifndef PHS_H
#define PHS_H

#include "Search.h"

namespace Search
{
	template <typename Node>
	Path PHS(const Node &start, const Node &goal, HeuristicFunc<Node> h)
	{
		//Priority queue of nodes to be expanded
		std::priority_queue<Node, std::vector<Node>, std::function<bool(const Node&, const Node&)>> open
		(
			[&](const Node &a, const Node &b) { return h(a, goal) > h(b, goal); }
		);
		open.push(start);

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
			if (open.top() == goal)
			{
				solution = &open.top();
				found = true;
			}
			else
			{
				//Get the next node to expand
				Node n = open.top();
				open.pop();
				closed.insert(n);

				//Get the node's children
				for (const Edge<Node> &c : n.expand())
				{
					//Only keep new children
					if (closed.find(c.first) == closed.end() && !found)
					{
						open.push(c.first);

						//Log the parent edge
						trace.insert({ c.first, { n, c.second } });
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
