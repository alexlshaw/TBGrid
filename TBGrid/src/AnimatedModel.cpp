#include "AnimatedModel.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include <format>
#include "GraphicsResourceManager.h"
#include "Utilities.h"

AnimatedModel::AnimatedModel(const std::string& name, const std::string& path, bool gamma)
	: gammaCorrection(gamma),
	name(name)
{
	loadModel(path);
}

void AnimatedModel::draw(size_t subMeshIndex)
{
	meshes[subMeshIndex]->draw();
}

std::map<std::string, BoneInfo>& AnimatedModel::getBoneInfoMap()
{
	return boneInfoMap;
}

int& AnimatedModel::getBoneCount()
{
	return boneCounter;
}

void AnimatedModel::loadModel(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
	//check that model loaded successfully
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		DEBUG_PRINTLN(std::format("Error loading animated model: {}", path));
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));	//TODO: What is directory used for?
	processNode(scene->mRootNode, scene);
}

void AnimatedModel::processNode(aiNode* node, const aiScene* scene)
{
	//First process meshes in the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		//pull data from the scene (the node contains indices to refer to the scene's objects rather than the actual data)
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	//Then recursively process child nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

void AnimatedModel::setVertexBoneDataToDefault(Vertex& vertex)
{
	for (int i = 0; i < AnimationConstants::MAX_BONE_INFLUENCE; i++)
	{
		vertex.boneIDs[i] = -1;
		vertex.boneWeights[i] = 0.0f;
	}
}

Mesh* AnimatedModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture*> textures;

	//Process the vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex v{};
		setVertexBoneDataToDefault(v);
		v.position = AssimpGLMHelpers::convertVec3ToGLM(mesh->mVertices[i]);
		v.normal = AssimpGLMHelpers::convertVec3ToGLM(mesh->mNormals[i]);
		if (mesh->mTextureCoords[0])
		{
			v.texCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].x };
		}
		else
		{
			v.texCoords = { 0.0f, 0.0f };	//Should be unneccessary, but just in case
		}
		vertices.push_back(v);
	}
	//process the indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	//process materials, TODO: move this stuff out of animatedModel and into Material
	aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture*> diffuseMaps = loadMaterialTextures(aiMat, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	std::vector<Texture*> specularMaps = loadMaterialTextures(aiMat, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	//TODO: These two are either named oddly, or a mistake in the example code
	std::vector<Texture*> normalMaps = loadMaterialTextures(aiMat, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	std::vector<Texture*> heightMaps = loadMaterialTextures(aiMat, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	//construct our own material
	Material* material = new Material(aiMat);
	if (diffuseMaps.size() > 0)
	{
		material->diffuseMap = diffuseMaps[0];
	}
	if (specularMaps.size() > 0)
	{
		material->specularMap = specularMaps[0];
	}
	//pass it to the GRM to keep track of
	GraphicsResourceManager::getInstance().addMaterial(material->name, material);
	materials.push_back(material);

	extractBoneWeightForVertices(vertices, mesh, scene);
	std::string meshName = mesh->mName.C_Str();
	Mesh* finalMesh = new Mesh(meshName, vertices, indices);
	GraphicsResourceManager::getInstance().addMesh(finalMesh->name, finalMesh);
	return finalMesh;
}

void AnimatedModel::setVertexBoneData(Vertex& vertex, int boneID, float weight)
{
	//Iterate over the vertices "bone weight slots" until we find an empty one, then use that one for this bone data
	for (int i = 0; i < AnimationConstants::MAX_BONE_INFLUENCE; i++)
	{
		if (vertex.boneIDs[i] < 0)
		{
			vertex.boneWeights[i] = weight;
			vertex.boneIDs[i] = boneID;
			break;
		}
	}
}

void AnimatedModel::extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
	auto& boneMap = boneInfoMap;	//TODO: Do I really need a ref here? Pretty sure I can just use the class member directly
	int boneCount = boneCounter;

	for (unsigned int boneIdx = 0; boneIdx < mesh->mNumBones; boneIdx++)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIdx]->mName.C_Str();
		if (boneMap.find(boneName) == boneMap.end())
		{
			//bone is not currently in our map, add it
			BoneInfo newBoneInfo{};
			newBoneInfo.id = boneCount;
			newBoneInfo.offset = AssimpGLMHelpers::convertMatrixToGLM(mesh->mBones[boneIdx]->mOffsetMatrix);
			boneMap[boneName] = newBoneInfo;
			boneID = boneCount;
			boneCount++;
		}
		else
		{
			//bone is in our map, update the id
			boneID = boneMap[boneName].id;
		}
		//Check for error, then process weights
		if (boneID == -1)
		{
			DEBUG_PRINTLN(std::format("Error loading mesh: {}, failed to find bone: {}", mesh->mName.C_Str(), boneName));
		}
		else
		{
			auto weights = mesh->mBones[boneIdx]->mWeights;
			int numWeights = mesh->mBones[boneIdx]->mNumWeights;

			for (int w = 0; w < numWeights; w++)
			{
				int vertexID = weights[w].mVertexId;
				float weight = weights[w].mWeight;
				setVertexBoneData(vertices[vertexID], boneID, weight);
			}
		}
	}
}

std::vector<Texture*> AnimatedModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture*> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		//example code has some rules around skipping already loaded textures, but the GRM already takes care of that
		GraphicsResourceManager& resourceManager = GraphicsResourceManager::getInstance();
		string textureName = str.C_Str();
		Texture* texture = resourceManager.loadTexture(textureName);
		//TODO: Not tracking typename in my current texture implementation, but it would be a useful thing to have
		textures.push_back(texture);
	}
	return textures;
}
