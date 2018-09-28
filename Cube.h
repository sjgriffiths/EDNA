/**
 * Cube.h
 * Declares the Cube class, representing a 3x3x3 Rubik's
 * Cube by encoding 20 movable cubies.
 *
 * @author Sam Griffiths
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct Cube
{
	//Represents an individual cubie, which can be an edge or corner piece
	struct Cubie
	{
		char a; //First face
		char b; //Second face
		char c; //Third face; zero if edge piece

		Cubie(char a, char b, char c) : a(a), b(b), c(c) {}
		Cubie(char a, char b) : Cubie(a, b, 0) {}
		Cubie() : Cubie(0, 0, 0) {}

		friend bool operator==(const Cubie &lhs, const Cubie &rhs) {
			return lhs.a == rhs.a && lhs.b == rhs.b && lhs.c == rhs.c;
		}

		std::string string() const {
			std::string s; s += a; s += b; if (c != 0) s += c; return s;
		}
	};
	//Flat list of cubies in order
	std::vector<Cubie> cubies;

	//Number of cubies expected to represent a Cube
	static const size_t NUMBER_OF_CUBIES = 20;

	//Defines the possible operations on a cube as cyclic sequences
	static const std::unordered_map<char, std::vector<char>> sequences;


	//Default constructor initialises void cubies
	Cube();
	//Instantiates cubies from the string representation provided
	Cube(const std::string &str);

	//Cubes are equal if their 20-cubie representations are the same
	friend bool operator==(const Cube &lhs, const Cube &rhs);
	
	//Compares the given cube based solely on the given cubie
	bool compareCubie(const Cube &other, size_t cubie) const;

	//Outputs 20-cubie representation
	friend std::ostream& operator<<(std::ostream &os, const Cube &cube);

	//Returns a new Cube state having twisted the given face in the given dir (+,-,2)
	Cube twist(char face, char dir) const;
};


template<>
struct std::hash<Cube::Cubie>
{
	size_t operator()(const Cube::Cubie &c) const noexcept
	{
		//Bernstein hash
		size_t hash = 5381;
		hash = hash * 33 + std::hash<char>()(c.a);
		hash = hash * 33 + std::hash<char>()(c.b);
		hash = hash * 33 + std::hash<char>()(c.c);
		return hash;
	}
};

template<>
struct std::hash<Cube>
{
	size_t operator()(const Cube &c) const noexcept
	{
		//Bernstein hash
		size_t hash = 5381;
		for (const Cube::Cubie &cubie : c.cubies)
			hash = hash * 33 + std::hash<Cube::Cubie>()(cubie);
		return hash;
	}
};
