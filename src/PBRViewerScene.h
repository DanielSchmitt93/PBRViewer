#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "PBRViewerMesh.h"

#include "PBRViewerShader.h"
#include "PBRViewerTexture.h"

#include <string>
#include <vector>
#include "PBRViewerEnumerations.h"

/// <summary>
/// This class represents a loaded 3D model selected by the user.
/// It is based on the implementation by Joey de Vries (http://www.learnopengl.com)
/// </summary>
class PBRViewerScene
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="LearnOpenGLModel"/> class.
	/// </summary>
	/// <param name="path">The filepath to the model.</param>
	explicit PBRViewerScene( std::string const& path );

	/// <summary>
	/// Disposes internal instances and frees memory.
	/// </summary>	
	GLvoid Cleanup();

	/// <summary>
	/// Draws the 3D model with the specified shader.
	/// </summary>
	/// <param name="shader">The shader which will be used to draw the 3D model.</param>	
	GLvoid Draw( std::shared_ptr<PBRViewerShader> const& shader );

	/// <summary>
	/// Adds the texture to all meshes so it can be used for rendering.
	/// </summary>
	/// <param name="textureToAdd">The texture to add.</param>	
	GLvoid AddTextureToAllMeshes( PBRViewerTexture const& textureToAdd );

	/// <summary>
	/// Removes the texture from all meshes.
	/// </summary>
	/// <param name="textureToRemove">The texture to remove.</param>	
	GLvoid RemoveTextureFromAllMeshes( PBRViewerTexture const& textureToRemove );

	/// <summary>
	/// Gets the model matrix of this 3D model.
	/// </summary>
	/// <returns>The current model matrix.</returns>
	glm::mat4 GetModelMatrix() const;

	/// <summary>
	/// Gets the up vector of this 3D model.
	/// </summary>
	/// <returns>The up vector.</returns>
	glm::vec4 GetUpVector() const;

	/// <summary>
	/// Gets the front vector of this 3D model.
	/// </summary>
	/// <returns>The up vector.</returns>
	glm::vec4 GetFrontVector() const;

	/// <summary>
	/// Gets the right vector of this 3D model.
	/// </summary>
	/// <returns>The up vector.</returns>
	glm::vec4 GetRightVector() const;

	/// <summary>
	/// Sets the model matrix for this 3D model.
	/// </summary>
	/// <param name="modelMatrix">The model matrix to set.</param>	
	GLvoid SetModelMatrix( glm::mat4 modelMatrix );

	/// <summary>
	/// Rotates the model with the given rotation matrix.
	/// </summary>
	/// <param name="rotationMatrix">The rotation matrix.</param>	
	GLvoid RotateModel(glm::mat4 rotationMatrix);

	/// <summary>
	/// Rotates the model incremental around the global x-axis.
	/// </summary>
	/// <param name="rotationAroundX">The rotation around the x-axis in degrees.</param>
	GLvoid RotateModelX(GLfloat rotationAroundX);

	/// <summary>
	/// Rotates the model incremental around the global y-axis.
	/// </summary>
	/// <param name="rotationAroundY">The rotation around the y-axis in degrees.</param>
	GLvoid RotateModelY(GLfloat rotationAroundY);

	/// <summary>
	/// Rotates the model incremental around the global z-axis.
	/// </summary>
	/// <param name="rotationAroundZ">The rotation around the z-axis in degrees.</param>
	GLvoid RotateModelZ(GLfloat rotationAroundZ);

	/// <summary>
	/// Sets the position of the loaded model.
	/// </summary>
	/// <param name="newPosition">The new position.</param>	
	GLvoid SetPosition(glm::vec3 newPosition);

	/// <summary>
	/// Scales the model incrementally.
	/// </summary>
	/// <param name="increaseValue">The value to scale.</param>
	/// <param name="scaleOperation">Either increase or decrease the model.</param>
	GLvoid ScaleIsotropically(GLfloat increaseValue, PBRViewerEnumerations::ScaleOperation scaleOperation);

	/// <summary>
	/// Gets a flag indicating if this model could be loaded and is ready for usage.
	/// </summary>
	/// <returns>True if the model is ready for usage, false if not.</returns>
	GLboolean IsReady() const;

private:
	const glm::vec4 DefaultFrontVector = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	const glm::vec4 DefaultRightVector = glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f);
	const glm::vec4 DefaultUpVector = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

	GLboolean myIsReady = GL_FALSE;

	glm::mat4 myModelMatrix = glm::identity<glm::mat4>();
	glm::vec4 myFrontVector = DefaultFrontVector;
	glm::vec4 myRightVector = DefaultRightVector;
	glm::vec4 myUpVector = DefaultUpVector;

	std::vector<PBRViewerTexture> myTextures;
	std::vector<PBRViewerMesh> myMeshes;
	std::string myDirectory;

	/// <summary>
	/// Updates the front-, right- and up vector of the model.
	/// </summary>
	GLvoid UpdateVectors();

	/// <summary>
	///  Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	/// </summary>
	/// <param name="path">The filepath to the model.</param>	
	/// <returns>True if the model could be loaded, false if not.</returns>
	GLboolean loadModel( std::string const& path );

	/// <summary>
	/// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	/// </summary>
	/// <param name="node">The node to process.</param>
	/// <param name="scene">The scene as returned by ASSIMP.</param>	
	GLvoid processNode( aiNode* node, const aiScene* scene );

	/// <summary>
	/// Extracts all properties from a mesh like vertices, texture coordinates, textures, ...
	/// </summary>
	/// <param name="mesh">The mesh to process.</param>
	/// <param name="scene">The scene as returned by ASSIMP.</param>
	/// <returns>The transformed mesh with all important properties in PBRViewer format.</returns>
	PBRViewerMesh processMesh( aiMesh* mesh, const aiScene* scene );

	/// <summary>
	/// Loads a texture from a filepath.
	/// Usually, the textures are stored in the same directory as the 3D model.
	/// </summary>
	/// <param name="path">The filepath to the texture.</param>
	/// <returns>The id of the texture.</returns>
	GLuint TextureFromFile( const GLchar* path ) const;

	/// <summary>
	/// Loads the material textures (if any).
	/// </summary>
	/// <param name="mat">The material to process.</param>
	/// <param name="type">The type of the texture to extract from the material.</param>
	/// <param name="typeName">The internal name of the texture type as string.</param>
	/// <returns>A vector containing all textures extracted from the material (if any).</returns>
	std::vector<PBRViewerTexture> loadMaterialTextures( aiMaterial* mat, aiTextureType type, const std::string& typeName );
};
