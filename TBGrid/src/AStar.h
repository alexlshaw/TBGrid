#pragma once

#include "Utilities.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <forward_list>
#include <vector>

//code to support the A* pathfinding

class LevelGrid;	//Forward declaration due to circular dependency

namespace AI
{
	struct Node
	{
		int x, y, z;
		float f, g, h;	//cost values
		std::shared_ptr<Node> parent;

		Node(int x, int y, int z, float g = 0.0f, float h = 0.0f, std::shared_ptr<Node> parent = nullptr)
			: x(x), y(y), z(z), g(g), h(h), f(g + h), parent(parent) {}

		bool operator>(const Node& other) const
		{
			return f > other.f;
		}

		bool operator==(const Node& other) const
		{
			return x == other.x && y == other.y && z == other.z;
		}
	};

	inline float manhattan_distance(int x1, int y1, int z1, int x2, int y2, int z2)
	{
		return static_cast<float>(std::abs(x1 - x2) + std::abs(y1 - y2) + std::abs(z1 - z2));
	}

	//Tests if the xyz coordinates fall within the bounds of 0-w,0-h,0-d respectively
	inline bool isValid(int x, int y, int z, int w, int h, int d)
	{
		return x >= 0 && x < w && y >= 0 && y < h && z >= 0 && z < d;
	}

	std::vector<Node> aStar(const LevelGrid& grid, Node start, Node goal);
}