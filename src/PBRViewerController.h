#pragma once

#include "PBRViewerModel.h"
#include "PBRViewerOverlay.h"

/// <summary>
/// This class is the controller in the MVC pattern. 
/// It handles the connection between the model (where the OpenGL calls are made) and the View (the UI elements for the user).
/// It is the first class instantiated by the main function.
/// </summary>
class PBRViewerController
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="PBRViewerController"/> class.
	/// </summary>
	PBRViewerController() = default;

	/// <summary>
	/// Initializes the architectural view of the MVC pattern (the UI elements).
	/// </summary>
	GLvoid InitOverlay();
	
	/// <summary>
	/// Initializes the architectural model of the MVC pattern.
	/// </summary>
	GLvoid InitModel() const;

	/// <summary>
	/// Starts the render (game) loop.
	/// </summary>
	GLvoid StartRenderLoop();

	/// <summary>
	/// Disposes internal instances and frees memory.
	/// </summary>
	GLvoid Cleanup();
	
private:
	std::shared_ptr<PBRViewerOverlay> myOverlayRoot = std::make_shared<PBRViewerOverlay>();
	std::shared_ptr<PBRViewerModel> myModel = std::make_shared<PBRViewerModel>();

	GLuint myNumberOfFrames = 0u;
	GLdouble myLastTime = 0.0;

	/// <summary>
	/// Sets the callbacks for all overlay components, i. e. the visible windows.
	/// </summary>
	GLvoid ConfigureOverlayComponents();

	/// <summary>
	/// Sets the callbacks for the model loader window.
	/// </summary>
	GLvoid ConfigureModelLoader();

	/// <summary>
	/// Sets the callbacks for the graphic settings window.
	/// </summary>
	GLvoid ConfigureGraphicSettings() const;

	/// <summary>
	/// Sets the callbacks for the Image Based Lighting window.
	/// </summary>
	GLvoid ConfigureIBLSettings() const;

	/// <summary>
	/// Sets the callbacks for the GLFW window (the window to draw into).
	/// </summary>
	GLvoid SetGlfwCallbacks(GLFWwindow* window) const;

	/// <summary>
	/// Calculates frames per second.
	/// </summary>
	GLvoid CalculateFps();	
};
