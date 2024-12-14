#include "PlayerUnit.h"
#include <functional>
#include <memory>
#include "BoxCollider.h"
#include "GraphicsResourceManager.h"
#include "RiggedObject.h"
#include "Transform.h"

//Throw in a few usings to keep std::X usage under control and the code readable, since we have some nested types here
using std::function;
using std::pair;
using std::shared_ptr;
using std::vector;

PlayerUnit::PlayerUnit(LevelGrid* grid)
	:TurnBoundUnit(grid),
	attackActionPointCost(3)	//arbitary value
{
	movementSpeed = 1.7f;	//Sync with the walk animation speed

	GraphicsResourceManager& resourceManager = GraphicsResourceManager::getInstance();
	name = "PlayerUnit";
	//Probably need to place a default material here
	Mesh* cube = resourceManager.loadMesh("unit_cube");
	Material* defaultRed = resourceManager.loadMaterial("DefaultRed");
	collider = std::make_unique<BoxCollider>();
	collider->offset = glm::vec3(-0.5f, 0.0f, -0.5f);
	collider->layer = Collision::Layer_Unit;

	//load the animations
	AnimatedModel* animModel = resourceManager.loadAnimatedModel("X Bot");
	Animation* tauntAnim = resourceManager.loadAnimation("Taunt", animModel);
	Animation* idleAnim = resourceManager.loadAnimation("Idle", animModel);
	Animation* walkAnim = resourceManager.loadAnimation("Female Walk", animModel);
	//create the animation states
	shared_ptr<AnimationGraphNode> tauntState = std::make_shared<AnimationGraphNode>(tauntAnim);
	shared_ptr<AnimationGraphNode> idleState = std::make_shared<AnimationGraphNode>(idleAnim);
	shared_ptr<AnimationGraphNode> walkState = std::make_shared<AnimationGraphNode>(walkAnim);
	vector<shared_ptr<AnimationGraphNode>> allStates;
	allStates.push_back(tauntState);
	allStates.push_back(idleState);
	allStates.push_back(walkState);
	//create the animation state transitions
	function<bool(Animator*)> exitOnComplete = [](Animator* a) {return a->playCount > 0; };
	AnimationGraphTransition tauntExitToIdle = { idleState.get(), 0.75f, exitOnComplete};
	tauntState->transitions.push_back(tauntExitToIdle);
	AnimationGraphTransition idleExitToTaunt = { tauntState.get(), 0.5f, exitOnComplete };
	idleState->transitions.push_back(idleExitToTaunt);
	function<bool(Animator*)> startWalking = [](Animator* a) { return a->getBool("walking"); };
	function<bool(Animator*)> stopWalking = [](Animator* a) { return !a->getBool("walking"); };
	AnimationGraphTransition exitToWalk = { walkState.get(), 0.1f, startWalking };
	tauntState->transitions.push_back(exitToWalk);
	idleState->transitions.push_back(exitToWalk);
	AnimationGraphTransition walkExitToIdle = { idleState.get(), 0.1f, stopWalking };
	walkState->transitions.push_back(walkExitToIdle);

	//create the animator
	shared_ptr<Animator> animator = std::make_shared<Animator>(tauntState.get(), allStates);
	visualsAnimator = animator.get();
	//create the actual animated visuals
	shared_ptr<RiggedObject> animatedObject = std::make_shared<RiggedObject>(animator);
	animatedObject->transform.setScale({ 0.01f, 0.01f, 0.01f });	//Assimp brings in mixamo's fbx files at 100x the scale this engine uses, scale it down
	animatedObject->name = "Player Visuals";
	addChild(animatedObject);

	//Create the selection plane
	Mesh* plane = resourceManager.loadMesh("unit_plane");
	Material* selectionMaterial = resourceManager.loadMaterial("SelectionIndicator");
	selectedIndicator = std::make_shared<StaticMesh>(plane, selectionMaterial);
	selectedIndicator->name = "Player unit selection indicator";
	selectedIndicator->transform = Transform(glm::vec3(-0.5f, 0.025f, -0.5f), glm::identity<glm::mat4>(), glm::vec3(1.0f, 1.0f, 1.0f));
	selectedIndicator->enabled = false;
	selectedIndicator->castsShadows = false;
	addChild(selectedIndicator);
}

void PlayerUnit::update(float deltaTime)
{
	//set the animator state
	if (visualsAnimator)
	{
		bool walking = hasAction() && action->actionType == Actions::Movement;
		visualsAnimator->setBool("walking", walking);
	}

	//for now, we only need to process the action, but that will change in the future
	if (processAction(deltaTime))
	{
		action.reset();
		action = nullptr;
	}
}

void PlayerUnit::activateAbility(int abilityID)
{
	DEBUG_PRINTLN(std::format("{} activated ability: {}", name, abilityID));
}
