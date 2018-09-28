/**
 * Utility.h
 * Declares functions representing various utilities
 * for the experimental project.
 *
 * @author Sam Griffiths
 */

#pragma once

#include "CubeNode.h"
#include "FourBitIntPair.h"

//Defines the Cube goal states
const Cube GOAL_CUBE("UF UR UB UL DF DR DB DL FR FL BR BL UFR URB UBL ULF DRF DFL DLB DBR");
const CubeNode GOAL_CUBE_NODE(GOAL_CUBE);


//Generates a random Cube permutation of given solution depth
Cube generateCubeProblem(size_t depth, bool print = false);


//Internal map format of the Manhattan distance function
using ManhattanMap = std::unordered_map<std::string, std::unordered_map<std::string, size_t>>;

//Generates the edge piece Manhattan lookup CSV to the given stream
void generateManhattanTable(std::ostream &os);

//Loads a ManhattanMap from the given stream
ManhattanMap loadManhattanTable(std::istream &is);

//Looks up the Manhattan distance between the given edge cubies
size_t lookupManhattanTable(const Cube::Cubie &a, const Cube::Cubie &b, ManhattanMap &m);


//Internal map format of the pattern database
using PatternDatabase = std::vector<FourBitIntPair>;

//Loads a PatternDatabase from the given stream
PatternDatabase loadPatternDatabase(std::istream &is, size_t n);


//Enumerates the corner piece configuration of the given Cube
std::vector<uint8_t> enumerateCornerConfig(const Cube &cube);

//Converts a corner piece enumeration into database index
size_t getCornerConfigIndex(const std::vector<uint8_t> &config);

//Generates the corner piece pattern database to the given stream
void generateCornerPatternDatabase(std::ostream &os);


//Enumerates the edge piece configuration of the given Cube (set 1 or 2)
std::vector<uint8_t> enumerateEdgeConfig(const Cube &cube, int set);

//Converts an edge piece enumeration into database index (set 1 or 2)
size_t getEdgeConfigIndex(const std::vector<uint8_t> &config);

//Generates the edge piece pattern database to the given stream (set 1 or 2)
void generateEdgePatternDatabase(std::ostream &os, int set);


//Computes the median of a given vector of elements
template <typename T>
T median(std::vector<T> v)
{
	size_t n = v.size();

	std::nth_element(v.begin(), v.begin() + n/2, v.end());

	if (n % 2 == 0)
	{
		std::nth_element(v.begin(), v.begin() + n/2 - 1, v.end());
		return (v[n/2] + v[n/2 - 1]) / 2;
	}
	else
		return v[n/2];
}
