#include "PBRViewerKeyboardCallbacks.h"

// Needs to be static so we can set the GLFW callback accordingly.
static std::shared_ptr<PBRViewerOverlay> myOverlay;
static std::shared_ptr<PBRViewerModel> myModel;

/// <summary>
/// Register the callbacks.
/// </summary>
/// <param name="currentWindow">The current window.</param>
/// <param name="overlay">The overlay.</param>
/// <param name="model">The model.</param>	
GLvoid PBRViewerKeyboardCallbacks::RegisterCallbacks( GLFWwindow* currentWindow,
                                                      std::shared_ptr<PBRViewerOverlay> const& overlay,
                                                      std::shared_ptr<PBRViewerModel> const& model )
{
	myOverlay = overlay;
	myModel = model;

	glfwSetKeyCallback(currentWindow, []( GLFWwindow*, const GLint key, const GLint scancode, const GLint action, const GLint mods )
	{
		myOverlay->keyCallbackEvent(key, scancode, action, mods);
		
		static const GLfloat scaleFactorPerKeyPress = 0.1f;		

		// Scale model
		if (key == GLFW_KEY_KP_ADD && (action == GLFW_REPEAT || action == GLFW_PRESS))
		{
			myModel->ScaleIsotropically(scaleFactorPerKeyPress, PBRViewerEnumerations::ScaleOperation::Increase);
		}
		else if (key == GLFW_KEY_KP_SUBTRACT && (action == GLFW_REPEAT || action == GLFW_PRESS))
		{
			myModel->ScaleIsotropically(scaleFactorPerKeyPress, PBRViewerEnumerations::ScaleOperation::Decrease);
		}

			// Reset model transformations
		else if (key == GLFW_KEY_R && action == GLFW_PRESS)
		{
			myModel->ResetModelTransformations();
		}

			// Toggle visibility of UI elements
		else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		{
			myOverlay->GraphicSettings->setVisible(!myOverlay->GraphicSettings->visible());
			myOverlay->ModelLoader->setVisible(!myOverlay->ModelLoader->visible());
			myOverlay->IBLSettings->setVisible(!myOverlay->IBLSettings->visible());
		}
	});

	glfwSetCharCallback(currentWindow, []( GLFWwindow*, const GLuint codepoint )
	{
		myOverlay->charCallbackEvent(codepoint);
	});
}
