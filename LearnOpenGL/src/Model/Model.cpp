#include "Model.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

unsigned int TextureFromFile(const char* path, const std::string& directory, GLint wrapMode = GL_REPEAT, GLint MagFilterMode = GL_LINEAR, GLint MinFilterMode = GL_LINEAR_MIPMAP_LINEAR);
GLuint TextureFromAssImp(const aiTexture* aiTex, GLint wrapMode = GL_REPEAT, GLint MagFilterMode = GL_LINEAR, GLint MinFilterMode = GL_LINEAR_MIPMAP_LINEAR);

void Model::Draw(Shader& shader)
{
	shader.use();
	for (auto& mesh : meshes)
		mesh.Draw(shader);
}

void Model::loadModel(std::string path)
{
	Assimp::Importer importer;
	//将模型的图元变换为三角形 翻转y轴
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	//文件的目录路径
	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// 处理节点所有的网格（如果有的话）
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		//索引场景mMeshes数组获取网格
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	// 接下来对它的子节点重复这一过程
	for (int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	//把assimp的mesh转换为我们的mesh 只需要复制数据 很简单
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		// normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for(int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}


	//从场景mMaterials数组中获取mesh的材质数组
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene);
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", scene);
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", scene);
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	std::vector<Texture> reflectMap = loadMaterialTextures(material, aiTextureType_REFLECTION, "texture_reflect", scene);
	textures.insert(textures.end(), reflectMap.begin(), reflectMap.end());

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene)
{
	std::vector<Texture> textures;
	for (int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		//获取纹理的文件位置
		mat->GetTexture(type, i, &str);
		str = str.C_Str() + 4;

		//纹理是否之前加载过
		if (texture_loaded.find(str.C_Str()) != texture_loaded.end())
		{
			textures.push_back(texture_loaded[str.C_Str()]);
		}
		else
		{
			Texture texture;

			auto aiTexture = scene->GetEmbeddedTexture(str.C_Str());
			if (aiTexture != nullptr)
				texture.id = TextureFromAssImp(aiTexture);
			else
				texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			texture_loaded[str.C_Str()] = texture;
		}
	}
	return textures;
}

// 加载一个纹理并返回该纹理的ID
unsigned int TextureFromFile(const char* path, const std::string& directory, GLint wrapMode, GLint MagFilterMode, GLint MinFilterMode)
{
	std::string filename = path;
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		else
			assert(0, "Invalid texture format!");

		glBindTexture(GL_TEXTURE_2D, textureID);
		//生成纹理
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//纹理环绕
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
		
		//mipmap
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilterMode);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << filename << std::endl;
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureID;
}

GLuint TextureFromAssImp(const aiTexture* aiTex, GLint wrapMode, GLint MagFilterMode, GLint MinFilterMode)
{
	if (aiTex == nullptr)
		return 0;
	unsigned int textureID = 0;
	glGenTextures(GL_TEXTURE_2D, &textureID);

	//纹理环绕
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

	//mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilterMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilterMode);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* image_data = nullptr;
	if (aiTex->mHeight == 0)
	{
		image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(aiTex->pcData), aiTex->mWidth, &width, &height, &nrChannels, 0);
	}
	else
	{
		image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(aiTex->pcData), aiTex->mWidth * aiTex->mHeight, &width, &height, &nrChannels, 0);
	}

	if (image_data != nullptr)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;
		else
			assert(0, "Invalid texture format!");

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
	}

	glGenerateMipmap(GL_TEXTURE_2D);
	return textureID;
}