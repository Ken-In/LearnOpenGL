#pragma once

#include <assimp/scene.h>

#include <map>

#include "Mesh.h"

class Model
{
public:
	std::map<std::string, Texture> texture_loaded;
	std::vector<Mesh> meshes;
	std::string directory;

	Model(const char* path)
	{
		loadModel(path);
	}

	void Draw(Shader& shader);

private:
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene);
};