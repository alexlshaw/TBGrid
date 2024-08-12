#pragma once

#include <string>
#include <vector>

#include "Camera.h"
#include "DebuggingTools.h"
#include "Light.h"
#include "Material.h"
#include "Transform.h"

//Base class representing an object that has references in the world
class GameObject
{
private:
protected:
	std::vector<Material*> materials;
public:
	virtual void draw(int renderPass, Transform transform) = 0;	//draw() should always be overridden in a child class
	virtual int getRenderPasses();
	virtual void activateMaterial(int renderPass, Camera* camera, Light light);
	std::string name = "";
	bool noCull = false;	//if true, the scene never tries to cull the object if it is outside the camera's frustrum
};