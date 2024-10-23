#pragma once

#include <vector>
#include "BoxCollider.h"
#include "GraphicsResourceManager.h"
#include "LevelGrid.h"
#include "StaticMesh.h"
#include "Transform.h"
#include "TurnBoundUnit.h"

class EnemyUnit : public TurnBoundUnit
{
public:
	EnemyUnit();
	~EnemyUnit() {}
	void update(const float deltaTime);
	
	//AI Stuff
	void determineAction(LevelGrid& levelInfo);
};