#pragma once

#include <vector>
#include "Billboard.h"
#include "BoxCollider.h"
#include "GraphicsResourceManager.h"
#include "LevelGrid.h"
#include "StaticMesh.h"
#include "Transform.h"
#include "TurnBoundUnit.h"

class EnemyUnit : public TurnBoundUnit
{
private:
	std::shared_ptr<Billboard> healthBar = nullptr;
	Mesh* constructFlatMesh();
	void createHealthBar();
public:
	EnemyUnit(LevelGrid* grid);
	~EnemyUnit() {}
	void update(const float deltaTime);
	void updateHealthBarDisplay() override;
	//AI Stuff
	void determineAction();
};