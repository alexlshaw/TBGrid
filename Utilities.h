#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "glm\glm.hpp"

constexpr float DEGREES_TO_RADIANS = .017453292f;
constexpr float RADIANS_TO_DEGREES = 57.29577951f;
constexpr float PI = 3.1415926535f;

inline bool within(float n1, float n2, float range)
{
	if (n1 - n2 > range || n2 - n1 > range)
	{
		return false;
	}
	return true;
}

//This is clamp()
inline float lockToRange(float val, float min, float max)
{
	return val < min ? min : val > max ? max : val;
}

//This is an efficient lerp implementation, but can suffer from precision loss when a and b are far apart
inline float fastLerp(float a, float b, float f)
{
	return a + f * (b - a);
}

bool pointInTriangle(glm::vec2 p, glm::vec2 p0, glm::vec2 p1, glm::vec2 p2);

//Outputs a random float between 0 (inclusive), and 1 (exclusive)
inline float randomFloat()
{
	return static_cast<float>(rand() % 10000) / 10000.0f;
}

//Splits a string according to a (single character) delimiter
std::vector<std::string_view> split(std::string_view str, char delim);

//takes a string in the format x,y,z,w and creates a vec4. Assumes string is well formed
glm::vec4 parseVector(std::string str);


//Useful for working with 3D arrays that are compressed into a 1d array
struct ArrayCoords3D
{
	int x, y, z;
};

//if we have a 3D grid represented by a 1D array of values, it's useful to be able to convert from index to axis values and vice versa
//Assumes values are emplaced incrementing first along z, then y, then x (so nested loops with x outside to z inside)
inline ArrayCoords3D arrayCoordsFromIndex3D(const int index, const int width, const int height, const int depth)
{
	//width is in fact an unnecessary parameter, but we're leaving it in because it makes the 3D usage clearer
	int x = index / (depth * height);
	int zyRemainder = index - (depth * height * x);
	int y = zyRemainder / depth;
	int z = zyRemainder - (depth * y);
	return ArrayCoords3D{ x, y, z };
}

//if we have a 3D grid represented by a 1D array of values, it's useful to be able to convert from axis values to index
//Assumes values are emplaced incrementing first along z, then y, then x (so nested loops with x outside to z inside)
inline int arrayIndexFromCoords3D(const ArrayCoords3D coords, const int width, const int height, const int depth)
{
	//width is in fact an unnecessary parameter, but we're leaving it in because it makes the 3D usage clearer
	//return x + (width * y) + (width * height * z);
	return coords.z + (depth * coords.y) + (depth * height * coords.x);
}