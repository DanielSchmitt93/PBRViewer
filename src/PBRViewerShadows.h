#pragma once
#include <vector>
#include "PBRViewerTexture.h"
#include "PBRViewerScene.h"
#include "PBRViewerPointLight.h"

/// <summary>
/// This class is responsible to create shadow textures which can be used for shadow coverage calculations within the shader components.
/// Shadows will be calculated within the specified near- and far plane. The size of the texture is adjustable.
/// Objects within PBRViewer will not cast a shadow on the floor (because there is no floor at all) but they can be used for self-shadowing.
/// Some normal vectors could be pointing torwards the light source [dot(l, n) > 0.0f] despite the light should be blocked by some other geometry.
/// We can reduce the light received by a point based on shadow coverage calculations and obtain a more realistic image.
/// The scene will be rendered from the point of view of a light source. The depth information will be saved within a 2D texture.
/// If the current depth of a fragment is greater than the depth value stored within the texture, the fragment is obscured by other geometry.
/// </summary>
class PBRViewerShadows
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="PBRViewerShadows"/> class.	
	/// </summary>
	/// <param name="model">The model for which shadows are to be calculated.</param>
	/// <param name="nearPlane">The near plane.</param>
	/// <param name="farPlane">The far plane.</param>
	PBRViewerShadows(std::shared_ptr<PBRViewerScene> const& model,
	                  GLfloat nearPlane = 0.1f,
	                  GLfloat farPlane = 2.0f );

	/// <summary>
	/// Creates a vector of textures containing the depth information of the scene when rendered from the point of view of a light source.
	/// </summary>
	/// <param name="amountLightSources">The amount light sources.</param>
	/// <param name="textureWidth">The width of the texture.</param>
	/// <param name="textureHeight">The height of the texture.</param>
	/// <returns>A vector of textures.</returns>
	std::vector<PBRViewerTexture> CreateSelfShadowingTextures(GLuint amountLightSources,
	                                                           GLuint textureWidth = 4096u,
	                                                           GLuint textureHeight = 4096u );

	/// <summary>
	/// Renders the scene from the point of view of the light sources and stores the depth information within the associated textures.
	/// IMPORTANT: Call method <see cref="CreateSelfShadowingTextures"/> before calling this method. Otherwise no depth information will be generated.
	/// </summary>
	/// <param name="currentViewportWidth">The current width of the viewport.</param>
	/// <param name="currentViewportHeight">The current height of the viewport.</param>
	/// <param name="lightSources">A vector of lights.</param>	
	GLvoid CalculateSelfShadowing(GLuint currentViewportWidth,
	                               GLuint currentViewportHeight,
	                               const std::vector<PBRViewerPointLight>& lightSources );

	/// <summary>
	/// Gets the width of a shadow texture.
	/// All textures share the same width.
	/// </summary>
	/// <returns>An unsigned integer representing the width of a shadow texture.</returns>
	GLuint GetTextureWidth() const;

	/// <summary>
	/// Gets the height of a shadow texture.
	/// All textures share the same height.
	/// </summary>
	/// <returns>A <see cref="GLuint"/> representing the height of a shadow texture.</returns>
	GLuint GetTextureHeight() const;

	/// <summary>
	/// Gets the near plane.
	/// </summary>
	/// <returns>A <see cref="GLfloat"/> representing currently set near plane.</returns>
	GLfloat GetNearPlane() const;

	/// <summary>
	/// Gets the far plane.
	/// </summary>
	/// <returns>A <see cref="GLfloat"/> representing currently set far plane.</returns>
	GLfloat GetFarPlane() const;

	/// <summary>
	/// Sets the near plane.
	/// </summary>
	/// <param name="nearPlane">The near plane.</param>	
	GLvoid SetNearPlane(GLfloat nearPlane);

	/// <summary>
	/// Sets the far plane.
	/// </summary>
	/// <param name="farPlane">The far plane.</param>	
	GLvoid SetFarPlane(GLfloat farPlane);

	/// <summary>
	/// Gets the shadow projection matrix.
	/// This matrix is used to render a scene from the viewpoint of a light source.
	/// It can also be used to transform a vertex or fragment into light space.
	/// </summary>
	/// <returns>The projection matrix for the shadow calculation.</returns>
	glm::mat4 GetShadowProjectionMatrix() const;

	/// <summary>
	/// Disposes internal instances and frees memory.
	/// </summary>
	GLvoid Cleanup();

private:
	std::shared_ptr<PBRViewerShader> myShadowShader;

	std::shared_ptr<PBRViewerScene> myModel;
	std::vector<PBRViewerTexture> myTextures;
	std::vector<GLuint> myFramebufferObjects;

	GLuint myTextureWidth = 0u;
	GLuint myTextureHeight = 0u;

	GLfloat myNearPlane = 0.0f;
	GLfloat myFarPlane = 0.0f;

	std::vector<PBRViewerTexture> CreateDepthTextures( GLuint amountLightSources,
	                                                   GLuint textureWidth,
	                                                   GLuint textureHeight ) const;

	std::vector<GLuint> CreateShadowFrameBuffers( std::vector<PBRViewerTexture> shadowDepthCubeMaps ) const;
};
