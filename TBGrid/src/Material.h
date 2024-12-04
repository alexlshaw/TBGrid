#pragma once

#include <map>
#include <string>

#include "glad/gl.h"
#include "assimp/material.h"

#include "Camera.h"
#include "Light.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

//For each of our different texture types, we reserve a particular slot (Currently this won't handle meshes with multiple textures of any given type, but that doesn't apply to any I use)
enum TextureSlots	//Use these when setting shader uniforms
{
	TEXTURE_DIFFUSE = 0,
	TEXTURE_SPECULAR = 1,
	TEXTURE_NORMAL_MAP = 2,
	TEXTURE_SHADOW_MAP = 3
};
//use these when setting glActiveTexture
constexpr int GL_TEXTURE_DIFFUSE = GL_TEXTURE0;
constexpr int GL_TEXTURE_SPECULAR = GL_TEXTURE1;
constexpr int GL_TEXTURE_NORMAL_MAP = GL_TEXTURE2;
constexpr int GL_TEXTURE_SHADOW_MAP = GL_TEXTURE3;

class Material
{
private:
	//Matrix uniforms
	int modelMatrix = -1;
	int projectionViewMatrix = -1;
	int normalMatrix = -1;
	//Texture and colour uniforms
	int ambientColourUniform = -1;
	int diffuseTextureUniform = -1;
	int diffuseColourUniform = -1;
	int specularTextureUniform = -1;
	int specularColourUniform = -1;
	//lighting uniforms
	int viewPosUniform = -1;
	int lightUniformBlockIndex = -1;
	unsigned int uboLight = -1;
	int shininessUniform = -1;
	int shadowMapUniform = -1;
	int lightSpaceMatrixUniform = -1;
	float shininess = 1.0f;
	Shader* shader = nullptr;
	bool lit = false;
	bool useNormals = false;	//If the material needs a normalMatrix passed in. automatically set to true if lit is true
	//todo: find a more generic solution for this rather than using a pair of per-type maps
	std::map<std::string, int> vectorPropertyUniforms;
	std::map<std::string, glm::vec4> vectorPropertyValues;
	std::map<std::string, int> floatPropertyUniforms;
	std::map<std::string, float> floatPropertyValues;
public:
	Material(std::string name, Shader* shader, Texture* diffuse, Texture* specular);
	Material(std::string name);
	Material(aiMaterial* material);
	std::string name;
	void setLit(bool val);
	void setUseNormals(bool val);
	void use(Camera* camera, const LightBlock& lights, const glm::mat4& lightSpaceMatrix);
	void setTransform(Transform transform);
	bool enableBlending = false;
	void setProperty(std::string propertyName, glm::vec4 propertyValue, bool createIfMissing = false);
	void setProperty(std::string propertyName, float propertyValue, bool createIfMissing = false);
	
	//Texture and colour properties
	glm::vec4 ambientColour = glm::vec4(1.0f);
	glm::vec4 diffuseColour = glm::vec4(1.0f);
	glm::vec4 specularColour = glm::vec4(1.0f);
	Texture* diffuseMap = nullptr;
	Texture* specularMap = nullptr;
};