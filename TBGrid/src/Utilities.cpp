#include "Utilities.h"

bool pointInTriangle(glm::vec2 p, glm::vec2 p0, glm::vec2 p1, glm::vec2 p2)
{
	float A = 0.5f * (-p1.y * p2.x + p0.y * (-p1.x + p2.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y);
	float sign = A < 0.0f ? -1.0f : 1.0f;
	float s = (p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * p.x + (p0.x - p2.x) * p.y) * sign;
	float t = (p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y) * sign;
	return s > 0 && t > 0 && (s + t) < 2 * A * sign;
}

std::vector<std::string_view> split(std::string_view str, char delim)
{
    std::vector<std::string_view> result;
    auto left = str.begin();
    for (auto it = left; it != str.end(); ++it)
    {
        if (*it == delim)
        {
            result.emplace_back(&*left, it - left);
            left = it + 1;
        }
    }
    if (left != str.end())
    {
        result.emplace_back(&*left, str.end() - left);
    }
    return result;
}

glm::vec4 parseVector(std::string str, const char delim)
{
    glm::vec4 output = glm::vec4();
    int idx = 0;
    for (auto& section : split(str, delim))
    {
        output[idx] = static_cast<float>(atof(section.data()));
        idx++;
    }
    return output;
}