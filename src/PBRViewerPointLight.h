#pragma once

#include <string>
#include "PBRViewerShader.h"
#include "PBRViewerScene.h"

/// <summary>
/// This class represents a point light source surrounding the loaded 3D model.
/// The light source is not just a point in space but instead uses a 3D model by itself for representation.
/// </summary>
class PBRViewerPointLight
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="PBRViewerPointLight"/> class.
	/// Uses the default 3D model as representation.
	/// The light is inactive by default.
	/// </summary>
	PBRViewerPointLight();

	/// <summary>
	/// Initializes a new instance of the <see cref="PBRViewerPointLight"/> class.
	/// The light is inactive by default.
	/// </summary>
	/// <param name="pathToModel">The filepath to the 3D model representing the light source.</param>
	explicit PBRViewerPointLight(const std::string& pathToModel);

	/// <summary>
	/// Gets a flag indicating if the light is currently active.
	/// </summary>
	/// <returns>True if the light is switched on, false if not.</returns>
	GLboolean GetIsActive() const;

	/// <summary>
	/// Sets the light as active or inactive.
	/// </summary>
	/// <param name="newValue">True if the light is switched on, false if not.</param>	
	GLvoid SetIsActive(GLboolean newValue);

	/// <summary>
	/// Gets the position of the light source.
	/// </summary>
	/// <returns>A three dimensional vector representing the position of the light source in world coordinates.</returns>
	glm::vec3 GetPosition() const;

	/// <summary>
	/// Sets the position of the light source.
	/// </summary>
	/// <param name="newPosition">A three dimensional vector representing the position of the light source in world coordinates.</param>	
	GLvoid SetPosition(glm::vec3 newPosition);

	/// <summary>
	/// Gets the color of the light.
	/// </summary>
	/// <returns>A three dimensional vector representing the color of the light as RGB values. The values are NOT clamped between [0, 1].</returns>
	glm::vec3 GetLightColor() const;

	/// <summary>
	/// Sets the color of the light.
	/// </summary>
	/// <param name="newColor">
	/// A three dimensional vector representing the color of the light as RGB values. 
	/// The values do not have to be clamped between [0, 1].
	/// Higher values equal higher intensity.
	/// </param>	
	GLvoid SetLightColor(glm::vec3 newColor);	

	/// <summary>
	/// Draws the light source.
	/// </summary>
	/// <param name="modelMatrix">The model matrix used to draw the light.</param>
	/// <param name="viewMatrix">The view matrix used to draw the light.</param>
	/// <param name="projectionMatrix">The projection matrix used to draw the light.</param>	
	GLvoid Draw(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) const;

private:
	glm::vec3 myPosition = glm::vec3(0.0f);
	glm::vec3 myLightColor = glm::vec3(1.0f);
	GLboolean myIsActive = GL_FALSE;	
};
