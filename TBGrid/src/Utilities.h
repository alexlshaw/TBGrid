#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "assimp/quaternion.h"
#include "assimp/vector3.h"
#include "assimp/color4.h"
#include "assimp/matrix4x4.h"

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
glm::vec4 parseVector(std::string str, const char delim = ',');

//if we have a 3D grid represented by a 1D array of values, it's useful to be able to convert from index to axis values and vice versa
//Assumes values are emplaced incrementing first along z, then y, then x (so nested loops with x outside to z inside)
inline glm::ivec3 arrayCoordsFromIndex3D(const int index, const int width, const int height, const int depth)
{
	//width is in fact an unnecessary parameter, but we're leaving it in because it makes the 3D usage clearer
	int x = index / (depth * height);
	int zyRemainder = index - (depth * height * x);
	int y = zyRemainder / depth;
	int z = zyRemainder - (depth * y);
	return { x, y, z };
}

//if we have a 3D grid represented by a 1D array of values, it's useful to be able to convert from axis values to index
//Assumes values are emplaced incrementing first along z, then y, then x (so nested loops with x outside to z inside)
inline int arrayIndexFromCoords3D(const glm::ivec3 coords, const int width, const int height, const int depth)
{
	//width is in fact an unnecessary parameter, but we're leaving it in because it makes the 3D usage clearer
	//return x + (width * y) + (width * height * z);
	return coords.z + (depth * coords.y) + (depth * height * coords.x);
}

//Conversion from Assimp types to glm types
namespace AssimpGLMHelpers
{
	inline glm::mat4 convertMatrixToGLM(const aiMatrix4x4& from)
	{
		glm::mat4 to{};
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	inline glm::vec3 convertVec3ToGLM(const aiVector3D& vec)
	{
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	inline glm::vec4 convertVec4ToGLM(const aiColor4D& vec)
	{
		return glm::vec4(vec.r, vec.g, vec.b, vec.a);
	}

	inline glm::quat convertQuatToGLM(const aiQuaternion& from)
	{
		return glm::quat(from.w, from.x, from.y, from.z);
	}
}