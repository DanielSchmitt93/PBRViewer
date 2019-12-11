#pragma once
#include <vector>
#include <glad/glad.h>

/// <summary>
/// This class creates objects 'on-the-fly' without the need to load a 3D object.
/// </summary>
class PBRViewerObjectCreator
{
public:	
	/// <summary>
	/// Gets a vector containing the positions to form a distant cube which can be used as a skybox base.
	/// The positions are already in clip space [-1, 1].
	/// </summary>
	/// <returns>The vector containing the vertex positions for a skybox.</returns>
	static std::vector<GLfloat> GetSkyboxVertexData();
};
