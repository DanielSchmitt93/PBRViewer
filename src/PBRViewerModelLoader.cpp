#include "PBRViewerModelLoader.h"

#include "PBRViewerOverlayConstants.h"

#include <nanogui/label.h>
#include <nanogui/button.h>
#include <nanogui/layout.h>
#include <nanogui/entypo.h>
#include <nanogui/messagedialog.h>

// ReSharper disable once CppUnusedIncludeDirective
// This include is needed to recognize the screen() method.
#include <nanogui/screen.h>

#include <iostream>

/// <summary>
/// Initializes a new instance of the <see cref="PBRViewerModelLoader"/> class.
/// </summary>
/// <param name="parent">The parent widget.</param>
PBRViewerModelLoader::PBRViewerModelLoader( Widget* parent ) : Window(parent, "Model loader")
{
	setLayout(new nanogui::GroupLayout(15, 6, PBRViewerOverlayConstants::GroupLayoutSpacingBetweenLabels, 20));

	// FPS and help button
	new nanogui::Label(this, "FPS counter ", "sans-bold");

	auto* containerWidget = new Widget(this);
	containerWidget->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal, nanogui::Alignment::Maximum, 0, 106));

	myFpsCounter = new nanogui::TextBox(containerWidget, "0");
	myFpsCounter->setFixedSize(Eigen::Vector2i(60, PBRViewerOverlayConstants::ButtonHeight));
	myFpsCounter->setUnits("fps");
	myFpsCounter->setAlignment(nanogui::TextBox::Alignment::Left);
	myFpsCounter->setFontSize(18);

	myHelpButton = new nanogui::Button(containerWidget, "");
	myHelpButton->setFontSize(PBRViewerOverlayConstants::ButtonFontSize);
	myHelpButton->setFixedHeight(PBRViewerOverlayConstants::ButtonHeight);
	myHelpButton->setIcon(ENTYPO_ICON_HELP_WITH_CIRCLE);
	myHelpButton->setFixedHeight(myFpsCounter->fixedHeight());
	myHelpButton->setCallback([this]()
	{
		std::stringstream helpText;	
		helpText << "Controls: " << std::endl;
		helpText << std::endl;
		helpText << "* Press and hold left mouse button to rotate the camera" << std::endl;		
		helpText << "* Press and hold middle mouse button to rotate the loaded 3D model" << std::endl;	
		helpText << "* Press and hold right mouse button to move the view" << std::endl;
		helpText << std::endl;
		helpText << "* Numpad plus: Enlarge Model" << std::endl;
		helpText << "* Numpad minus: Shrink Model" << std::endl;
		helpText << std::endl;
		helpText << "* R-Key: Reverse scaling and rotation operations" << std::endl;
		helpText << std::endl;
		helpText << "* Space button: Toggle window visibility" << std::endl;

		auto helpWindow = new nanogui::MessageDialog(screen(), nanogui::MessageDialog::Type::Information,
		                                             "PBRViewer Help", helpText.str(), "Got it !");
		helpWindow->setModal(GL_TRUE);	
	});

	// Load model
	new nanogui::Label(this, "Currently loaded model: ", "sans-bold");
	myTextBoxLoadModel = new nanogui::TextBox(this);
	myTextBoxLoadModel->setFixedWidth(200);
	myTextBoxLoadModel->setFixedHeight(20);
	myTextBoxLoadModel->setFontSize(16);
	myTextBoxLoadModel->setValue("No model");

	auto* horizontalLayoutModel = new Widget(this);
	horizontalLayoutModel->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal, nanogui::Alignment::Maximum, 0, 6));

	myLoadModelButton = new nanogui::Button(horizontalLayoutModel, "Load model");
	myLoadModelButton->setFixedSize(Eigen::Vector2i(160, PBRViewerOverlayConstants::ButtonHeight));
	myLoadModelButton->setIcon(ENTYPO_ICON_FOLDER);
	myLoadModelButton->setFontSize(PBRViewerOverlayConstants::ButtonFontSize);

	myClearModelButton = new nanogui::Button(horizontalLayoutModel, "");
	myClearModelButton->setIcon(ENTYPO_ICON_CIRCLE_WITH_CROSS);
	myClearModelButton->setFixedHeight(PBRViewerOverlayConstants::ButtonHeight);
	myClearModelButton->setTooltip("Clear currently loaded model.");
	myClearModelButton->setFontSize(PBRViewerOverlayConstants::ButtonFontSize);

	new nanogui::Label(this, "");

	// Load skybox
	new nanogui::Label(this, "Currently loaded skybox: ", "sans-bold");
	myTextBoxSkybox = new nanogui::TextBox(this);
	myTextBoxSkybox->setFixedWidth(200);
	myTextBoxSkybox->setFixedHeight(20);
	myTextBoxSkybox->setFontSize(16);
	myTextBoxSkybox->setValue("No skybox");

	Widget* horizontalLayoutSkybox = new Widget(this);
	horizontalLayoutSkybox->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal, nanogui::Alignment::Maximum, 0, 6));

	myLoadSkyboxButton = new nanogui::Button(horizontalLayoutSkybox, "Load skybox");
	myLoadSkyboxButton->setFixedSize(Eigen::Vector2i(160, PBRViewerOverlayConstants::ButtonHeight));
	myLoadSkyboxButton->setIcon(ENTYPO_ICON_FOLDER);
	myLoadSkyboxButton->setFontSize(PBRViewerOverlayConstants::ButtonFontSize);

	myClearSkyboxButton = new nanogui::Button(horizontalLayoutSkybox, "");
	myClearSkyboxButton->setIcon(ENTYPO_ICON_CIRCLE_WITH_CROSS);
	myClearSkyboxButton->setFixedHeight(PBRViewerOverlayConstants::ButtonHeight);
	myClearSkyboxButton->setTooltip("Clear currently loaded skybox.");
	myClearSkyboxButton->setFontSize(PBRViewerOverlayConstants::ButtonFontSize);
}

/// <summary>
/// Sets the callback for the button loading a model.
/// </summary>
/// <param name="callback">The callback to set.</param>	
GLvoid PBRViewerModelLoader::SetOpenButtonCallback( const std::function<GLvoid()>& callback ) const
{
	myLoadModelButton->setCallback(callback);
}

/// <summary>
/// Sets the name of the currently loaded model.
/// </summary>
/// <param name="content">The name of the currently loaded model.</param>	
GLvoid PBRViewerModelLoader::SetTextBoxOpenModelContent( const std::string& content ) const
{
	myTextBoxLoadModel->setValue(content);
}

/// <summary>
/// Sets the callback for the button clearing a loaded model.
/// </summary>
/// <param name="callback">The callback to set.</param>	
GLvoid PBRViewerModelLoader::SetClearModelButtonCallback( const std::function<GLvoid()>& callback ) const
{
	myClearModelButton->setCallback(callback);
}

/// <summary>
/// Sets the callback for the button loading a skybox texture.
/// </summary>
/// <param name="callback">The callback to set.</param>	
GLvoid PBRViewerModelLoader::SetLoadSkyboxButtonCallback( const std::function<GLvoid()>& callback ) const
{
	myLoadSkyboxButton->setCallback(callback);
}

/// <summary>
/// Sets the name of the currently loaded skybox texture.
/// </summary>
/// <param name="content">The name of the currently loaded skybox texture.</param>	
GLvoid PBRViewerModelLoader::SetTextBoxSkyboxContent( const std::string& content ) const
{
	myTextBoxSkybox->setValue(content);
}

/// <summary>
/// Sets the callback for the button clearing a loaded skybox texture.
/// </summary>
/// <param name="callback">The callback to set.</param>	
GLvoid PBRViewerModelLoader::SetClearSkyboxButtonCallback( const std::function<GLvoid()>& callback ) const
{
	myClearSkyboxButton->setCallback(callback);
}

/// <summary>
/// Sets the content of the FPS counter.
/// </summary>
/// <param name="content">The current amount of frames per second.</param>	
GLvoid PBRViewerModelLoader::SetFpsCounterContent( const std::string& content ) const
{
	myFpsCounter->setValue(content);
}
