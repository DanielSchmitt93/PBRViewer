#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

// Default camera values
const glm::vec3 DefaultPosition = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 DefaultTargetPosition = glm::vec3(0.0f, 0.0f, 0.0f);

const glm::vec3 DefaultUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 DefaultFrontVector = glm::vec3(0.0f, 0.0f, -1.0f);

const GLfloat DefaultYaw = 90.0f;
const GLfloat DefaultPitch = 0.0f;

/// <summary>
/// This class represents the main camera in PBRViewer. 
/// It acts like an arcball camera, where the camera is constantly rotating around the object of interest.
/// This makes it easier to examine an object and study the light properties.
/// </summary>
class PBRViewerArcballCamera
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="PBRViewerArcballCamera"/> class.
	/// </summary>
	/// <param name="position">The position of the camera.</param>
	/// <param name="upVector">The up vector.</param>
	/// <param name="yaw">The yaw.</param>
	/// <param name="pitch">The pitch.</param>
	/// <param name="fixedTargetPosition">The position to look at.</param>
	/// <param name="frontVector">The front vector.</param>
	PBRViewerArcballCamera( glm::vec3 position = DefaultPosition,
	                        glm::vec3 upVector = DefaultUpVector,
	                        GLfloat yaw = DefaultYaw,
	                        GLfloat pitch = DefaultPitch,
	                        glm::vec3 fixedTargetPosition = DefaultTargetPosition,
	                        glm::vec3 frontVector = DefaultFrontVector );

	/// <summary>
	/// Gets the current view matrix.
	/// </summary>
	/// <returns>The current 4x4 view matrix.</returns>
	glm::mat4 GetViewMatrix() const;

	/// <summary>
	/// Gets the camera position.
	/// </summary>
	/// <returns>The current camera position in world coordinates.</returns>
	glm::vec3 GetCameraPosition() const;

	/// <summary>
	/// Gets the zoom.
	/// </summary>
	/// <returns></returns>
	GLfloat GetZoom() const;

	/// <summary>
	/// Processes the left mouse button.
	/// </summary>
	/// <param name="xoffset">The xoffset.</param>
	/// <param name="yoffset">The yoffset.</param>
	/// <param name="constrainPitch">A flag indicating if the pitch should be constrained (prevents camera flip at 90 degrees).</param>
	GLvoid ProcessLeftMouseButton( GLdouble xoffset, GLdouble yoffset, GLboolean constrainPitch = GL_TRUE );

	/// <summary>
	/// Processes the right mouse button.
	/// </summary>
	/// <param name="xoffset">The xoffset.</param>
	/// <param name="yoffset">The yoffset.</param>
	GLvoid ProcessRightMouseButton( GLdouble xoffset, GLdouble yoffset );

	/// <summary>
	/// Processes the mouse scroll.
	/// </summary>
	/// <param name="yoffset">The yoffset.</param>
	GLvoid ProcessMouseScroll( GLdouble yoffset );

	/// <summary>
	/// Gets the front vector.
	/// </summary>
	/// <returns>The front vector.</returns>
	glm::vec3 GetFrontVector() const;

	/// <summary>
	/// Gets the right vector.
	/// </summary>
	/// <returns>The right vector.</returns>
	glm::vec3 GetRightVector() const;

	/// <summary>
	/// Gets the up vector.
	/// </summary>
	/// <returns>The up vector.</returns>
	glm::vec3 GetUpVector() const;

private:
	GLvoid UpdateCameraVectors();

	glm::vec3 myFixedTargetPosition;

	// Camera Attributes
	glm::vec3 myCameraPosition;
	
	glm::vec3 myFrontVector;
	glm::vec3 myUpVector;
	glm::vec3 myRightVector;
	glm::vec3 myWorldUpVector;

	// Euler Angles
	GLdouble myYaw;
	GLdouble myPitch;
};
