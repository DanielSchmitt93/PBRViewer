#include "PBRViewerShadows.h"
#include "PBRViewerLogger.h"

/// <summary>
/// Initializes a new instance of the <see cref="PBRViewerShadows"/> class.	
/// </summary>
/// <param name="model">The model for which shadows are to be calculated.</param>
/// <param name="nearPlane">The near plane.</param>
/// <param name="farPlane">The far plane.</param>
PBRViewerShadows::PBRViewerShadows( std::shared_ptr<PBRViewerScene> const& model,
                                    const GLfloat nearPlane,
                                    const GLfloat farPlane )
{
	myNearPlane = nearPlane;
	myFarPlane = farPlane;
	
	myModel = model;

	if (myShadowShader)
	{
		myShadowShader.reset();
	}

	myShadowShader = std::make_unique<PBRViewerShader>("SelfShadowing.vert", "SelfShadowing.frag");
	myShadowShader->Compile();
}

/// <summary>
/// Creates a vector of textures containing the depth information of the scene when rendered from the point of view of a light source.
/// </summary>
/// <param name="amountLightSources">The amount light sources.</param>
/// <param name="textureWidth">The width of the texture.</param>
/// <param name="textureHeight">The height of the texture.</param>
/// <returns>A vector of textures.</returns>
std::vector<PBRViewerTexture> PBRViewerShadows::CreateSelfShadowingTextures( const GLuint amountLightSources,
                                                                             const GLuint textureWidth,
                                                                             const GLuint textureHeight )
{
	myTextureWidth = textureWidth;
	myTextureHeight = textureHeight;

	myTextures = CreateDepthTextures(amountLightSources, textureWidth, textureHeight);
	myFramebufferObjects = CreateShadowFrameBuffers(myTextures);

	return myTextures;
}

/// <summary>
/// Renders the scene from the point of view of the light sources and stores the depth information within the associated textures.
/// IMPORTANT: Call method <see cref="CreateSelfShadowingTextures"/> before calling this method. Otherwise no depth information will be generated.
/// </summary>
/// <param name="currentViewportWidth">The current width of the viewport.</param>
/// <param name="currentViewportHeight">The current height of the viewport.</param>
/// <param name="lightSources">A vector of lights.</param>	
GLvoid PBRViewerShadows::CalculateSelfShadowing( const GLuint currentViewportWidth,
                                                 const GLuint currentViewportHeight,
                                                 const std::vector<PBRViewerPointLight>& lightSources )
{
	if (myFramebufferObjects.empty())
	{
		PBRViewerLogger::PrintErrorMessage(__FILE__, __LINE__,
		                                   "It is not possible to calculate self-shading without having created the appropriate texture beforehand.");
		return;
	}

	glCullFace(GL_FRONT);

	glViewport(0, 0, myTextureWidth, myTextureHeight);
	myShadowShader->Use();

	// Create a shadow cube map for each shadow FBO
	for (GLuint i = 0u; i < myFramebufferObjects.size(); ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, myFramebufferObjects[i]);
		glClear(GL_DEPTH_BUFFER_BIT);

		if (false == lightSources[i].GetIsActive())
		{
			// Light is not visible and, thus, does not generate shadows.
			continue;
		}

		glm::mat4 lightSpaceMatrix = GetShadowProjectionMatrix() * lookAt(lightSources[i].GetPosition(),
		                                                                  glm::vec3(0.0f),
		                                                                  glm::vec3(0.0f, 1.0f, 0.0f));

		myShadowShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		myShadowShader->setMat4("model", myModel->GetModelMatrix());
		myModel->Draw(myShadowShader);
	}

	// Reset states	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, currentViewportWidth, currentViewportHeight);
	glCullFace(GL_BACK);
}

std::vector<PBRViewerTexture> PBRViewerShadows::CreateDepthTextures( const GLuint amountLightSources,
                                                                     const GLuint textureWidth,
                                                                     const GLuint textureHeight ) const
{
	std::vector<PBRViewerTexture> shadowTextures;

	for (GLuint i = 0u; i < amountLightSources; ++i)
	{
		GLuint depthMap;
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, textureWidth, textureHeight,
		             0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Set border color of textures to white and do not repeat the texture, so that values outside of the texture
		// have a depth value of 1.0f, which means, that a fragment is not in shadow no matter which depth value it has.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		PBRViewerTexture shadowTexture;
		shadowTexture.ID = depthMap;
		shadowTexture.Type = "textureShadows";

		shadowTextures.push_back(shadowTexture);
	}

	// Reset states	
	glBindTexture(GL_TEXTURE_2D, 0);

	return shadowTextures;
}

std::vector<GLuint> PBRViewerShadows::CreateShadowFrameBuffers( std::vector<PBRViewerTexture> shadowDepthCubeMaps ) const
{
	std::vector<GLuint> shadowFBOs;
	shadowFBOs.reserve(shadowDepthCubeMaps.size());

	for (const PBRViewerTexture& shadowDepthCubeMap : shadowDepthCubeMaps)
	{
		GLuint depthMapFBO;
		glGenFramebuffers(1, &depthMapFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

		// Attach depth texture as FBO's depth buffer		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthCubeMap.ID, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		shadowFBOs.push_back(depthMapFBO);
	}

	// Reset states	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return shadowFBOs;
}

/// <summary>
/// Disposes internal instances and frees memory.
/// </summary>
GLvoid PBRViewerShadows::Cleanup()
{
	for (PBRViewerTexture& texture : myTextures)
	{
		glDeleteTextures(1, &texture.ID);
	}

	for (GLuint fbo : myFramebufferObjects)
	{
		glDeleteFramebuffers(1, &fbo);
	}
}

/// <summary>
/// Gets the width of a shadow texture.
/// All textures share the same width.
/// </summary>
/// <returns>An unsigned integer representing the width of a shadow texture.</returns>
GLuint PBRViewerShadows::GetTextureWidth() const
{
	return myTextureWidth;
}

/// <summary>
/// Gets the height of a shadow texture.
/// All textures share the same height.
/// </summary>
/// <returns>A <see cref="GLuint"/> representing the height of a shadow texture.</returns>
GLuint PBRViewerShadows::GetTextureHeight() const
{
	return myTextureHeight;
}

/// <summary>
/// Gets the near plane.
/// </summary>
/// <returns>A <see cref="GLfloat"/> representing currently set near plane.</returns>
GLfloat PBRViewerShadows::GetNearPlane() const
{
	return myNearPlane;
}

/// <summary>
/// Gets the far plane.
/// </summary>
/// <returns>A <see cref="GLfloat"/> representing currently set far plane.</returns>
GLfloat PBRViewerShadows::GetFarPlane() const
{
	return myFarPlane;
}

/// <summary>
/// Sets the near plane.
/// </summary>
/// <param name="nearPlane">The near plane.</param>	
GLvoid PBRViewerShadows::SetNearPlane( const GLfloat nearPlane )
{
	myNearPlane = nearPlane;
}

/// <summary>
/// Sets the far plane.
/// </summary>
/// <param name="farPlane">The far plane.</param>	
GLvoid PBRViewerShadows::SetFarPlane( const GLfloat farPlane )
{
	myFarPlane = farPlane;
}

/// <summary>
/// Gets the shadow projection matrix.
/// This matrix is used to render a scene from the viewpoint of a light source.
/// It can also be used to transform a vertex or fragment into light space.
/// </summary>
/// <returns>The projection matrix for the shadow calculation.</returns>
glm::mat4 PBRViewerShadows::GetShadowProjectionMatrix() const
{
	GLuint aspectHeight = myTextureHeight;
	if (0u == aspectHeight)
	{
		aspectHeight = 1u;
	}

	return glm::perspective(glm::radians(90.0f),
	                        static_cast<GLfloat>(myTextureWidth) / static_cast<GLfloat>(aspectHeight),
	                        myNearPlane,
	                        myFarPlane);
}
