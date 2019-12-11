#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

/// <summary>
/// This struct represents a vertex.
/// </summary>
struct Vertex
{
	/// <summary>
	/// The three dimensional position of the vertex.
	/// </summary>
	glm::vec3 Position;

	/// <summary>
	/// The three dimensional normal vector of the vertex.
	/// </summary>
	glm::vec3 Normal;

	/// <summary>
	/// The two dimensional texture coodinate of the vertex.
	/// </summary>
	glm::vec2 TexCoords;

	/// <summary>
	/// The three dimensional tangent vector of the vertex.
	/// </summary>
	glm::vec3 Tangent;

	/// <summary>
	/// The three dimensional bitangent vector of the vertex.
	/// </summary>
	glm::vec3 Bitangent;
};