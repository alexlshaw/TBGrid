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
	for (auto& mesh : meshes)
	{
		delete mesh.second;
	}
}

void GraphicsResourceManager::initialseBasicResources()
{
	//We have some default resources that we should always load/generate rather than waiting for something to request it
	//Note that we can't call this in the ctor because we need to wait for openGL to be initialised first

	//First load the resources that are loaded from files
	
	//Start with our default fallback resources
	defaultMaterial = loadMaterial("DefaultLit");	//This will also automatically load our default shader
	defaultFont = loadFont("Poly-Regular.otf");
	defaultWhite = loadTexture("WhitePlain");

	//Then add the resources that are generated programmatically

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

	//3. Unit Sphere
	vertices.clear();
	indices.clear();
	MeshTools::createSphere(glm::vec3(), 1.0f, 8, 8, &vertices, &indices);
	Mesh* sphere = new Mesh("unit_sphere", vertices, indices);
	addMesh("unit_sphere", sphere);
}

FontAtlas* GraphicsResourceManager::loadFont(std::string name)
{
	std::map<std::string, FontAtlas*>::iterator it = fonts.find(name);
	if (it != fonts.end())
	{
		return it->second;
	}
	else
	{
		//parse the file and load the material
		FontAtlas* font = loadFontFromFile(name);
		if (font == nullptr)
		{
			//We couldn't find the material, good thing we have a default material to fall back on
			return defaultFont;
		}
		return font;
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

FontAtlas* GraphicsResourceManager::loadFontFromFile(std::string name)
{
	std::string fullName = "./Data/Fonts/" + name;	//No automatic extension, because there are several possible extensions
	if (std::filesystem::exists(fullName))
	{
		FontAtlas* font = new FontAtlas(fullName);
		fonts.emplace(name, font);
		return font;
	}
	else
	{
		DEBUG_PRINTLN("Font file not found: " + fullName);
		return nullptr;
	}
}

Material* GraphicsResourceManager::loadMaterialFromFile(std::string name)
{
	//This makes a few assumptions about the layour of the material file:
	//1: The shader will be specified before the texture
	//2: Shader and texture will be specified before the standard and non-standard properties

	std::string fullFileName = "./Data/Materials/" + name + ".mat";
	if (std::filesystem::exists(fullFileName))
	{
		Material* mat = new Material(name);
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

void GraphicsResourceManager::addMesh(const std::string& name, Mesh* mesh)
{
	if (hasMesh(name))
	{
		DEBUG_PRINT("GRM failed to manually add mesh: " + name + "as a mesh already exists with that name\n");
	}
	else
	{
		meshes.emplace(name, mesh);
	}
}

void GraphicsResourceManager::addShader(const std::string& name, Shader* shader)
{
	if (hasShader(name))
	{
		DEBUG_PRINT("GRM failed to manually add shader: " + name + "as a shader already exists with that name\n");
	}
	else
	{
		shaders.emplace(name, shader);
	}
}

void GraphicsResourceManager::addTexture(const std::string& name, Texture* texture)
{
	if (hasTexture(name))
	{
		DEBUG_PRINT("GRM failed to manually add texture: " + name + "as a texture already exists with that name\n");
	}
	else
	{
		textures.emplace(name, texture);
	}
}

void GraphicsResourceManager::addMaterial(const std::string& name, Material* material)
{
	if (hasMaterial(name))
	{
		DEBUG_PRINTLN("GRM failed to manually add material: " + name + "as a material already exists with that name");
	}
	else
	{
		materials.emplace(name, material);
	}
}

bool GraphicsResourceManager::hasMesh(const std::string& name) const
{
	return meshes.find(name) != meshes.end();
}

bool GraphicsResourceManager::hasShader(const std::string& name) const
{
	return shaders.find(name) != shaders.end();
}

bool GraphicsResourceManager::hasTexture(const std::string& name) const
{
	return textures.find(name) != textures.end();
}

bool GraphicsResourceManager::hasMaterial(const std::string& name) const
{
	return materials.find(name) != materials.end();
}

void GraphicsResourceManager::deleteTexture(std::string name)
{
	auto it = textures.find(name);
	if (it != textures.end())
	{
		Texture* tex = it->second;
		textures.erase(name);
		delete tex;
	}
	else
	{
		DEBUG_PRINTLN("Attempting to delete nonexistent texture: " + name);
	}
}