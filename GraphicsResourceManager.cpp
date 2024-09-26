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

void GraphicsResourceManager::initialseBasicResources()
{
	//We have some default resources that we should always load/generate rather than waiting for something to request it
	//Note that we can't call this in the ctor because we need to wait for openGL to be initialised first

	//First load the resources that are loaded from files
	
	//Start with our default material, which we also keep a reference to for later
	defaultMaterial = loadMaterial("DefaultLit");	//This will automatically load our default shader

	//Then generate the resources that are generated programmatically

	//1. Unit Cube
	std::vector<ColouredVertex> vertices;
	std::vector<unsigned int> indices;
	MeshTools::addCuboid(&vertices, &indices, glm::vec3(0.5f, 0.5f, 0.5f), 1.0f, 1.0f, 1.0f);
	Mesh* cube = new Mesh("unit_cube", vertices, indices);
	addMesh("unit_cube", cube);

	//2. Unit Plane
	vertices.clear();
	indices.clear();
	MeshTools::addQuad(&vertices, &indices, glm::vec3(), 1.0f, 1.0f);
	Mesh* plane = new Mesh("unit_plane", vertices, indices);
	addMesh("unit_plane", plane);
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
		Material* material = loadMaterialFromFile(name);
		if (material == nullptr)
		{
			//We couldn't find the material, good thing we have a default material to fall back on
			return defaultMaterial;
		}
		return material;
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
		//parse the file and load the mesh. This will return nullptr if the file does not exist
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
	if (std::filesystem::exists(fullFileName))
	{
		std::string line;
		std::ifstream fs(fullFileName.c_str());
		Shader* shader = nullptr;
		Texture* texture = nullptr;
		bool lit = false;
		bool transparent = false;
		std::map<int, float> floatProperties;
		std::map<int, glm::vec4> vectorProperties;
		while (std::getline(fs, line))
		{
			if (line.at(0) != '#')
			{
				std::string varName = line.substr(0, line.find('='));
				std::string varValue = line.substr(line.find('=') + 1, line.length());
				//First check for the generic properties
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
				else if (varName == "transparent")
				{
					std::transform(varValue.begin(), varValue.end(), varValue.begin(), ::tolower);
					transparent = varValue == "true";
				}
				else
				{
					//we're looking at one of our non-generic properties
					//first identify its type, then add to the map
					if (varValue[0] == 'f')
					{
						int loc = shader->getUniformLocation(varName.c_str());
						float val = static_cast<float>(atof(varValue.substr(2, varValue.length() - 1).c_str()));
						floatProperties[loc] = val;
					}
					else if (varValue[0] == 'v')
					{
						int loc = shader->getUniformLocation(varName.c_str());
						glm::vec4 val = parseVector(varValue.substr(2, varValue.length() - 1));
						vectorProperties[loc] = val;
					}
					else
					{
						DEBUG_PRINT("Failed to parse material property: " + line + "\n");
					}
				}
			}
		}
		fs.close();
		Material* mat = new Material(name, shader, texture);
		mat->floatProperties = floatProperties;
		mat->vectorProperties = vectorProperties;
		mat->setLit(lit);
		mat->enableBlending = transparent;
		materials.emplace(name, mat);
		return mat;
	}
	DEBUG_PRINT("Material file not found: " + fullFileName + "\n");
	return nullptr;
}

Mesh* GraphicsResourceManager::loadMeshFromFile(std::string name)
{
	std::string fullName = "./Data/Meshes/" + name + ".obj";
	//Check if the file exists
	if (std::filesystem::exists(fullName))
	{
		Mesh* mesh = new Mesh();
		mesh->LoadFromObj(fullName);
		meshes.emplace(name, mesh);
		return mesh;
	}
	//File not found, return nothing
	DEBUG_PRINT("Mesh file not found: " + fullName + "\n");
	return nullptr;
}

Shader* GraphicsResourceManager::loadShaderFromFile(std::string name)
{
	std::string vertexFileName = "./Data/Shaders/" + name + ".vert";
	std::string fragmentFileName = "./Data/Shaders/" + name + ".frag";
	if (std::filesystem::exists(vertexFileName) && std::filesystem::exists(fragmentFileName))
	{
		Shader* shader = new Shader();
		shader->compileShaderFromFile(vertexFileName.c_str(), VERTEX);
		shader->compileShaderFromFile(fragmentFileName.c_str(), FRAGMENT);
		shader->linkAndValidate();
		shaders.emplace(name, shader);
		return shader;
	}
	//File not found, return nothing
	DEBUG_PRINT("One or more parts of Shader file not found: " + name + "\n");
	return nullptr;
}

Texture* GraphicsResourceManager::loadTextureFromFile(std::string name)
{
	std::string fullName = "./Data/Textures/" + name + ".tga";
	if (std::filesystem::exists(fullName))
	{
		Texture* tex = new Texture(fullName.c_str());
		textures.emplace(name, tex);
		return tex;
	}
	else
	{
		DEBUG_PRINT("Texture file not found: " + fullName + "\n");
		return nullptr;
	}
}

void GraphicsResourceManager::addMesh(std::string name, Mesh* mesh)
{
	std::map<std::string, Mesh*>::iterator it = meshes.find(name);
	if (it != meshes.end())
	{
		DEBUG_PRINT("GRM failed to manually add mesh: " + name + "as a mesh already exists with that name\n");
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
		DEBUG_PRINT("GRM failed to manually add shader: " + name + "as a shader already exists with that name\n");
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
		DEBUG_PRINT("GRM failed to manually add texture: " + name + "as a texture already exists with that name\n");
	}
	else
	{
		textures.emplace(name, texture);
	}
}