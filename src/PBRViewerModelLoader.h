#pragma once

#include <glad/glad.h>

#include <nanogui/window.h>
#include <nanogui/textbox.h>

/// <summary>
/// This class represents the window used to load a 3D model or a skybox texture.
/// </summary>
class PBRViewerModelLoader : public nanogui::Window
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="PBRViewerModelLoader"/> class.
	/// </summary>
	/// <param name="parent">The parent widget.</param>
	PBRViewerModelLoader( Widget* parent );

	/// <summary>
	/// Sets the content of the FPS counter.
	/// </summary>
	/// <param name="content">The current amount of frames per second.</param>	
	GLvoid SetFpsCounterContent( const std::string& content ) const;

	/// <summary>
	/// Sets the callback for the button loading a model.
	/// </summary>
	/// <param name="callback">The callback to set.</param>	
	GLvoid SetOpenButtonCallback( const std::function<GLvoid()>& callback ) const;

	/// <summary>
	/// Sets the name of the currently loaded model.
	/// </summary>
	/// <param name="content">The name of the currently loaded model.</param>	
	GLvoid SetTextBoxOpenModelContent( const std::string& content ) const;

	/// <summary>
	/// Sets the callback for the button clearing a loaded model.
	/// </summary>
	/// <param name="callback">The callback to set.</param>	
	GLvoid SetClearModelButtonCallback( const std::function<GLvoid()>& callback ) const;

	/// <summary>
	/// Sets the callback for the button loading a skybox texture.
	/// </summary>
	/// <param name="callback">The callback to set.</param>	
	GLvoid SetLoadSkyboxButtonCallback( const std::function<GLvoid()>& callback ) const;

	/// <summary>
	/// Sets the name of the currently loaded skybox texture.
	/// </summary>
	/// <param name="content">The name of the currently loaded skybox texture.</param>	
	GLvoid SetTextBoxSkyboxContent( const std::string& content ) const;

	/// <summary>
	/// Sets the callback for the button clearing a loaded skybox texture.
	/// </summary>
	/// <param name="callback">The callback to set.</param>	
	GLvoid SetClearSkyboxButtonCallback( const std::function<GLvoid()>& callback ) const;

private:
	nanogui::TextBox* myFpsCounter;
	nanogui::Button* myHelpButton;

	nanogui::Button* myLoadModelButton;
	nanogui::TextBox* myTextBoxLoadModel;
	nanogui::Button* myClearModelButton;

	nanogui::Button* myLoadSkyboxButton;
	nanogui::TextBox* myTextBoxSkybox;
	nanogui::Button* myClearSkyboxButton;
};
