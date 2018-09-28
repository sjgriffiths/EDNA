/**
 * main.cpp
 * Defines the main entry point and program structure of
 * the EDNA.exe executable.
 *
 * @author Sam Griffiths
 */

//#define SEARCH_DEBUG

#include "XGetopt.h"
#include "Utility.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <functional>

//Ensures mutual exclusion of mode option flags
bool validateMode()
{
	static bool set = false;

	if (set)
	{
		std::cerr << "Error: Only 1 mode option can be set" << std::endl;
		return false;
	}
	else
	{
		set = true;
		return true;
	}
}

//Ensures mutual exclusion of algorithm option flags
bool validateAlg()
{
	static bool set = false;

	if (set)
	{
		std::cerr << "Error: Only 1 algorithm option can be set" << std::endl;
		return false;
	}
	else
	{
		set = true;
		return true;
	}
}


//Main entry point
int main(int argc, char **argv)
{
	//Clock to use for execution timing
	using clock = std::chrono::high_resolution_clock;

	//Depth flag
	size_t depth = 0;

	//Will we require a search algorithm?
	bool needAlg = true, needHeur = true;

	//Subroutine encapsulating the heuristic function to use (default: pattern database)
	std::function<double(const CubeNode&, const CubeNode&)> hFunc;

	//Subroutine encapsulating the search algorithm to use (default: IDA*)
	std::function<Search::Path(const CubeNode&, const CubeNode&)> executeSearch;

	//Get command line options
	enum CMD_OPTIONS { GENERATE, GENERATE_ALL, MANHATTAN, PATTERN, TIME, MANHATTAN_USE };
	bool opts[6] = { false };
	char optstring[] = "g:GMPtbdipam";
	int c;
	bool success = true;
	std::string algName = "ITERATIVE DEEPENING A*";

	while ((c = getopt(argc, argv, optstring)) != -1)
		switch (c)
		{
		case 'g':
			success &= validateMode();
			try { depth = std::stoi(optarg); }
			catch (std::invalid_argument&) { 
				std::cerr << "Error: Valid solution depth must be specified" << std::endl; 
				return EXIT_FAILURE;
			}
			opts[GENERATE] = true; needAlg = false; break;
		case 'G':
			success &= validateMode();
			opts[GENERATE_ALL] = true; needAlg = false; break;
		case 'M':
			success &= validateMode();
			opts[MANHATTAN] = true; needAlg = false; break;
		case 'P':
			success &= validateMode();
			opts[PATTERN] = true; needAlg = false; break;
		case 't':
			success &= validateMode();
			opts[TIME] = true; break;
		case 'b':
			success &= validateAlg();
			algName = "BREADTH-FIRST SEARCH"; needHeur = false;
			executeSearch = [](const CubeNode &a, const CubeNode &b) { return Search::BFS(a, b); }; break;
		case 'd':
			success &= validateAlg();
			algName = "DEPTH-FIRST SEARCH"; needHeur = false;
			executeSearch = [&depth](const CubeNode &a, const CubeNode &b) { return Search::DFS(a, b, depth); }; break;
		case 'i':
			success &= validateAlg();
			algName = "ITERATIVE DEEPENING DEPTH-FIRST SEARCH"; needHeur = false;
			executeSearch = [](const CubeNode &a, const CubeNode &b) { return Search::IDDFS(a, b); }; break;
		case 'p':
			success &= validateAlg();
			algName = "PURE HEURISTIC SEARCH";
			executeSearch = [&hFunc](const CubeNode &a, const CubeNode &b) { return Search::PHS(a, b, hFunc); }; break;
		case 'a':
			success &= validateAlg();
			algName = "A*";
			executeSearch = [&hFunc](const CubeNode &a, const CubeNode &b) { return Search::Astar(a, b, hFunc); }; break;
		case 'm':
			opts[MANHATTAN_USE] = true; break;
		default:
			std::cerr << "Error: Illegal option" << std::endl; return EXIT_FAILURE; break;
		}

	//Ensure valid combination of options
	if (!success)
		return EXIT_FAILURE;

	//Validate algorithm settings, if needed
	ManhattanMap m;
	PatternDatabase corner, edge1, edge2;
	if (needAlg)
	{
		//Default to IDA*
		if (!executeSearch)
			executeSearch = [&hFunc](const CubeNode &a, const CubeNode &b) { return Search::IDAstar(a, b, hFunc); };

		//Manual use of depth-first search not supported
		if (!opts[TIME] && algName == "DEPTH-FIRST SEARCH")
		{
			std::cerr << "Error: Manual use of depth-first search not supported" << std::endl;
			return EXIT_FAILURE;
		}

		//Set the heuristic to Manhattan, if user-defined
		if (needHeur)
		{
			if (opts[MANHATTAN_USE])
			{
				std::cout << "Loading Manhattan distance table..." << std::endl;

				std::ifstream mFile("manhattantable.txt");
				if (!mFile)
				{
					std::cerr << "Error: manhattantable.txt missing" << std::endl;
					return EXIT_FAILURE;
				}

				m = loadManhattanTable(mFile);
				mFile.close();

				//Total Manhattan distance is sum of edge piece distances, divided by 4
				hFunc = [&m](const CubeNode &a, const CubeNode &b)
				{
					size_t sum = 0;
					for (size_t i = 0; i < 12; i++)
						sum += lookupManhattanTable(a.cube.cubies[i], b.cube.cubies[i], m);
					return sum / 4.0;
				};
			}
			//Otherwise, default to pattern databases
			else
			{
				std::cout << "Loading pattern databases..." << std::endl;

				//Load corner database
				std::ifstream pdFile("cornerpd.bin", std::ifstream::binary);
				if (!pdFile)
				{
					std::cerr << "Error: cornerpd.bin missing" << std::endl;
					return EXIT_FAILURE;
				}				

				corner = loadPatternDatabase(pdFile, 88179840);
				pdFile.close();

				//Load edge 1 database
				pdFile.open("edge1pd.bin", std::ifstream::binary);
				if (!pdFile)
				{
					std::cerr << "Error: edge1pd.bin missing" << std::endl;
					return EXIT_FAILURE;
				}

				edge1 = loadPatternDatabase(pdFile, 42577920);
				pdFile.close();

				//Load edge 2 database
				pdFile.open("edge2pd.bin", std::ifstream::binary);
				if (!pdFile)
				{
					std::cerr << "Error: edge2pd.bin missing" << std::endl;
					return EXIT_FAILURE;
				}

				edge2 = loadPatternDatabase(pdFile, 42577920);
				pdFile.close();

				//Total heuristic is max of three pattern database lookups
				hFunc = [&corner, &edge1, &edge2](const CubeNode &a, const CubeNode &b)
				{
					size_t i = getCornerConfigIndex(enumerateCornerConfig(a.cube));
					double c = (i % 2 == 0) ? corner[i / 2].a() : corner[i / 2].b();
					i = getEdgeConfigIndex(enumerateEdgeConfig(a.cube, 1));
					double e1 = (i % 2 == 0) ? edge1[i / 2].a() : edge1[i / 2].b();
					i = getEdgeConfigIndex(enumerateEdgeConfig(a.cube, 2));
					double e2 = (i % 2 == 0) ? edge2[i / 2].a() : edge2[i / 2].b();

					return std::max({ c, e1, e2 });
				};
			}
		}
	}


	/* SINGLE TEST CASE GENERATION */
	if (opts[GENERATE])
	{
		std::cout << generateCubeProblem(depth, true) << std::endl;
		return EXIT_SUCCESS;
	}


	/* ALL TEST CASE GENERATION */
	if (opts[GENERATE_ALL])
	{
		std::cout << "Generating test case files..." << std::endl;

		for (size_t i = 2; i <= 20; i++)
		{
			std::ofstream file("testcases_depth" + std::to_string(i) + ".txt");

			for (int j = 0; j < 10; j++)
				file << generateCubeProblem(i) << std::endl;

			file.close();
		}

		return EXIT_SUCCESS;
	}


	/* MANHATTAN DISTANCE TABLE GENERATION */
	if (opts[MANHATTAN])
	{
		std::cout << "Generating Manhattan distance table file..." << std::endl;

		std::ofstream file("manhattantable.txt");
		generateManhattanTable(file);
		file.close();

		return EXIT_SUCCESS;
	}


	/* PATTERN DATABASE GENERATION */
	if (opts[PATTERN])
	{
		std::cout << "Generating pattern database files..." << std::endl;

		std::ofstream file;

		file.open("cornerpd.bin", std::ofstream::binary);
		generateCornerPatternDatabase(file);
		file.close();

		file.open("edge1pd.bin", std::ofstream::binary);
		generateEdgePatternDatabase(file, 1);
		file.close();

		file.open("edge2pd.bin", std::ofstream::binary);
		generateEdgePatternDatabase(file, 2);
		file.close();

		return EXIT_SUCCESS;
	}


	//Print algorithm name for modes hereinafter
	std::cout << "Using " << algName << std::endl << std::endl;


	/* TEST CASE TIMING */
	if (opts[TIME])
	{
		std::cout << "Time in seconds to solve depth n [median]:" << std::endl;

		for (depth = 2; depth <= 20; depth++)
		{
			//Load all string representations in file n
			std::vector<std::string> testCases;
			std::string fileName("testcases_depth" + std::to_string(depth) + ".txt");
			std::ifstream file;
			file.open(fileName);
			if (!file)
			{
				std::cerr << "Error: " << fileName << " missing" << std::endl;
				return EXIT_FAILURE;
			}

			std::string line;
			while (std::getline(file, line))
				testCases.push_back(line);
			file.close();

			std::cout << depth << ": ";

			//Solve and time all test cubes
			std::vector<std::chrono::duration<double>> times;
			for (auto &t : testCases)
			{
				Cube c(t);
				CubeNode cn(c);
				auto t0 = clock::now();
				Search::Path p = executeSearch(cn, GOAL_CUBE_NODE);
				auto t1 = clock::now();
				times.emplace_back(t1 - t0);
				std::cout << times.back().count() << " ";
			}

			std::cout << "[" << median(times).count() << "]" << std::endl;
		}

		return EXIT_SUCCESS;
	}
	
	
	/* DEFAULT CUBE SOLVING */

	//Ensure a cube has been given
	if (!argv[optind])
	{
		std::cerr << "Error: String representation of cube must be provided" << std::endl;
		return EXIT_FAILURE;
	}
	
	//Solve and time
	Cube cube;
	try { cube = Cube(argv[optind]); }
	catch (std::ios_base::failure &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	
	CubeNode cubeNode(cube);
	auto t0 = clock::now();
	Search::Path path = executeSearch(cubeNode, GOAL_CUBE_NODE);
	auto t1 = clock::now();
	
	//Display results
	std::cout << "Path: ";
	if (path.empty())
		std::cout << "NOT FOUND";
	else for (auto &p : path)
		std::cout << p << " ";

	std::cout << std::endl << "Depth: " << path.size() << std::endl;
	std::cout << "Time taken: " << std::chrono::duration<double>(t1 - t0).count() << " seconds" << std::endl;
 
	return EXIT_SUCCESS;
}
