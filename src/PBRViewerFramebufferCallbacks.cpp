#include "PBRViewerFramebufferCallbacks.h"

// Needs to be static so we can set the GLFW callback accordingly.
static std::shared_ptr<PBRViewerOverlay> myOverlay;
static std::shared_ptr<PBRViewerModel> myModel;

/// <summary>
/// Register the callbacks.
/// </summary>
/// <param name="currentWindow">The current window.</param>
/// <param name="overlay">The overlay.</param>
/// <param name="model">The model.</param>	
GLvoid PBRViewerFramebufferCallbacks::RegisterCallbacks( GLFWwindow* currentWindow,
                                                         std::shared_ptr<PBRViewerOverlay> const& overlay,
                                                         std::shared_ptr<PBRViewerModel> const& model )
{
	myOverlay = overlay;
	myModel = model;

	glfwSetFramebufferSizeCallback(currentWindow, []( GLFWwindow*, const GLint width, const GLint height )
	{
		glViewport(0, 0, width, height);

		myOverlay->resizeCallbackEvent(width, height);

		myOverlay->MoveComponentTopLeft(myOverlay->GraphicSettings);
		myOverlay->MoveComponentTopRight(myOverlay->ModelLoader);
		myOverlay->MoveComponentBottomRight(myOverlay->IBLSettings);	
	});
}
