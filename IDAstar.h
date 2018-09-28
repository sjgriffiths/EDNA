/**
 * IDAstar.h
 * Implements IDA* search.
 *
 * @author Sam Griffiths
 */

#ifndef IDAstar_H
#define IDAstar_H

#include "Search.h"

namespace Search
{
	template <typename Node>
	Path IDAstar(const Node &start, const Node &goal, HeuristicFunc<Node> h)
	{
		//Associate nodes with preceding edge and depth
		using IDANode = std::pair<Edge<Node>, size_t>;

		//Current heuristic depth limit
		double threshold = h(start, goal);

		//Current search path
		std::vector<IDANode> trace;

		//Only start searching if the goal isn't already reached
		bool found = false;
		while (!found)
		{
			//Tracker for the minimum of pruned costs
			double thresholdNew = std::numeric_limits<double>::infinity();

			//Stack of nodes to be expanded
			std::deque<IDANode> open;
			open.push_front({ { start,"" }, 0 });

			//Ensure trace is reset
			trace.clear();

			//Perform DFS iteration
			while (!open.empty() && !found)
			{
				//DEBUG - Print open stack
				DEBUG(
					std::deque<Node> out;
					std::transform(open.begin(), open.end(), std::back_inserter(out),
						[](const DFSNode &x){return x.first.first;});
					printCollection(out);
				)

				//Explore the next node
				IDANode n = open.front();
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

					//Get the node's children
					bool anyChildren = false;
					for (const Edge<Node> &c : n.first.first.expand())
					{
						double cost = d + 1 + h(c.first, goal);

						//If below the threshold, add
						if (cost <= threshold)
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

						//Else, prune and log minimum
						else if (cost < thresholdNew)
							thresholdNew = cost;
					}

					//Add to path (if not backtracking)
					if (anyChildren)
						trace.push_back(n);
				}
			}

			//Update threshold for next iteration
			if (!found)
				threshold = thresholdNew;
		}

		//Construct solution path
		Path path;
		
		for (size_t i = 1; i < trace.size(); i++)
			path.push_back(trace[i].first.second);

		return path;
	}
}

#endif
