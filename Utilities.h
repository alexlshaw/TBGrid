#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "glm\glm.hpp"

#define DEGREES_TO_RADIANS      .017453292F
#define RADIANS_TO_DEGREES      57.29577951F
#define PI                      ((double)3.1415926535F)

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