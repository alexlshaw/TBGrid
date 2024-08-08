#include "GraphicsResourceManager.h"

GraphicsResourceManager::GraphicsResourceManager() {}

GraphicsResourceManager::~GraphicsResourceManager()
{
	//delete the shaders, materials, and textures
	for (auto& material : materials)
	{
		delete material.second;
	}
	for (auto& shader : shaders)
	{
		delete shader.second;
	}
	for (auto& texture : textures)
	{
		delete texture.second;
	}
}

Material* GraphicsResourceManager::loadMaterial(std::string name)
{
	std::map<std::string, Material*>::iterator it = materials.find(name);
	if (it != materials.end())
	{
		return it->second;
	}
	else
	{
		//parse the file and load the material
		return loadMaterialFromFile(name);
	}
}

Mesh* GraphicsResourceManager::loadMesh(std::string name)
{
	std::map<std::string, Mesh*>::iterator it = meshes.find(name);
	if (it != meshes.end())
	{
		return it->second;
	}
	else
	{
		//parse the file and load the mesh
		return loadMeshFromFile(name);
	}
}

Shader* GraphicsResourceManager::loadShader(std::string name)
{
	std::map<std::string, Shader*>::iterator it = shaders.find(name);
	if (it != shaders.end())
	{
		return it->second;
	}
	else
	{
		//parse the file and load the shader
		return loadShaderFromFile(name);
	}
}

Texture* GraphicsResourceManager::loadTexture(std::string name)
{
	std::map<std::string, Texture*>::iterator it = textures.find(name);
	if (it != textures.end())
	{
		return it->second;
	}
	else
	{
		//parse the file and load the texture
		return loadTextureFromFile(name);
	}
}

Material* GraphicsResourceManager::loadMaterialFromFile(std::string name)
{
	std::string fullFileName = "./Data/Materials/" + name + ".mat";
	std::string line;
	std::ifstream fs(fullFileName.c_str());
	Shader* shader = nullptr;
	Texture* texture = nullptr;
	bool lit = false;
	while (std::getline(fs, line))
	{
		if (line.at(0) != '#')
		{
			std::string varName = line.substr(0, line.find('='));
			std::string varValue = line.substr(line.find('=') + 1, line.length());
			if (varName == "shader")
			{
				shader = loadShader(varValue);
			}
			else if (varName == "texture")
			{
				texture = loadTexture(varValue);
			}
			else if (varName == "lit")
			{
				std::transform(varValue.begin(), varValue.end(), varValue.begin(), ::tolower);
				lit = varValue == "true";
			}
		}
	}
	fs.close();
	Material* mat = new Material(shader, texture);
	mat->setLit(lit);
	materials.emplace(name, mat);
	return mat;
}

Mesh* GraphicsResourceManager::loadMeshFromFile(std::string name)
{
	std::string fullName = "./Data/Meshes/" + name + ".obj";
	Mesh* mesh = new Mesh();
	mesh->LoadFromObj(fullName);
	meshes.emplace(name, mesh);
	return mesh;
}

Shader* GraphicsResourceManager::loadShaderFromFile(std::string name)
{
	std::string vertexFileName = "./Data/Shaders/" + name + ".vert";
	std::string fragmentFileName = "./Data/Shaders/" + name + ".frag";
	Shader* shader = new Shader();
	shader->compileShaderFromFile(vertexFileName.c_str(), VERTEX);
	shader->compileShaderFromFile(fragmentFileName.c_str(), FRAGMENT);
	shader->linkAndValidate();
	shaders.emplace(name, shader);
	return shader;
}

Texture* GraphicsResourceManager::loadTextureFromFile(std::string name)
{
	std::string fullName = "./Data/Textures/" + name + ".tga";
	Texture* tex = new Texture(fullName.c_str());
	textures.emplace(name, tex);
	return tex;
}

void GraphicsResourceManager::addMesh(std::string name, Mesh* mesh)
{
	std::map<std::string, Mesh*>::iterator it = meshes.find(name);
	if (it != meshes.end())
	{
		string error = "GRM failed to manually add mesh: " + name + "as a mesh already exists with that name\n";
		DEBUG_PRINT(error.c_str());
	}
	else
	{
		meshes.emplace(name, mesh);
	}
}

void GraphicsResourceManager::addShader(std::string name, Shader* shader)
{
	std::map<std::string, Shader*>::iterator it = shaders.find(name);
	if (it != shaders.end())
	{
		string error = "GRM failed to manually add shader: " + name + "as a shader already exists with that name\n";
		DEBUG_PRINT(error.c_str());
	}
	else
	{
		shaders.emplace(name, shader);
	}
}

void GraphicsResourceManager::addTexture(std::string name, Texture* texture)
{
	std::map<std::string, Texture*>::iterator it = textures.find(name);
	if (it != textures.end())
	{
		string error = "GRM failed to manually add texture: " + name + "as a texture already exists with that name\n";
		DEBUG_PRINT(error.c_str());
	}
	else
	{
		textures.emplace(name, texture);
	}
}