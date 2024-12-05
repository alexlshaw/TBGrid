#pragma once

#include "assimp/scene.h"
#include "Bone.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include <map>
#include <string>
#include <vector>

class AnimatedModel
{
private:
	std::map<std::string, BoneInfo> boneInfoMap;
	int boneCounter = 0;
	void loadModel(const std::string& path);
	//Recursively processes nodes in the scene
	void processNode(aiNode* node, const aiScene* scene);
	void setVertexBoneDataToDefault(Vertex& vertex);
	Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
	void setVertexBoneData(Vertex& vertex, int boneID, float weight);
	void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
	std::vector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
public:
	std::string name;
	std::vector<Material*> materials;	//One material per mesh, in theory
	std::vector<Mesh*> meshes;
	std::string directory;
	bool gammaCorrection;
	AnimatedModel(const std::string& name, const std::string& path, bool gamma = false);
	void draw(size_t subMeshIndex);
	std::map<std::string, BoneInfo>& getBoneInfoMap();
	int& getBoneCount();
};