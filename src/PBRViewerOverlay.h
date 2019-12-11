#pragma once

#include <nanogui/screen.h>
#include "PBRViewerModelLoader.h"
#include "PBRViewerGraphicSettings.h"
#include "PBRViewerIBLSettings.h"

// This class acts as the view in the MVC pattern.
// The UI elements will be handled within this class. No frontend components should be outside of this class.
class PBRViewerOverlay : public nanogui::Screen
{
public:
	PBRViewerOverlay() = default;
	GLvoid CreateComponents();

	PBRViewerModelLoader* ModelLoader = nullptr;
	PBRViewerGraphicSettings* GraphicSettings = nullptr;
	PBRViewerIBLSettings* IBLSettings = nullptr;

	GLvoid MoveComponentTopLeft( nanogui::Window* component ) const;
	GLvoid MoveComponentTopRight( nanogui::Window* component ) const;

	GLvoid MoveComponentBottomLeft( nanogui::Window* component ) const;
	GLvoid MoveComponentBottomRight( nanogui::Window* component ) const;
};
