#pragma once

#include <glad/glad.h>

#include "PBRViewerShader.h"
#include "PBRViewerTexture.h"
#include "PBRViewerVertex.h"

#include <vector>

/// <summary>
/// This class represents a mesh of vertices. It is used to form a three dimensional model.
/// A model consists of one or more meshes.
/// It is based on the implementation by Joey de Vries (http://www.learnopengl.com)
/// </summary>
class PBRViewerMesh
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="PBRViewerMesh"/> class.
	/// </summary>
	/// <param name="vertices">The vertices of the mesh.</param>
	/// <param name="indices">The indices of the mesh.</param>
	/// <param name="textures">The textures of the mesh.</param>
	PBRViewerMesh(const std::vector<Vertex>& vertices,
	               const std::vector<GLuint>& indices,
	               const std::vector<PBRViewerTexture>& textures );

	/// <summary>
	/// Disposes internal instances and frees memory.
	/// </summary>
	GLvoid Cleanup() const;

	/// <summary>
	/// Adds a texture to the mesh.
	/// </summary>
	/// <param name="textureToAdd">The texture to add.</param>	
	GLvoid AddTexture(PBRViewerTexture const& textureToAdd);

	/// <summary>
	/// Removes a texture from the mesh.
	/// </summary>
	/// <param name="textureToRemove">The texture to remove.</param>	
	GLvoid RemoveTexture( PBRViewerTexture const& textureToRemove );

	/// <summary>
	/// Draws the mesh with the specified shader.
	/// </summary>
	/// <param name="shader">The shader to draw.</param>	
	GLvoid Draw(std::shared_ptr<PBRViewerShader> const& shader);

private:
	std::vector<Vertex> myVertices;
	std::vector<GLuint> myIndices;
	std::vector<PBRViewerTexture> myTextures;

	GLuint myVAO = 0u;
	GLuint myVBO = 0u;
	GLuint myEBO = 0u;

	GLvoid setupMesh();
};
