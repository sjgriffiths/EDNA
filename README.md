# EDNA
Rubik's Cube AI pathfinding solver

EDNA.exe provides functionality to optimally solve Rubik's Cube instances. It also provides functionality to solve instances through a variety of algorithms, generate test cases and perform timing experiments.


Default behaviour of EDNA.exe takes a Cube instance as a string argument and solves using ITERATIVE DEEPENING A* search (IDA*), outputting the solution path and time taken to solve. The algorithm used can be changed using these option flags:

-b BREADTH-FIRST SEARCH

-i ITERATIVE DEEPENING DEPTH-FIRST SEARCH

-p PURE HEURISTIC SEARCH

-a A*


By default, pattern databases are used as the heuristic function. This can be changed:

-m Uses the sum of edge piece Manhattan distances as the heuristic


Different execution modes are also available:

-g Generates a Cube of given solution depth n, giving the twists used to generate from the solved state and the string representation

-G Generates a series of .txt files for depths 2-20, each containing 10 different Cubes (testcases_depthN.txt)

-M Generates a .txt file of the edge piece Manhattan distance lookup table (manhattantable.txt)

-P Generates three .bin files of the pattern databases (cornerpd.bin, edge1pd.bin, edge2pd.bin) -- likely requires use of 64-bit application

-t Performs timing experiments on the above .txt test files, giving the time taken to solve each test case and the median for each depth, using the default/specified algorithm

-d DEPTH-FIRST SEARCH, available only for use with -t above 
