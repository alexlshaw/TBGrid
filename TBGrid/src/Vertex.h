#pragma once

#include "glad/gl.h"
#include "glm/glm.hpp"

//vertex data associated with UI elements
struct UIVertex
{
	glm::vec2 position;
	glm::vec2 texCoords;
};

namespace AnimationConstants
{
	constexpr int MAX_BONE_INFLUENCE = 4;	//Has a matching value in SkeletalAnimation.vert, make sure to change both
}

//Vertex data associated with animated meshes. Conveniently 64 bytes in size
struct Vertex
{
	glm::vec3 position;
	glm::vec2 texCoords;
	glm::vec3 normal;
	int boneIDs[AnimationConstants::MAX_BONE_INFLUENCE];	//indices of the bones within the mesh which will influence this vertex
	float boneWeights[AnimationConstants::MAX_BONE_INFLUENCE];	//weights of the influence of the associated bones
};

//vertex data associated with basic meshes
struct ColouredVertex
{
	glm::vec4 position;
	glm::vec2 texCoords;
	glm::vec3 normal;
	glm::vec4 color;
	GLubyte padding[12];	//pad to 64 bytes -> improved performance on some cards
};

inline void setUIVertexAttribs()
{
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const GLvoid*)8);
	glEnableVertexAttribArray(1);
}

inline void setVertexAttribs()
{
	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)0);
	glEnableVertexAttribArray(0);
	//texcoords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
	glEnableVertexAttribArray(1);
	//normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);
	glEnableVertexAttribArray(2);
	//ids
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (const GLvoid*)32);
	glEnableVertexAttribArray(3);
	//weights
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)48);
	glEnableVertexAttribArray(4);
}

inline void setColouredVertexAttribs()
{
	//position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ColouredVertex), (const GLvoid*)0);
	glEnableVertexAttribArray(0);
	//texcoords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ColouredVertex), (const GLvoid*)16);
	glEnableVertexAttribArray(1);
	//normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ColouredVertex), (const GLvoid*)24);
	glEnableVertexAttribArray(2);
	//color
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(ColouredVertex), (const GLvoid*)36);
	glEnableVertexAttribArray(3);
}