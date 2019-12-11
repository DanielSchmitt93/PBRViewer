#include "PBRViewerMesh.h"

/// <summary>
/// Initializes a new instance of the <see cref="PBRViewerMesh"/> class.
/// </summary>
/// <param name="vertices">The vertices of the mesh.</param>
/// <param name="indices">The indices of the mesh.</param>
/// <param name="textures">The textures of the mesh.</param>
PBRViewerMesh::PBRViewerMesh( const std::vector<Vertex>& vertices,
                              const std::vector<GLuint>& indices,
                              const std::vector<PBRViewerTexture>& textures )
{
	myVertices = vertices;
	myIndices = indices;
	myTextures = textures;	
	
	setupMesh();
}

/// <summary>
/// Disposes internal instances and frees memory.
/// </summary>
GLvoid PBRViewerMesh::Cleanup() const
{
	glDeleteVertexArrays(1, &myVAO);
	glDeleteBuffers(1, &myVBO);
	glDeleteBuffers(1, &myEBO);
}

/// <summary>
/// Adds a texture to the mesh.
/// </summary>
/// <param name="textureToAdd">The texture to add.</param>	
GLvoid PBRViewerMesh::AddTexture( PBRViewerTexture const& textureToAdd )
{
	// Do not add duplicate textures.
	if (std::find(myTextures.begin(), myTextures.end(), textureToAdd) != myTextures.end())
	{
		return;
	}

	myTextures.push_back(textureToAdd);
}

/// <summary>
/// Removes a texture from the mesh.
/// </summary>
/// <param name="textureToRemove">The texture to remove.</param>	
GLvoid PBRViewerMesh::RemoveTexture( PBRViewerTexture const& textureToRemove )
{
	const auto it = std::find(myTextures.begin(), myTextures.end(), textureToRemove);
	if (it != myTextures.end())
	{
		myTextures.erase(it);
	}
}

/// <summary>
/// Draws the mesh with the specified shader.
/// </summary>
/// <param name="shader">The shader to draw.</param>	
GLvoid PBRViewerMesh::Draw( std::shared_ptr<PBRViewerShader> const& shader )
{
	// Materials
	GLuint diffuseNr = 0u;
	GLuint normalNr = 0u;
	GLuint roughnessNr = 0u;
	GLuint emissiveNr = 0u;

	// Image Based Lighting
	GLuint irradianceNr = 0u;
	GLuint preFilteredEnvironmentNr = 0u;
	GLuint brdfLookupNr = 0u;

	// Shadows
	GLuint shadowNr = 0u;

	for (GLuint i = 0u; i < myTextures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		std::string variableName;
		std::string name = myTextures[i].Type;

		if (name == "textureDiffuse")
		{
			glBindTexture(GL_TEXTURE_2D, myTextures[i].ID);
			variableName = name.append("[").append(std::to_string(diffuseNr++)).append("]");
			shader->setBool("textureDiffuseAvailable", GL_TRUE);
		}
		else if (name == "textureNormal")
		{
			glBindTexture(GL_TEXTURE_2D, myTextures[i].ID);
			variableName = name.append("[").append(std::to_string(normalNr++)).append("]");
			shader->setBool("textureNormalAvailable", GL_TRUE);
		}
		else if (name == "textureRoughness")
		{
			glBindTexture(GL_TEXTURE_2D, myTextures[i].ID);
			variableName = name.append("[").append(std::to_string(roughnessNr++)).append("]");
			shader->setBool("textureRoughnessAvailable", GL_TRUE);
		}
		else if (name == "textureEmissive")
		{
			glBindTexture(GL_TEXTURE_2D, myTextures[i].ID);
			variableName = name.append("[").append(std::to_string(emissiveNr++)).append("]");
			shader->setBool("textureEmissiveAvailable", GL_TRUE);
		}
		else if (name == "textureIrradiance")
		{
			// The irradiance map is a cubemap texture and not a plain 2D one.
			glBindTexture(GL_TEXTURE_CUBE_MAP, myTextures[i].ID);
			variableName = name.append("[").append(std::to_string(irradianceNr++)).append("]");
			shader->setBool("textureIrradianceAvailable", GL_TRUE);
		}
		else if (name == "texturePreFilterEnvironment")
		{
			// The prefiltered environment map is a cubemap texture and not a plain 2D one.
			glBindTexture(GL_TEXTURE_CUBE_MAP, myTextures[i].ID);
			variableName = name.append("[").append(std::to_string(preFilteredEnvironmentNr++)).append("]");
			shader->setBool("texturePrefilteredEnvironmentAvailable", GL_TRUE);
		}
		else if (name == "textureBRDFLookup")
		{
			glBindTexture(GL_TEXTURE_2D, myTextures[i].ID);
			variableName = name.append("[").append(std::to_string(brdfLookupNr++)).append("]");
			shader->setBool("textureBRDFLookupAvailable", GL_TRUE);
		}
		else if (name == "textureShadows")
		{
			glBindTexture(GL_TEXTURE_2D, myTextures[i].ID);
			variableName = name.append("[").append(std::to_string(shadowNr++)).append("]");
			shader->setBool("textureShadowsAvailable", GL_TRUE);
		}

		shader->setInt(variableName, i);
	}

	// Draw mesh
	glBindVertexArray(myVAO);
	glDrawElements(GL_TRIANGLES, static_cast<GLint>(myIndices.size()), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	// Reset states
	glActiveTexture(GL_TEXTURE0);
	shader->setBool("textureDiffuseAvailable", GL_FALSE);
	shader->setBool("textureNormalAvailable", GL_FALSE);
	shader->setBool("textureRoughnessAvailable", GL_FALSE);
	shader->setBool("textureEmissiveAvailable", GL_FALSE);

	shader->setBool("textureIrradianceAvailable", GL_FALSE);
	shader->setBool("textureBRDFLookupAvailable", GL_FALSE);
	shader->setBool("texturePrefilteredEnvironmentAvailable", GL_FALSE);

	shader->setBool("textureShadowsAvailable", GL_FALSE);
}

GLvoid PBRViewerMesh::setupMesh()
{
	glGenVertexArrays(1, &myVAO);
	glGenBuffers(1, &myVBO);
	glGenBuffers(1, &myEBO);

	glBindVertexArray(myVAO);

	// Load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, myVBO);	
	glBufferData(GL_ARRAY_BUFFER, sizeof myVertices[0] * myVertices.size(), myVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof myIndices[0] * myIndices.size(), myIndices.data(), GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<GLvoid*>(nullptr));

	// Vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, Normal)));

	// Vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, TexCoords)));

	// Tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, Tangent)));

	// Bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, Bitangent)));

	// Reset states
	glBindVertexArray(0);
}
