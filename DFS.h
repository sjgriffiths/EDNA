/**
* DFS.h
* Implements depth-first search.
*
* @author Sam Griffiths
*/

#ifndef DFS_H
#define DFS_H

#include "Search.h"

namespace Search
{
	template <typename Node>
	Path DFS(const Node &start, const Node &goal, size_t depthLimit)
	{
		//Associate nodes with preceding edge and depth
		using DFSNode = std::pair<Edge<Node>, size_t>;

		//Stack of nodes to be expanded
		std::deque<DFSNode> open;
		open.push_front({ {start,""}, 0 });

		//Current search path
		std::vector<DFSNode> trace;

		//Only start searching if the goal isn't already reached
		bool found = false;
		while (!found && !open.empty())
		{
			//DEBUG - Print open stack
			DEBUG(
				std::deque<Node> out;
				std::transform(open.begin(), open.end(), std::back_inserter(out),
					[](const DFSNode &x){return x.first.first;});
				printCollection(out);
			)

			//Explore the next node
			DFSNode n = open.front();
			open.pop_front();

			//Check for solution
			if (n.first.first == goal)
			{
				trace.push_back(n);
				found = true;
			}
			else
			{
				//Backtrack path by depth
				size_t d = n.second;
				while (trace.size() > d)
					trace.pop_back();

				//Get the node's children, if below the depth limit
				bool anyChildren = false;
				if (d < depthLimit)
				{
					for (const Edge<Node> &c : n.first.first.expand())
					{
						//Only add if not in current path (i.e. does not form a cycle)
						bool cycle = false;
						for (const auto &x : trace)
							if (x.first.first == c.first)
								cycle = true;

						if (!cycle)
						{
							open.push_front({ c, d + 1 });
							anyChildren = true;
						}
					}
				}

				//Add to path (if not backtracking)
				if (anyChildren)
					trace.push_back(n);
			}
		}

		//Construct solution path
		Path path;

		if (found)
			for (size_t i = 1; i < trace.size(); i++)
				path.push_back(trace[i].first.second);

		return path;
	}
}

#endif
