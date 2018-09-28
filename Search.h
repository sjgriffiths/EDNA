/**
 * Search.h
 * Declares type definitions and functions representing
 * AI state space search. Custom node types will implement
 * expand(), returning a vector of Edges, and also the 
 * equality operator and hash function object.
 *
 * Define SEARCH_DEBUG to activate debug mode for searching.
 *
 * @author Sam Griffiths
 */

#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <functional>

#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <queue>

#ifdef SEARCH_DEBUG
	#define DEBUG(X) X
#else
	#define DEBUG(X)
#endif

namespace Search
{
	//Edge has an operation represented by a string
	template <typename Node>
	using Edge = std::pair<Node, std::string>;

	//Path is a list of operations (strings)
	using Path = std::vector<std::string>;

	//Heuristic function returns a double from 2 Nodes
	template <typename Node>
	using HeuristicFunc = std::function<double(const Node&, const Node&)>;


	/* SEARCH ALGORITHMS */

	//Breadth-first search
	template <typename Node>
	Path BFS(const Node &start, const Node &goal);

	//Depth-first search (empty Path if not found)
	template <typename Node>
	Path DFS(const Node &start, const Node &goal, size_t depthLimit);

	//Iterative deepening depth-first search
	template <typename Node>
	Path IDDFS(const Node &start, const Node &goal);

	//Pure heuristic search (greedy best-first)
	template <typename Node>
	Path PHS(const Node &start, const Node &goal, HeuristicFunc<Node> h);

	//A* search
	template <typename Node>
	Path Astar(const Node &start, const Node &goal, HeuristicFunc<Node> h);

	//Iterative deepening A* search
	template <typename Node>
	Path IDAstar(const Node &start, const Node &goal, HeuristicFunc<Node> h);



	/* UTILITY FUNCTIONS */

	//Prints the contents of a collection to std::cout
	template <typename T, template <typename, typename> typename C>
	void printCollection(const C<T, std::allocator<T>> &c)
	{
		for (const T &t : c)
			std::cout << t << " ";
		std::cout << std::endl;
	}
}

#include "BFS.h"
#include "DFS.h"
#include "IDDFS.h"
#include "PHS.h"
#include "Astar.h"
#include "IDAstar.h"
