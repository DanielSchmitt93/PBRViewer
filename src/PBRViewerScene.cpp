#include "PBRViewerScene.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include <stb_image.h>
#include "PBRViewerLogger.h"
#include <glm/ext/quaternion_geometric.inl>

/// <summary>
/// Initializes a new instance of the <see cref="LearnOpenGLModel"/> class.
/// </summary>
/// <param name="path">The filepath to the model.</param>
PBRViewerScene::PBRViewerScene( std::string const& path )
{
	if (loadModel(path))
	{
		myIsReady = GL_TRUE;
	}
}

/// <summary>
/// Disposes internal instances and frees memory.
/// </summary>	
GLvoid PBRViewerScene::Cleanup()
{
	// Textures
	for (auto& texture : myTextures)
	{
		glDeleteTextures(1, &texture.ID);
	}

	// Meshes
	for (auto& mesh : myMeshes)
	{
		mesh.Cleanup();
	}
}

/// <summary>
/// Draws the 3D model with the specified shader.
/// </summary>
/// <param name="shader">The shader which will be used to draw the 3D model.</param>	
GLvoid PBRViewerScene::Draw( std::shared_ptr<PBRViewerShader> const& shader )
{
	shader->Use();
	for (auto& mesh : myMeshes)
	{
		mesh.Draw(shader);
	}
}

/// <summary>
/// Adds the texture to all meshes so it can be used for rendering.
/// </summary>
/// <param name="textureToAdd">The texture to add.</param>
GLvoid PBRViewerScene::AddTextureToAllMeshes( PBRViewerTexture const& textureToAdd )
{
	for (auto& mesh : myMeshes)
	{
		mesh.AddTexture(textureToAdd);
	}
}

/// <summary>
/// Removes the texture from all meshes.
/// </summary>
/// <param name="textureToRemove">The texture to remove.</param>
GLvoid PBRViewerScene::RemoveTextureFromAllMeshes( PBRViewerTexture const& textureToRemove )
{
	for (auto& mesh : myMeshes)
	{
		mesh.RemoveTexture(textureToRemove);
	}
}

/// <summary>
/// Gets the model matrix for this 3D model.
/// </summary>
/// <returns>The current model matrix.</returns>
glm::mat4 PBRViewerScene::GetModelMatrix() const
{
	return myModelMatrix;
}

/// <summary>
/// Gets the up vector of this 3D model.
/// </summary>
/// <returns>The up vector.</returns>
glm::vec4 PBRViewerScene::GetUpVector() const
{
	return myUpVector;
}

/// <summary>
/// Gets the front vector of this 3D model.
/// </summary>
/// <returns>The up vector.</returns>
glm::vec4 PBRViewerScene::GetFrontVector() const
{
	return myFrontVector;
}

/// <summary>
/// Gets the right vector of this 3D model.
/// </summary>
/// <returns>The up vector.</returns>
glm::vec4 PBRViewerScene::GetRightVector() const
{
	return myRightVector;
}

/// <summary>
/// Sets the model matrix for this 3D model.
/// </summary>
/// <param name="modelMatrix">The model matrix to set.</param>	
GLvoid PBRViewerScene::SetModelMatrix( const glm::mat4 modelMatrix )
{
	myModelMatrix = modelMatrix;
	UpdateVectors();
}

/// <summary>
/// Rotates the model with the given rotation matrix.
/// </summary>
/// <param name="rotationMatrix">The rotation matrix.</param>	
GLvoid PBRViewerScene::RotateModel( const glm::mat4 rotationMatrix )
{
	myModelMatrix = rotationMatrix * myModelMatrix;
	UpdateVectors();
}

/// <summary>
/// Rotates the model incremental around the global x-axis.
/// </summary>
/// <param name="rotationAroundX">The rotation around the x-axis in degrees.</param>
GLvoid PBRViewerScene::RotateModelX( const GLfloat rotationAroundX )
{
	myModelMatrix = rotate(myModelMatrix, glm::radians(rotationAroundX), glm::vec3(1.0f, 0.0f, 0.0f));
	UpdateVectors();
}

/// <summary>
/// Rotates the model incremental around the global y-axis.
/// </summary>
/// <param name="rotationAroundY">The rotation around the y-axis in degrees.</param>
GLvoid PBRViewerScene::RotateModelY( const GLfloat rotationAroundY )
{
	myModelMatrix = rotate(myModelMatrix, glm::radians(rotationAroundY), glm::vec3(0.0f, 1.0f, 0.0f));
	UpdateVectors();
}

/// <summary>
/// Rotates the model incremental around the global z-axis.
/// </summary>
/// <param name="rotationAroundZ">The rotation around the z-axis in degrees.</param>
GLvoid PBRViewerScene::RotateModelZ( const GLfloat rotationAroundZ )
{
	myModelMatrix = rotate(myModelMatrix, glm::radians(rotationAroundZ), glm::vec3(0.0f, 0.0f, 1.0f));
	UpdateVectors();
}

/// <summary>
/// Sets the position of the loaded model.
/// </summary>
/// <param name="newPosition">The new position.</param>	
GLvoid PBRViewerScene::SetPosition( const glm::vec3 newPosition )
{
	const glm::mat4 modelMatrix = glm::identity<glm::mat4>();
	myModelMatrix = translate(modelMatrix, newPosition);
}

/// <summary>
/// Scales the model incrementally.
/// </summary>
/// <param name="increaseValue">The value to scale.</param>
/// <param name="scaleOperation">Either increase or decrease the model.</param>
GLvoid PBRViewerScene::ScaleIsotropically( const GLfloat increaseValue,
                                           const PBRViewerEnumerations::ScaleOperation scaleOperation )
{
	if (scaleOperation == PBRViewerEnumerations::ScaleOperation::Increase)
	{
		myModelMatrix = scale(myModelMatrix, glm::vec3(1.0f + increaseValue));
	}
	else if (scaleOperation == PBRViewerEnumerations::ScaleOperation::Decrease)
	{
		myModelMatrix = scale(myModelMatrix, glm::vec3(1.0f - increaseValue));
	}
}

/// <summary>
/// Gets a flag indicating if this model could be loaded and is ready for usage.
/// </summary>
/// <returns>True if the model is ready for usage, false if not.</returns>
GLboolean PBRViewerScene::IsReady() const
{
	return myIsReady;
}

/// <summary>
/// Updates the front-, right- and up vector of the model.
/// </summary>
GLvoid PBRViewerScene::UpdateVectors()
{
	myFrontVector = normalize(myModelMatrix * DefaultFrontVector);
	myRightVector = normalize(myModelMatrix * DefaultRightVector);
	myUpVector = normalize(myModelMatrix * DefaultUpVector);
}

/// <summary>
///  Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
/// </summary>
/// <param name="path">The filepath to the model.</param>
GLboolean PBRViewerScene::loadModel( std::string const& path )
{
	// Read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
	                                         aiProcess_Triangulate |
	                                         aiProcess_JoinIdenticalVertices |
	                                         aiProcess_SplitLargeMeshes |
	                                         aiProcess_FlipUVs |
	                                         aiProcess_OptimizeMeshes |
	                                         aiProcess_OptimizeGraph |
	                                         aiProcess_CalcTangentSpace |
	                                         aiProcess_ValidateDataStructure);

	// Check for errors
	if (nullptr == scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || nullptr == scene->mRootNode)
	{
		PBRViewerLogger::PrintErrorMessage(__FILE__, __LINE__, importer.GetErrorString());
		return GL_FALSE;
	}

	// retrieve the directory path of the filepath
	myDirectory = path.substr(0, path.find_last_of('\\'));

	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);

	return GL_TRUE;
}

/// <summary>
/// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
/// </summary>
/// <param name="node">The node to process.</param>
/// <param name="scene">The scene as returned by ASSIMP.</param>
GLvoid PBRViewerScene::processNode( aiNode* node, const aiScene* scene )
{
	// process each mesh located at the current node
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		myMeshes.push_back(processMesh(mesh, scene));
	}

	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

/// <summary>
/// Extracts all properties from a mesh like vertices, texture coordinates, textures, ...
/// </summary>
/// <param name="mesh">The mesh to process.</param>
/// <param name="scene">The scene as returned by ASSIMP.</param>
/// <returns>The transformed mesh with all important properties in PBRViewer format.</returns>
PBRViewerMesh PBRViewerScene::processMesh( aiMesh* mesh, const aiScene* scene )
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<PBRViewerTexture> textures;

	// Walk through each of the mesh's vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex{};
		glm::vec3 vector;
		// we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.

		// Vertex positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		// Vertex normals
		if (mesh->mNormals)
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		// Vertex texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;

			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		if (mesh->mTangents)
		{
			glm::vec3 tangent;

			tangent.x = mesh->mTangents[i].x;
			tangent.y = mesh->mTangents[i].y;
			tangent.z = mesh->mTangents[i].z;

			vertex.Tangent = tangent;
		}

		if (mesh->mBitangents)
		{
			glm::vec3 bitangent;

			bitangent.x = mesh->mBitangents[i].x;
			bitangent.y = mesh->mBitangents[i].y;
			bitangent.z = mesh->mBitangents[i].z;

			vertex.Bitangent = bitangent;
		}

		vertices.push_back(vertex);
	}

	// Now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		// retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	// 1. diffuse (albedo) maps
	std::vector<PBRViewerTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "textureDiffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	// 2. normal maps
	std::vector<PBRViewerTexture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "textureNormal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

	// 3. Roughness maps (also include AO and metallic components on different color channels)
	std::vector<PBRViewerTexture> roughnessMaps = loadMaterialTextures(material, aiTextureType_UNKNOWN, "textureRoughness");
	textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());

	// 4. Emissive maps
	std::vector<PBRViewerTexture> emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "textureEmissive");
	textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());

	// return a mesh object created from the extracted mesh data
	return PBRViewerMesh(vertices, indices, textures);
}

/// <summary>
/// Loads a texture from a filepath.
/// Usually, the textures are stored in the same directory as the 3D model.
/// </summary>
/// <param name="path">The filepath to the texture.</param>
/// <returns>The id of the texture.</returns>
GLuint PBRViewerScene::TextureFromFile( const GLchar* path ) const
{
	std::string filename = std::string(path);
	filename = myDirectory + '\\' + filename;

	GLuint textureID;
	glGenTextures(1, &textureID);

	GLint width, height, nrComponents;	
	GLubyte* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = GL_RED;
		if (nrComponents == 1)
		{
			format = GL_RED;
		}
		else if (nrComponents == 3)
		{
			format = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::string message = "Texture failed to load at path: ";
		message += path;
		PBRViewerLogger::PrintErrorMessage(__FILE__, __LINE__, message);
		stbi_image_free(data);
	}

	return textureID;
}

/// <summary>
/// Loads the material textures (if any).
/// </summary>
/// <param name="mat">The material to process.</param>
/// <param name="type">The type of the texture to extract from the material.</param>
/// <param name="typeName">The internal name of the texture type as string.</param>
/// <returns>A vector containing all textures extracted from the material (if any).</returns>
std::vector<PBRViewerTexture> PBRViewerScene::loadMaterialTextures( aiMaterial* mat,
                                                                    const aiTextureType type,
                                                                    const std::string& typeName )
{
	std::vector<PBRViewerTexture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); ++i)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		GLboolean textureAlreadyLoaded = GL_FALSE;
		for (auto& loadedTexture : textures)
		{
			if (std::strcmp(loadedTexture.Filepath.data(), str.C_Str()) == 0)
			{
				textureAlreadyLoaded = GL_TRUE;
				break;
			}
		}

		// if texture hasn't been loaded already, load it
		if (GL_FALSE == textureAlreadyLoaded)
		{
			PBRViewerTexture texture;
			texture.ID = TextureFromFile(str.C_Str());
			texture.Type = typeName;
			texture.Filepath = str.C_Str();
			textures.push_back(texture);
			myTextures.push_back(texture);
			// store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}

	return textures;
}
