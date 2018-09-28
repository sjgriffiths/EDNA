/**
 * FourBitIntPair.h
 * Lightweight struct providing the ability for
 * two four-bit unsigned integers (0-15) to be 
 * represented in an octet.
 *
 * @author Sam Griffiths
 */

#pragma once

#include <cstdint>

struct FourBitIntPair
{
	uint8_t x;


	//Default constructor initialises to zero
	FourBitIntPair() : x(0) {}

	//Constructs a FourBitIntPair from octet repesentation
	FourBitIntPair(uint8_t x) : x(x) {}

	//Constructs a FourBitIntPair from the two values
	FourBitIntPair(uint8_t a, uint8_t b) { x = (a << 4) + b; }

	//Returns the first value
	uint8_t a() const { return x >> 4; }

	//Returns the second value
	uint8_t b() const { return x & 15; }

	//Sets the first value
	void aSet(uint8_t a) { x = (a << 4) + b(); }

	//Sets the second value
	void bSet(uint8_t b) { x = (a() << 4) + b; }
};
