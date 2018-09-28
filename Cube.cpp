/**
 * Cube.cpp
 * Implements the Cube class, representing a 3x3x3 Rubik's
 * Cube by encoding 20 movable cubies.
 *
 * @author Sam Griffiths
 */

#include "Cube.h"

#include <sstream>

const std::unordered_map<char, std::vector<char>> Cube::sequences{
	/* Sequences for quarter clockwise turn (opposite faces
		are simply the reverse sequence, but they are still
		hard-coded here for lookup speed). */
	{ 'U', { 'F', 'L', 'B', 'R' } },
	{ 'D', { 'R', 'B', 'L', 'F' } },

	{ 'R', { 'U', 'B', 'D', 'F' } },
	{ 'L', { 'F', 'D', 'B', 'U' } },

	{ 'F', { 'U', 'R', 'D', 'L' } },
	{ 'B', { 'L', 'D', 'R', 'U' } }
};

Cube::Cube()
{
	//Reserves and initialises empty cubies
	cubies.reserve(NUMBER_OF_CUBIES);

	while (cubies.size() < NUMBER_OF_CUBIES)
		cubies.emplace_back();
}

Cube::Cube(const std::string &str)
{
	cubies.reserve(NUMBER_OF_CUBIES);

	std::string buffer;
	std::istringstream iss(str);

	//Reads in cubie tokens separated by whitespace
	while (iss >> buffer)
	{
		if (buffer.length() < 2 || buffer.length() > 3)
			throw std::ios_base::failure("Invalid cubie specification");

		//Load each edge, deciding whether it's an edge or corner
		cubies.emplace_back(buffer.at(0), buffer.at(1), 
			(buffer.length() == 3) ? buffer.at(2) : 0);
	}

	//Expect correct number of cubies
	if (cubies.size() != NUMBER_OF_CUBIES)
		throw std::ios_base::failure("Invalid cubie specification");
}

bool operator==(const Cube &lhs, const Cube &rhs)
{
	//Cube completely identified by vector of cubies, which implements equality
	return lhs.cubies == rhs.cubies;
}

bool Cube::compareCubie(const Cube &other, size_t cubie) const
{
	return cubies[cubie] == other.cubies[cubie];
}

std::ostream& operator<<(std::ostream &os, const Cube &cube)
{
	bool first = true;
	for (const Cube::Cubie &c : cube.cubies)
	{
		if (!first) os << " ";
		os << c.string();
		first = false;
	}
	return os;
}

Cube Cube::twist(char face, char dir) const
{
	//Get relevant cyclic sequence
	auto it = sequences.find(face);

	//Validate
	if ((dir != '+' && dir != '-' && dir != '2') || (it == sequences.end()))
		throw std::logic_error(std::string("Operation '") + face + dir + "' not recognised");
	
	const std::vector<char> &seq = it->second;

	//Utility function applies operation to specified cubie face
	auto apply = [&seq, &dir](char &c)
	{
		//Find in sequence
		auto it = std::find(seq.begin(), seq.end(), c);
		if (it == seq.end())
			throw std::logic_error("Impossible face/operation combination");

		//Change to next/previous in sequence
		if (dir == '-')
		{
			if (it == seq.begin())
				it = seq.end();
			--it;
		}
		else
		{
			++it;
			if (it == seq.end())
				it = seq.begin();
		}

		c = *it;
	};

	//Iterate over cloned child, applying operation where applicable
	Cube cube(*this);

	int num = dir == '2' ? 2 : 1;
	for (int i = 0; i < num; ++i)
		for (Cubie &c : cube.cubies)
		{
			if (c.a == face)
			{
				apply(c.b);
				if (c.c != 0) apply(c.c);
			}
			else if (c.b == face)
			{
				apply(c.a);
				if (c.c != 0) apply(c.c);
			}
			else if (c.c == face)
			{
				apply(c.a);
				apply(c.b);
			}
		}

	return cube;
}
