#pragma once
#include <glad/glad.h>
#include <iostream>

/// <summary>
/// This class provides functions to log and display messages.
/// All calls to the standard output device are made by this class.
/// </summary>
class PBRViewerLogger
{
public:
	/// <summary>
	/// Callback function for OpenGL in case of errors in debug mode.
	/// This function is passed to 'glDebugMessageCallback' once and will not be used otherwise.
	/// </summary>
	/// <param name="source">The source.</param>
	/// <param name="type">The type.</param>
	/// <param name="id">The identifier.</param>
	/// <param name="severity">The severity.</param>	
	/// <param name="message">The message.</param>	
	static GLvoid APIENTRY PrintOpenGLDebugOutput( const GLenum source,
	                                               const GLenum type,
	                                               const GLuint id,
	                                               const GLenum severity,
	                                               GLsizei,
	                                               const GLchar* message,
	                                               const GLvoid* )
	{
		// Ignore non-significant error/warning codes
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
		{
			return;
		}

		std::cout << "---------------" << std::endl;
		std::cout << "Debug message (" << id << "): " << message << std::endl;

		switch (source)
		{
			case GL_DEBUG_SOURCE_API: std::cout << "Source: API";
				break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: std::cout << "Source: Window System";
				break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler";
				break;
			case GL_DEBUG_SOURCE_THIRD_PARTY: std::cout << "Source: Third Party";
				break;
			case GL_DEBUG_SOURCE_APPLICATION: std::cout << "Source: Application";
				break;
			case GL_DEBUG_SOURCE_OTHER: std::cout << "Source: Other";
				break;
			default: ;
		}
		std::cout << std::endl;

		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR: std::cout << "Type: Error";
				break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour";
				break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cout << "Type: Undefined Behaviour";
				break;
			case GL_DEBUG_TYPE_PORTABILITY: std::cout << "Type: Portability";
				break;
			case GL_DEBUG_TYPE_PERFORMANCE: std::cout << "Type: Performance";
				break;
			case GL_DEBUG_TYPE_MARKER: std::cout << "Type: Marker";
				break;
			case GL_DEBUG_TYPE_PUSH_GROUP: std::cout << "Type: Push Group";
				break;
			case GL_DEBUG_TYPE_POP_GROUP: std::cout << "Type: Pop Group";
				break;
			case GL_DEBUG_TYPE_OTHER: std::cout << "Type: Other";
				break;
			default: ;
		}
		std::cout << std::endl;

		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH: std::cout << "Severity: high";
				break;
			case GL_DEBUG_SEVERITY_MEDIUM: std::cout << "Severity: medium";
				break;
			case GL_DEBUG_SEVERITY_LOW: std::cout << "Severity: low";
				break;
			case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification";
				break;
			default: ;
		}

		std::cout << std::endl;
		std::cout << std::endl;
	}

	/// <summary>
	/// Prints a error message to the standard output device.
	/// </summary>
	/// <param name="file">The file where the error occurred.</param>
	/// <param name="line">The line were the error occurred.</param>
	/// <param name="message">The message of the error.</param>
	/// <param name="additionalInfo">(Optional) additional information.</param>	
	static GLvoid PrintErrorMessage( const std::string& file,
	                                 const GLint line,
	                                 const std::string& message,
	                                 const std::string& additionalInfo = "" )
	{
		std::cout << "---------------" << std::endl;
		std::cout << "Error in file: " << file << " in line: " << line << std::endl;
		std::cout << "Message: " << message << " " << additionalInfo << std::endl;
		std::cout << "---------------" << std::endl;
	}

	/// <summary>
	/// Prints a welcome message to the standard output device.
	/// </summary>
	static GLvoid PrintWelcomeMessage()
	{
		std::cout << "------------------------------------------------" << std::endl;
		std::cout << "--- PBRViewer by Daniel Schmitt. Version 1.0 ---" << std::endl;
		std::cout << "------------------------------------------------" << std::endl;
		std::cout << std::endl;
		std::cout <<
			"In this window all possible error messages of PBRViewer are logged. The following is a list of all error messages that have occurred so far: "
			<< std::endl;
		std::cout << std::endl;
	}
};
