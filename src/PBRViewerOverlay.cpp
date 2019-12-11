#include "PBRViewerOverlay.h"
#include <GLFW/glfw3.h>

GLvoid PBRViewerOverlay::CreateComponents()
{
	ModelLoader = new PBRViewerModelLoader(this);
	GraphicSettings = new PBRViewerGraphicSettings(this);
	IBLSettings = new PBRViewerIBLSettings(this);
}

GLvoid PBRViewerOverlay::MoveComponentTopLeft( nanogui::Window* component ) const
{
	component->setPosition(Eigen::Vector2i(0, 0));
}

GLvoid PBRViewerOverlay::MoveComponentTopRight( nanogui::Window* component ) const
{
	GLint currentWindowWidth, currentWindowHeight;
	glfwGetWindowSize(glfwGetCurrentContext(), &currentWindowWidth, &currentWindowHeight);

	component->setPosition(Eigen::Vector2i(currentWindowWidth - component->width(), 0));
}

GLvoid PBRViewerOverlay::MoveComponentBottomLeft( nanogui::Window* component ) const
{
	GLint currentWindowWidth, currentWindowHeight;
	glfwGetWindowSize(glfwGetCurrentContext(), &currentWindowWidth, &currentWindowHeight);

	component->setPosition(Eigen::Vector2i(0, currentWindowHeight - component->height()));
}

GLvoid PBRViewerOverlay::MoveComponentBottomRight( nanogui::Window* component ) const
{
	GLint currentWindowWidth, currentWindowHeight;
	glfwGetWindowSize(glfwGetCurrentContext(), &currentWindowWidth, &currentWindowHeight);

	component->setPosition(Eigen::Vector2i(currentWindowWidth - component->width(),
	                                       currentWindowHeight - component->height()));
}
