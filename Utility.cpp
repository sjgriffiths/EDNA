/**
	* Utility.cpp
	* Implements functions representing various utilities
	* for the experimental project.
	*
	* @author Sam Griffiths
*/

#include "Utility.h"

#include <random>

Cube generateCubeProblem(size_t depth, bool print)
{
	Cube c(GOAL_CUBE);

	char faces[]  = { 'F','R','U' };
	char twists[] = { '+','-','2' };
	char face = 0, faceLast = 0;

	std::default_random_engine rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<size_t> dist(0, 2);

	for (size_t i = 0; i < depth; i++)
	{
		while (face == faceLast)
			face = faces[dist(rng)];

		char twist = twists[dist(rng)];

		c = c.twist(face, twist);
		faceLast = face;

		if (print)
			std::cout << face << twist << " ";
	}

	if (print)
		std::cout << std::endl;

	return c;
}

void generateManhattanTable(std::ostream &os)
{
	for (size_t i = 0; i < 12; i++)
	{
		std::deque<std::pair<CubeNode, size_t>> open;
		open.push_back({ GOAL_CUBE_NODE, 0 });

		std::unordered_set<CubeNode> closed;
		std::unordered_set<Cube::Cubie> found;
		found.insert(open.front().first.cube.cubies[i]);

		while (found.size() < 23)
		{
			std::pair<CubeNode, size_t> n = open.front();
			open.pop_front();
			closed.insert(n.first);

			size_t d = n.second + 1;

			for (const Search::Edge<CubeNode> &c : n.first.expand())
			{
				//Only consider new children
				if (closed.find(c.first) == closed.end())
				{
					if (found.find(c.first.cube.cubies[i]) == found.end())
					{
						found.insert(c.first.cube.cubies[i]);

						os << GOAL_CUBE.cubies[i].string() <<
							"," << c.first.cube.cubies[i].string() <<
							"," << d << std::endl;
					}

					open.push_back({ c.first, d });
				}
			}
		}
	}
}

ManhattanMap loadManhattanTable(std::istream &is)
{
	ManhattanMap m;
	std::string sa, sb, val;

	while (std::getline(is, sa, ','))
	{
		std::getline(is, sb, ',');
		std::getline(is, val);
		size_t v = std::stoi(val);
		m[sa][sb] = v;
	}

	return m;
}

size_t lookupManhattanTable(const Cube::Cubie &a, const Cube::Cubie &b, ManhattanMap &m)
{
	std::string ca(a.string()), cb(b.string());

	if (ca == cb) return 0;

	//If the cubie isn't to be found, its reverse should be
	if (m.find(ca) == m.end())
	{
		std::reverse(ca.begin(), ca.end());
		std::reverse(cb.begin(), cb.end());
	}

	return m[ca][cb];
}

PatternDatabase loadPatternDatabase(std::istream &is, size_t n)
{
	PatternDatabase pd(n / 2);
	is.read(reinterpret_cast<char*>(&pd[0]), sizeof(pd[0]) * pd.size());

	return pd;
}

std::vector<uint8_t> enumerateCornerConfig(const Cube &cube)
{
	//Enumeration 0-7 of goal corner pieces
	static std::vector<std::string> pieces { GOAL_CUBE.cubies[12].string(),
		GOAL_CUBE.cubies[13].string(), GOAL_CUBE.cubies[14].string(),
		GOAL_CUBE.cubies[15].string(), GOAL_CUBE.cubies[16].string(), 
		GOAL_CUBE.cubies[17].string(), GOAL_CUBE.cubies[18].string(), 
		GOAL_CUBE.cubies[19].string() };

	//Look up above numeration by string
	auto lookupEnum = [](const std::string &s) -> uint8_t {
		for (uint8_t i = 0; i < pieces.size(); i++)
			if (std::is_permutation(s.begin(), s.end(), pieces[i].begin()))
				return i;
		return 0;
	};

	std::vector<uint8_t> result(15);

	for (size_t i = 0; i < 8; i++)
	{
		//Get cubie string
		std::string cubie = cube.cubies[12 + i].string();

		//Log piece position
		result[i] = lookupEnum(cubie);

		if (i < 7)
		{
			//Log piece orientation (having defaulted to 0)
			const std::string &goalCubie = pieces[result[i]];
			if (cubie != goalCubie)
			{
				std::string nextOrientation = goalCubie.substr(1, 2) + goalCubie[0];
				result[i + 8] = (cubie == nextOrientation) ? 1 : 2;
			}
		}
	}

	return result;
}

size_t getCornerConfigIndex(const std::vector<uint8_t> &config)
{
	size_t index = 0;
	size_t n = 88179840;
	size_t p = 8;

	bool indices[8];
	for (size_t i = 0; i < 8; i++)
		indices[i] = true;

	//For each digit, bar the last
	while (p > 1)
	{
		//Count 1s in the array before this digit
		size_t c = 0;
		for (size_t i = 0; i < config[8 - p]; i++)
			if (indices[i])
				c++;

		//Update index
		indices[config[8 - p]] = false;
		n /= p;
		p--;
		index += c * n;
	}

	//Adjust to orientiation index
	std::string base3;
	for (size_t i = 8; i < 15; i++)
		base3 += config[i] + '0';

	index += std::stoi(base3, nullptr, 3);

	return index;
}

void generateCornerPatternDatabase(std::ostream &os)
{
	//As possible values range from 0-11, use this as null value
	const uint8_t FOURBIT_NULL_VALUE = 15;

	//Initialise table to 88,179,840 4-bit integers
	PatternDatabase table(44089920, FourBitIntPair(FOURBIT_NULL_VALUE, FOURBIT_NULL_VALUE));

	std::deque<std::pair<CubeNode, uint8_t>> open;
	open.push_back({ GOAL_CUBE_NODE, 0 });

	//Counter of found states
	size_t found = 0;

	//Goal state yields zero
	table[0].aSet(0);
	found++;

	while (!open.empty() && found < 88179840)
	{
		std::pair<CubeNode, uint8_t> n = open.front();
		open.pop_front();

		uint8_t d = n.second + 1;

		for (const Search::Edge<CubeNode> &c : n.first.expand())
		{
			//Only consider new children (in terms of considered config)
			size_t index = getCornerConfigIndex(
				enumerateCornerConfig(c.first.cube)
			);

			if ( (index % 2 == 0 ? table[index / 2].a() : table[index / 2].b()) == FOURBIT_NULL_VALUE)
			{
				if (index % 2 == 0)
					table[index / 2].aSet(d);
				else
					table[index / 2].bSet(d);

				open.push_back({ c.first, d });
				found++;
			}
		}
	}

	os.write(reinterpret_cast<const char*>(&table[0]), sizeof(table[0]) * table.size());
}

std::vector<uint8_t> enumerateEdgeConfig(const Cube &cube, int set)
{
	if (set != 1 && set != 2)
		throw std::invalid_argument("Edge set must be 1 or 2");

	////Enumeration 0-5 of goal edge pieces (set 1)
	//static std::vector<std::string> pieces1 { GOAL_CUBE.cubies[0].string(),
	//	GOAL_CUBE.cubies[1].string(), GOAL_CUBE.cubies[2].string(),
	//	GOAL_CUBE.cubies[3].string(), GOAL_CUBE.cubies[4].string(),
	//	GOAL_CUBE.cubies[5].string() };

	////Enumeration 0-5 of goal edge pieces (set 2)
	//static std::vector<std::string> pieces2 { GOAL_CUBE.cubies[6].string(),
	//	GOAL_CUBE.cubies[7].string(), GOAL_CUBE.cubies[8].string(),
	//	GOAL_CUBE.cubies[9].string(), GOAL_CUBE.cubies[10].string(),
	//	GOAL_CUBE.cubies[11].string() };

	//const std::vector<std::string> &pieces = (set == 1) ? pieces1 : pieces2;

	//Enumeration 0-11 of goal edge pieces
	static std::vector<std::string> pieces { GOAL_CUBE.cubies[0].string(),
		GOAL_CUBE.cubies[1].string(), GOAL_CUBE.cubies[2].string(),
		GOAL_CUBE.cubies[3].string(), GOAL_CUBE.cubies[4].string(),
		GOAL_CUBE.cubies[5].string(), GOAL_CUBE.cubies[6].string(),
		GOAL_CUBE.cubies[7].string(), GOAL_CUBE.cubies[8].string(),
		GOAL_CUBE.cubies[9].string(), GOAL_CUBE.cubies[10].string(),
		GOAL_CUBE.cubies[11].string() };

	//Look up above numeration by string
	auto lookupEnum = [](const std::string &s) -> uint8_t {
		for (uint8_t i = 0; i < pieces.size(); i++)
			if (std::is_permutation(s.begin(), s.end(), pieces[i].begin()))
				return i;
		return 0;
	};

	std::vector<uint8_t> result(12);

	for (size_t i = 0; i < 6; i++)
	{
		//Get cubie string
		std::string cubie = cube.cubies[(set == 1) ? i : 6 + i].string();

		//Log piece position
		result[i] = lookupEnum(cubie);

		//Log piece orientation (having defaulted to 0)
		if (cubie != pieces[result[i]])
			result[i + 6] = 1;
	}

	return result;
}

size_t getEdgeConfigIndex(const std::vector<uint8_t> &config)
{
	size_t index = 0;
	size_t n = 42577920;
	size_t p = 6;
	size_t q = 12;

	bool indices[12];
	for (size_t i = 0; i < 12; i++)
		indices[i] = true;

	//For each digit, including the last
	while (p > 0)
	{
		//Count 1s in the array before this digit
		size_t c = 0;
		for (size_t i = 0; i < config[6 - p]; i++)
			if (indices[i])
				c++;

		//Update index
		indices[config[6 - p]] = false;
		n /= q;
		p--; q--;
		index += c * n;
	}

	//Adjust to orientation index
	std::string base2;
	for (size_t i = 6; i < 12; i++)
		base2 += config[i] + '0';

	index += std::stoi(base2, nullptr, 2);

	return index;
}

void generateEdgePatternDatabase(std::ostream &os, int set)
{
	if (set != 1 && set != 2)
		throw std::invalid_argument("Edge set must be 1 or 2");

	//As possible values range from 0-10, use this as null value
	const uint8_t FOURBIT_NULL_VALUE = 15;

	//Initialise table to 42,577,920 4-bit integers
	PatternDatabase table(21288960, FourBitIntPair(FOURBIT_NULL_VALUE, FOURBIT_NULL_VALUE));

	std::deque<std::pair<CubeNode, uint8_t>> open;
	open.push_back({ GOAL_CUBE_NODE, 0 });

	//Counter of found states
	size_t found = 0;

	//Goal state yields zero
	table[0].aSet(0);
	found++;

	while (!open.empty() && found < 42577920)
	{
		std::pair<CubeNode, uint8_t> n = open.front();
		open.pop_front();

		uint8_t d = n.second + 1;

		for (const Search::Edge<CubeNode> &c : n.first.expand())
		{
			//Only consider new children (in terms of considered config)
			size_t index = getEdgeConfigIndex(
				enumerateEdgeConfig(c.first.cube, set)
			);

			if ( (index % 2 == 0 ? table[index / 2].a() : table[index / 2].b()) == FOURBIT_NULL_VALUE)
			{
				if (index % 2 == 0)
					table[index / 2].aSet(d);
				else
					table[index / 2].bSet(d);

				open.push_back({ c.first,d });
				found++;
			}
		}
	}

	std::cout << "found: " << found << std::endl;

	os.write(reinterpret_cast<const char*>(&table[0]), sizeof(table[0]) * table.size());
}
