#include "PBRViewerController.h"
#include "PBRViewerLogger.h"

/// <summary>
/// Main function of PBRViewer.
/// </summary>
GLint main(GLint, GLchar**)
{
	PBRViewerLogger::PrintWelcomeMessage();

	PBRViewerController controller;

	controller.InitModel();
	controller.InitOverlay();
	controller.StartRenderLoop();
	controller.Cleanup();	
}