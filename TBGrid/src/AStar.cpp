#include "AStar.h"

#include "LevelGrid.h"

using namespace AI;

std::vector<Node> AI::aStar(const LevelGrid& grid, Node start, Node goal)
{
	glm::ivec3 gridSize = grid.getDimensions();
	std::forward_list<Node> openSet;
	std::vector<std::vector<std::vector<bool>>> closedSet(static_cast<size_t>(gridSize.x), std::vector<std::vector<bool>>(static_cast<size_t>(gridSize.y), std::vector<bool>(static_cast<size_t>(gridSize.z), false)));

	openSet.push_front(start);

	// Movement in 4 directions (forward, back, left, right).		For now we're only doing movement on the 2d plane
	std::vector<std::pair<int, int>> directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

	while (!openSet.empty())
	{
		//pop the node with the smallest f-score from the open set
		auto minFNode = std::min_element(openSet.begin(), openSet.end(),
			[](const Node& a, const Node& b) 
			{
				return a.f < b.f;
			});
		Node current = *minFNode;
		openSet.remove(current);

		//if goal is reached, reconstruct the path
		if (current.x == goal.x && current.y == goal.y && current.z == goal.z)
		{
			std::vector<Node> path;
			while (current.parent != nullptr)
			{
				path.push_back(current);
				current = *current.parent;
			}
			std::reverse(path.begin(), path.end());
			return path;
		}
		//goal not yet reached, proceed
		closedSet[current.x][current.y][current.z] = true;

		//explore neighbors (f, b, l, r)
		for (const auto& dir : directions)
		{
			glm::ivec3 newLoc{ current.x + dir.first, current.y, current.z + dir.second };
			//check if the node in this direction is possible to look at and walkable and not already checked
			if (grid.validCell(newLoc) && grid.getCell(newLoc).walkable && !closedSet[newLoc.x][newLoc.y][newLoc.z])
			{
				//compute scores for the node in question
				float newG = current.g + 1.0f;
				float newH = manhattan_distance(newLoc.x, newLoc.y, newLoc.z, goal.x, goal.y, goal.z);
				//check if the node is in the open set already
				auto it = std::find_if(openSet.begin(), openSet.end(), [newLoc](Node n) {return n.x == newLoc.x && n.y == newLoc.y && n.z == newLoc.z; });
				if (it != openSet.end())
				{
					//it is already in the open set, update its values
					Node& match = *it;
					if (match.g > newG)
					{
						match.g = newG;
					}
				}
				else
				{
					//it's not currently in the open set, add it
					Node neighbor(newLoc.x, newLoc.y, newLoc.z, newG, newH, std::make_shared<Node>(current));
					openSet.push_front(neighbor);
				}
			}
		}
	}
	//Couldn't find a path, return an empty one
	return {};
}