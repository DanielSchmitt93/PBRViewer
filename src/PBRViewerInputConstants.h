#pragma once

#include <glad/glad.h>

/// <summary>
/// This namespace contains constants used for input devices.
/// </summary>
namespace PBRViewerInputConstants
{
	/// <summary>
	/// The global mouse sensitivity used within the application.
	/// </summary>
	const GLfloat MouseSensitivity = 0.2f;

	/// <summary>
	/// Half of the global mouse sensitivity used within the application.
	/// </summary>
	const GLfloat HalfMouseSensitivity = MouseSensitivity * 0.5f;
}
