#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

#include "Material.h"
#include "Mesh.h"
#include "MeshTools.h"
#include "Shader.h"
#include "Texture.h"
#include "Utilities.h"

//Singleton class for managing graphical resources
//Although the use of a singleton here is technically bad practice (it's really just a global), I'm taking the hit because it's annoying passing the damn thing around everywhere
class GraphicsResourceManager
{
private:
	Material* defaultMaterial = nullptr;	//For convenience, we keep a record of a default material to use as a fallback if we ever fail to load one
	std::map<std::string, Material*> materials;
	std::map<std::string, Shader*> shaders;
	std::map<std::string, Texture*> textures;
	std::map<std::string, Mesh*> meshes;
	Material* loadMaterialFromFile(std::string fileName);
	Shader* loadShaderFromFile(std::string fileName);
	Texture* loadTextureFromFile(std::string fileName);
	Mesh* loadMeshFromFile(std::string fileName);
	GraphicsResourceManager();
public:
	~GraphicsResourceManager();
	GraphicsResourceManager(const GraphicsResourceManager&) = delete;
	GraphicsResourceManager& operator=(const GraphicsResourceManager&) = delete;
	void initialseBasicResources();
	Material* loadMaterial(std::string name);
	Shader* loadShader(std::string name);		//Note that at the moment we can only load the standard Vertex+Fragment shader type
	Texture* loadTexture(std::string name);
	Mesh* loadMesh(std::string name);
	void addMesh(std::string name, Mesh* mesh);	//Add a mesh created elsewhere so the GRM can look after it
	void addShader(std::string name, Shader* shader);	//Add a shader created elsewhere so the GRM can look after it
	void addTexture(std::string name, Texture* texture);	//Add a texture created elsewhere so the GRM can look after it
	void deleteTexture(std::string name);
	//Get the singleton instance
	static GraphicsResourceManager& getInstance()
	{
		static GraphicsResourceManager instance;
		return instance;
	}
};