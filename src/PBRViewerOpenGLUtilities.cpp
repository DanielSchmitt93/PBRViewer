# include "PBRViewerOpenGLUtilities.h"

#include <glad/glad.h>
#include <glm/ext/matrix_transform.inl>

/// <summary>
/// Renders a full screen quad to the screen.
/// </summary>
GLvoid PBRViewerOpenGLUtilities::RenderFullScreenQuad()
{
	static GLuint myFullScreenQuadVAO;

	if (myFullScreenQuadVAO == 0)
	{
		GLuint quadVBO;

		GLfloat quadVertices[] = {
			// positions        // texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		// setup plane VAO
		glGenVertexArrays(1, &myFullScreenQuadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(myFullScreenQuadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof quadVertices, &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));
	}

	RenderQuad(myFullScreenQuadVAO);
}

/// <summary>
/// Renders a quarter quad at the bottom right of the screen.
/// </summary>
GLvoid PBRViewerOpenGLUtilities::RenderQuadBottomRight()
{
	static GLuint myQuarterQuadTopRightVAO;

	if (myQuarterQuadTopRightVAO == 0)
	{
		GLuint quadVBO;

		GLfloat quadVertices[] = {
			// positions       
			0.5f, -0.5f, 0.0f,
			0.5f, -1.0f, 0.0f,
			1.0f, -0.5f, 0.0f,
			1.0f, -1.0f, 0.0f
		};

		// setup plane VAO
		glGenVertexArrays(1, &myQuarterQuadTopRightVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(myQuarterQuadTopRightVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof quadVertices, &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
	}

	RenderQuad(myQuarterQuadTopRightVAO);
}

/// <summary>
/// Gets six view matrices where each one is looking at a different side of a cube.
/// The order corresponds to the cube map texture ordering as defined in glad.h.
/// </summary>
/// <returns>A vector of six view matrices.</returns>
std::vector<glm::mat4> PBRViewerOpenGLUtilities::GetCaptureViewsForCubeMap()
{
	static std::vector<glm::mat4> captureViews =
	{
		// GL_TEXTURE_CUBE_MAP_POSITIVE_X
		lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
		       glm::vec3(1.0f, 0.0f, 0.0f),
		       glm::vec3(0.0f, -1.0f, 0.0f)),

		// GL_TEXTURE_CUBE_MAP_NEGATIVE_X
		lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
		       glm::vec3(-1.0f, 0.0f, 0.0f),
		       glm::vec3(0.0f, -1.0f, 0.0f)),

		// GL_TEXTURE_CUBE_MAP_POSITIVE_Y
		lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
		       glm::vec3(0.0f, 1.0f, 0.0f),
		       glm::vec3(0.0f, 0.0f, 1.0f)),

		// GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
		lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
		       glm::vec3(0.0f, -1.0f, 0.0f),
		       glm::vec3(0.0f, 0.0f, -1.0f)),

		// GL_TEXTURE_CUBE_MAP_POSITIVE_Z
		lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
		       glm::vec3(0.0f, 0.0f, 1.0f),
		       glm::vec3(0.0f, -1.0f, 0.0f)),

		// GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
		lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
		       glm::vec3(0.0f, 0.0f, -1.0f),
		       glm::vec3(0.0f, -1.0f, 0.0f))
	};

	return captureViews;
}

/// <summary>
/// Shows a texture at the bottom right of the screen.
/// </summary>
/// <param name="textureId">The id of the texture to show.</param>
/// <param name="shader">The shader to use.</param>	
GLvoid PBRViewerOpenGLUtilities::ShowTextureBottomRight( const GLuint textureId, PBRViewerShader shader )
{
	shader.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
	shader.setInt("textureBottomRight", 0);

	RenderQuadBottomRight();
}

GLvoid PBRViewerOpenGLUtilities::RenderQuad( const GLuint vao )
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
