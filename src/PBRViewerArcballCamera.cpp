#include "PBRViewerArcballCamera.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/euler_angles.inl>
#include "PBRViewerInputConstants.h"

/// <summary>
/// Initializes a new instance of the <see cref="PBRViewerArcballCamera"/> class.
/// </summary>
/// <param name="position">The position of the camera.</param>
/// <param name="upVector">The up vector.</param>
/// <param name="yaw">The yaw.</param>
/// <param name="pitch">The pitch.</param>
/// <param name="fixedTargetPosition">The position to look at.</param>
/// <param name="frontVector">The front vector.</param>
PBRViewerArcballCamera::PBRViewerArcballCamera( glm::vec3 position,
                                                glm::vec3 upVector,
                                                GLfloat yaw,
                                                GLfloat pitch,
                                                glm::vec3 fixedTargetPosition,
                                                glm::vec3 frontVector )
{
	myCameraPosition = position;
	myWorldUpVector = upVector;
	myFrontVector = frontVector;

	myYaw = yaw;
	myPitch = pitch;

	myFixedTargetPosition = fixedTargetPosition;

	UpdateCameraVectors();
}

/// <summary>
/// Gets the current view matrix.
/// </summary>
/// <returns>The current 4x4 view matrix.</returns>
glm::mat4 PBRViewerArcballCamera::GetViewMatrix() const
{
	return lookAt(myCameraPosition, myCameraPosition + myFrontVector, myUpVector);
}

/// <summary>
/// Gets the camera position.
/// </summary>
/// <returns>The current camera position in world coordinates.</returns>
glm::vec3 PBRViewerArcballCamera::GetCameraPosition() const
{
	return myCameraPosition;
}

/// <summary>
/// Gets the zoom.
/// </summary>
/// <returns></returns>
GLfloat PBRViewerArcballCamera::GetZoom() const
{
	return 45.0f;
}

/// <summary>
/// Processes the left mouse button.
/// </summary>
/// <param name="xoffset">The xoffset.</param>
/// <param name="yoffset">The yoffset.</param>
/// <param name="constrainPitch">A flag indicating if the pitch should be constrained (prevents camera flip at 90 degrees).</param>
GLvoid PBRViewerArcballCamera::ProcessLeftMouseButton( const GLdouble xoffset, const GLdouble yoffset, const GLboolean constrainPitch )
{
	if (xoffset == 0.0 && yoffset == 0.0)
	{
		return;
	}

	myYaw += xoffset * PBRViewerInputConstants::HalfMouseSensitivity;
	myPitch -= yoffset * PBRViewerInputConstants::HalfMouseSensitivity;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (myPitch > 89.9999999f)
		{
			myPitch = 89.9999999f;
		}
		else if (myPitch < -89.9999999f)
		{
			myPitch = -89.9999999f;
		}
	}

	// Form a view vector using total pitch & yaw as spherical coordinates.
	// See: https://gamedev.stackexchange.com/questions/136174/im-rotating-an-object-on-two-axes-so-why-does-it-keep-twisting-around-the-thir
	const GLfloat radius = length(myCameraPosition);
	myCameraPosition.x = static_cast<GLfloat>(cos(glm::radians(myPitch)) * cos(glm::radians(myYaw)));
	myCameraPosition.y = static_cast<GLfloat>(sin(glm::radians(myPitch)));
	myCameraPosition.z = static_cast<GLfloat>(cos(glm::radians(myPitch)) * sin(glm::radians(myYaw)));
	myCameraPosition *= radius;

	// Update Front, Right and Up Vectors using the updated Euler angles
	UpdateCameraVectors();
}

/// <summary>
/// Processes the right mouse button.
/// </summary>
/// <param name="xoffset">The xoffset.</param>
/// <param name="yoffset">The yoffset.</param>
GLvoid PBRViewerArcballCamera::ProcessRightMouseButton( const GLdouble xoffset, const GLdouble yoffset )
{
	if (xoffset == 0.0 && yoffset == 0.0)
	{
		return;
	}

	myCameraPosition -= myUpVector * glm::vec3(static_cast<GLfloat>(yoffset * glm::pow(PBRViewerInputConstants::MouseSensitivity, 4)));
	myCameraPosition -= myRightVector * glm::vec3(static_cast<GLfloat>(xoffset * glm::pow(PBRViewerInputConstants::MouseSensitivity, 4)));
}

/// <summary>
/// Processes the mouse scroll.
/// </summary>
/// <param name="yoffset">The yoffset.</param>
GLvoid PBRViewerArcballCamera::ProcessMouseScroll( const GLdouble yoffset )
{
	if (yoffset == 0.0)
	{
		return;
	}

	myCameraPosition += myFrontVector * glm::vec3(static_cast<GLfloat>(yoffset * PBRViewerInputConstants::MouseSensitivity));
}

/// <summary>
/// Gets the front vector.
/// </summary>
/// <returns>The front vector.</returns>
glm::vec3 PBRViewerArcballCamera::GetFrontVector() const
{
	return myFrontVector;
}

/// <summary>
/// Gets the right vector.
/// </summary>
/// <returns>The right vector.</returns>
glm::vec3 PBRViewerArcballCamera::GetRightVector() const
{
	return myRightVector;
}

/// <summary>
/// Gets the up vector.
/// </summary>
/// <returns>The up vector.</returns>
glm::vec3 PBRViewerArcballCamera::GetUpVector() const
{
	return myUpVector;
}

GLvoid PBRViewerArcballCamera::UpdateCameraVectors()
{
	myFrontVector = normalize(myFixedTargetPosition - myCameraPosition);
	myRightVector = normalize(cross(myFrontVector, myWorldUpVector));

	// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	myUpVector = normalize(cross(myRightVector, myFrontVector));
}
