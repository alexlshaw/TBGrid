#pragma once

#include <map>
#include <string>

#include "glad/gl.h"

#include "Camera.h"
#include "DebuggingTools.h"
#include "Light.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

class Material
{
private:
	int modelMatrix;
	int projectionViewMatrix;
	int normalMatrix;
	int textureUniform;
	int ambientUniform;
	int diffuseUniform;
	int lightPositionUniform;
	Shader* shader;
	Texture* texture;		//may need to improve to handle multiple textures for one material
	bool lit;
	//todo: find a more generic solution for this rather than using a pair of per-type maps
	std::map<std::string, int> vectorPropertyUniforms;
	std::map<std::string, glm::vec4> vectorPropertyValues;
	std::map<std::string, int> floatPropertyUniforms;
	std::map<std::string, float> floatPropertyValues;
public:
	Material(std::string name, Shader* shader, Texture* texture);
	std::string name;
	void setLit(bool val);
	void use(Camera* camera, Light light);
	void setTransform(Transform transform);
	bool enableBlending;
	void setProperty(std::string propertyName, glm::vec4 propertyValue, bool createIfMissing = false);
	void setProperty(std::string propertyName, float propertyValue, bool createIfMissing = false);
};