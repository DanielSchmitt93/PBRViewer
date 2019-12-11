#include "PBRViewerMouseCallbacks.h"

#include "glm/gtx/euler_angles.hpp"
#include "PBRViewerInputConstants.h"

// Needs to be static so we can set the GLFW callback accordingly.
static std::shared_ptr<PBRViewerOverlay> myOverlay;
static std::shared_ptr<PBRViewerModel> myModel;

/// <summary>
/// Register the callbacks.
/// </summary>
/// <param name="currentWindow">The current window.</param>
/// <param name="overlay">The overlay.</param>
/// <param name="model">The model.</param>	
GLvoid PBRViewerMouseCallbacks::RegisterCallbacks( GLFWwindow* currentWindow,
                                                   std::shared_ptr<PBRViewerOverlay> const& overlay,
                                                   std::shared_ptr<PBRViewerModel> const& model )
{
	myOverlay = overlay;
	myModel = model;

	glfwSetCursorPosCallback(currentWindow, []( GLFWwindow* window, const GLdouble cursorPosX, const GLdouble cursorPosY )
	{
		if(glfwGetInputMode(window,GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
		{
			// Do not interact with the UI if the cursor is disabled.
			// UI elements could get focus - this should be prevented.
			myOverlay->cursorPosCallbackEvent(cursorPosX, cursorPosY);
		}		

		static GLdouble myLastCursorPosX = 0.0;
		static GLdouble myLastCursorPosY = 0.0;

		// Camera movement
		if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
		{
			if (myModel->GetMouseProcessing())
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

				const GLdouble xoffset = cursorPosX - myLastCursorPosX;
				const GLdouble yoffset = myLastCursorPosY - cursorPosY; // reversed since y-coordinates go from bottom to top

				myModel->GetCamera().lock()->ProcessLeftMouseButton(xoffset, yoffset);
			}
		}
		else if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
		{
			if (myModel->GetMouseProcessing())
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

				const GLdouble xoffset = cursorPosX - myLastCursorPosX;
				const GLdouble yoffset = myLastCursorPosY - cursorPosY; // reversed since y-coordinates go from bottom to top

				myModel->GetCamera().lock()->ProcessRightMouseButton(xoffset, yoffset);
			}
		}
		// Model rotation
		else if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_3))
		{
			if (myModel->GetMouseProcessing())
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

				const GLdouble diffX = (cursorPosX - myLastCursorPosX) * PBRViewerInputConstants::MouseSensitivity;
				const GLdouble diffY = (myLastCursorPosY - cursorPosY) * PBRViewerInputConstants::MouseSensitivity;

				if (diffX == 0.0 && diffY == 0.0)
				{
					return;
				}

				// Use the global y-axis for the x-offset and the local y-axis to calculate the rotation axis for the y-offset.
				// See: https://gamedev.stackexchange.com/questions/136174/im-rotating-an-object-on-two-axes-so-why-does-it-keep-twisting-around-the-thir	
				glm::mat4 rotationMatrix = glm::identity<glm::mat4>();

				rotationMatrix = rotate(rotationMatrix, glm::radians(static_cast<GLfloat>(diffX)), glm::vec3(0.0f, 1.0f, 0.0f));

				const glm::vec3 viewVector = myModel->GetCamera().lock()->GetFrontVector();
				const glm::vec3 upVector = myModel->GetCamera().lock()->GetUpVector();
				const glm::vec3 rotationAxis = normalize(cross(upVector, viewVector));

				// Care if cross product is zero
				if (rotationAxis == glm::vec3(0.0f))
				{
					rotationMatrix = rotate(rotationMatrix, glm::radians(static_cast<GLfloat>(diffY)), upVector);
				}
				else
				{
					rotationMatrix = rotate(rotationMatrix, glm::radians(static_cast<GLfloat>(diffY)), rotationAxis);
				}

				myModel->RotateModel(rotationMatrix);
			}
		}

		myLastCursorPosX = cursorPosX;
		myLastCursorPosY = cursorPosY;
	});

	glfwSetMouseButtonCallback(currentWindow, []( GLFWwindow* window, const GLint button, const GLint action, const GLint modifiers )
	{
		if (button == GLFW_MOUSE_BUTTON_1 || button == GLFW_MOUSE_BUTTON_2)
		{
			// Check if the user clicked on any of the UI elements
			if (myOverlay->mouseButtonCallbackEvent(button, action, modifiers))
			{
				// If the user did so, disable the camera movement
				if (action == GLFW_PRESS)
				{
					myModel->SetMouseProcessing(GL_FALSE);
					return;
				}
			}

			if (action == GLFW_RELEASE)
			{
				// Restore cursor from camera movement
				if (GLFW_CURSOR_DISABLED == glfwGetInputMode(window, GLFW_CURSOR))
				{
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}

				myModel->SetMouseProcessing(GL_TRUE);
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_3)
		{
			// Restore cursor from model rotation
			if (action == GLFW_RELEASE && GLFW_CURSOR_DISABLED == glfwGetInputMode(window, GLFW_CURSOR))
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
		}
	});
}
