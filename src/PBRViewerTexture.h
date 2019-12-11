#pragma once

#include <glad/glad.h>

#include <string>

/// <summary>
/// This struct represents a texture within PBRViewer.
/// </summary>
struct PBRViewerTexture 
{
	/// <summary>
	/// The identifier of the texture.
	/// </summary>
	GLuint ID;
	
	/// <summary>
	/// The type of the texture. E. g. 'textureDiffuse', 'textureEmissive', ...
	/// </summary>
	std::string Type;
	
	/// <summary>
	/// The filepath of the texture.
	/// </summary>
	std::string Filepath;
	
	/// <summary>
	/// Initializes a new instance of the <see cref="PBRViewerTexture"/> struct.
	/// </summary>
	PBRViewerTexture()
	{
		ID = 0u;
		Type = "";
		Filepath = "";
	}	
	
	GLboolean operator==(const PBRViewerTexture& other) const
	{
		return ID == other.ID;
	}

	GLboolean operator!=(const PBRViewerTexture& other) const
	{
		return !(this == &other);
	}
};
