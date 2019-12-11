#pragma once
#include <vector>
#include <glad/glad.h>

#include "PBRViewerShader.h"

/// <summary>
/// This class contains various utility functions used in several classes of PBRViewer.
/// </summary>
class PBRViewerOpenGLUtilities
{
public:
	/// <summary>
	/// Renders a full screen quad to the screen.
	/// </summary>
	static GLvoid RenderFullScreenQuad();

	/// <summary>
	/// Renders a quarter quad at the bottom right of the screen.
	/// </summary>
	static GLvoid RenderQuadBottomRight();

	/// <summary>
	/// Gets six view matrices where each one is looking at a different side of a cube.
	/// The order corresponds to the cube map texture ordering as defined in glad.h.
	/// </summary>
	/// <returns>A vector of six view matrices.</returns>
	static std::vector<glm::mat4> GetCaptureViewsForCubeMap();	

	/// <summary>
	/// Shows a texture at the bottom right of the screen.
	/// </summary>
	/// <param name="textureId">The id of the texture to show.</param>
	/// <param name="shader">The shader to use.</param>	
	static GLvoid ShowTextureBottomRight(GLuint textureId, PBRViewerShader shader);

private:
	static GLvoid RenderQuad( GLuint vao );
};
