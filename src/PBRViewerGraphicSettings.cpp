#include "PBRViewerGraphicSettings.h"

#include "PBRViewerOverlayConstants.h"

#include <nanogui/tabwidget.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/colorwheel.h>
#include <nanogui/combobox.h>
#include <nanogui/popupbutton.h>
#include <nanogui/checkbox.h>

/// <summary>
/// Initializes a new instance of the <see cref="PBRViewerGraphicSettings"/> class.
/// </summary>
/// <param name="parent">The parent widget.</param>
PBRViewerGraphicSettings::PBRViewerGraphicSettings( Widget* parent ) : Window(parent, "Graphic settings")
{
	setLayout(new nanogui::GroupLayout(15, 6, PBRViewerOverlayConstants::GroupLayoutSpacingBetweenLabels, 20));
	setFixedWidth(270);

	auto* tabWidget = new nanogui::TabWidget(this);

	Widget* firstTab = tabWidget->createTab("CT");
	firstTab->setLayout(new nanogui::GridLayout(nanogui::Orientation::Vertical, 1, nanogui::Alignment::Fill));	
	CookTorranceTab = new PBRViewerCookTorranceBRDFWidget(firstTab);

	Widget* secondTab = tabWidget->createTab("BP");
	secondTab->setLayout(new nanogui::GridLayout(nanogui::Orientation::Vertical, 1, nanogui::Alignment::Fill));
	BlinnPhongTab = new PBRViewerBlinnPhongWidget(secondTab);

	Widget* thirdTab = tabWidget->createTab("ON");
	thirdTab->setLayout(new nanogui::GridLayout(nanogui::Orientation::Vertical, 1, nanogui::Alignment::Fill));
	OrenNayarTab = new PBRViewerOrenNayarBRDFWidget(thirdTab);

	Widget* fourthTab = tabWidget->createTab("AS");
	fourthTab->setLayout(new nanogui::GridLayout(nanogui::Orientation::Vertical, 1, nanogui::Alignment::Fill));
	AshikhminShirleyTab = new PBRViewerAshikhminShirleyBRDFWidget(fourthTab);

	Widget* fifthTab = tabWidget->createTab("D");
	fifthTab->setLayout(new nanogui::GridLayout(nanogui::Orientation::Vertical, 1, nanogui::Alignment::Fill));
	DisneyTab = new PBRViewerDisneyBRDF(fifthTab);

	Widget* sixthTab = tabWidget->createTab("Debug");
	sixthTab->setLayout(new nanogui::GridLayout(nanogui::Orientation::Vertical, 1, nanogui::Alignment::Fill));
	DebugTab = new PBRViewerDebugWidget(sixthTab);

	Tabs = {CookTorranceTab, BlinnPhongTab, OrenNayarTab, AshikhminShirleyTab, DisneyTab, DebugTab };

	// Default tab is Cook-Torrance
	tabWidget->setActiveTab(tabWidget->tabIndex(CookTorranceTab->parent()));

	// ------------------------------------------------------------
	//                    --- General settings ---
	// ------------------------------------------------------------	
	new nanogui::Label(this, "General settings", "sans-bold");

	new nanogui::Label(this, "Light color");
	myLightingColorWheel = new nanogui::ColorWheel(this, nanogui::Color(Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));
	myLightingColorWheel->setTooltip("The color of the lightsources.");

	new nanogui::Label(this, "Point lights", "sans-bold");
	auto* horizontalLayoutLights = new Widget(this);
	horizontalLayoutLights->setLayout(
		new nanogui::BoxLayout(nanogui::Orientation::Horizontal, nanogui::Alignment::Maximum, 0, 8));

	new nanogui::Label(this, "Shadows", "sans-bold");
	myEnableShadowCheckBox = new nanogui::CheckBox(this, "Enable shadows");
	myEnableShadowCheckBox->setChecked(GL_TRUE);

	myFirstLight = new nanogui::CheckBox(horizontalLayoutLights, "1");
	mySecondLight = new nanogui::CheckBox(horizontalLayoutLights, "2");
	myThirdLight = new nanogui::CheckBox(horizontalLayoutLights, "3");
	myFourthLight = new nanogui::CheckBox(horizontalLayoutLights, "4");

	new nanogui::Label(this, "Render output", "sans-bold");
	myRenderOutputComboBox = new nanogui::ComboBox(this,
	                                               {
		                                               "Albedo", "Ambient Occlusion", "BRDF Lookup (IBL)", "Color",
		                                               "Emissive", "Metallic", "Roughness"
	                                               });
	myRenderOutputComboBox->setSelectedIndex(PBRViewerEnumerations::RenderOutput::Color);
	myRenderOutputComboBox->setFontSize(PBRViewerOverlayConstants::ButtonFontSize);
	myRenderOutputComboBox->setFixedHeight(PBRViewerOverlayConstants::ButtonHeight);
	myRenderOutputComboBox->popup()->setAnchorHeight(157);
	myRenderOutputComboBox->setTooltip("Defines the output of the fragment shader.");

	// Prevent DividyByZero-Exception if gamma = 0.0f
	new nanogui::Label(this, "Gamma", "sans-bold");
	myGammaSlider = new PBRViewerScalarSlider<GLfloat>(this, std::make_pair(0.01f, 8.0f), 2.2f);

	new nanogui::Label(this, "Exposure", "sans-bold");
	myExposureSlider = new PBRViewerScalarSlider<GLfloat>(this, std::make_pair(0.0f, 6.0f), 1.0f);
}

/// <summary>
/// Sets the callback of the color wheel which is responsible for the light color.
/// </summary>
/// <param name="callback">The callback to set.</param>	
GLvoid PBRViewerGraphicSettings::SetColorWheelCallback(
	const std::function<GLvoid( const nanogui::Color& )>& callback ) const
{
	myLightingColorWheel->setCallback(callback);
}

/// <summary>
/// Sets the callback of the combobox representing the render output.
/// </summary>
/// <param name="callback">The callback to set.</param>
GLvoid PBRViewerGraphicSettings::SetRenderOutputComboBoxCallback(
	const std::function<GLvoid( PBRViewerEnumerations::RenderOutput )>& callback ) const
{
	myRenderOutputComboBox->setCallback([callback]( const GLint currentRenderOutput )
	{
		callback(static_cast<PBRViewerEnumerations::RenderOutput>(currentRenderOutput));
	});
}

/// <summary>
/// Sets the callback of the checkbox representing the first light.
/// </summary>
/// <param name="callback">The callback to set.</param>	
GLvoid PBRViewerGraphicSettings::SetFirstLightCheckboxCallback( const std::function<GLvoid( GLboolean )>& callback ) const
{
	myFirstLight->setCallback(callback);
}

/// <summary>
/// Sets the callback of the checkbox representing the second light.
/// </summary>
/// <param name="callback">The callback to set.</param>	
GLvoid PBRViewerGraphicSettings::SetSecondLightCheckboxCallback( const std::function<GLvoid( GLboolean )>& callback ) const
{
	mySecondLight->setCallback(callback);
}

/// <summary>
/// Sets the allback of the checkbox representing the third light.
/// </summary>
/// <param name="callback">The callback to set.</param>	
GLvoid PBRViewerGraphicSettings::SetThirdLightCheckboxCallback( const std::function<GLvoid( GLboolean )>& callback ) const
{
	myThirdLight->setCallback(callback);
}

/// <summary>
/// Sets the allback of the checkbox representing the fourth light.
/// </summary>
/// <param name="callback">The callback to set.</param>	
GLvoid PBRViewerGraphicSettings::SetFourthLightCheckboxCallback( const std::function<GLvoid( GLboolean )>& callback ) const
{
	myFourthLight->setCallback(callback);
}

/// <summary>
/// Sets the callback of the checkbox which enables/disables shadows .
/// </summary>
/// <param name="callback">The callback to set.</param>	
GLvoid PBRViewerGraphicSettings::SetEnableShadowsCheckBoxCallback( const std::function<GLvoid( GLboolean )>& callback ) const
{
	myEnableShadowCheckBox->setCallback(callback);
}

/// <summary>
/// Sets the callback of the slider responsible for the gamma value.
/// </summary>
/// <param name="callback">The callback to set.</param>	
GLvoid PBRViewerGraphicSettings::SetGammaSliderCallback( const std::function<GLvoid( GLfloat )>& callback ) const
{
	myGammaSlider->SetSliderCallback(callback);
}

/// <summary>
/// Sets the callback of the slider responsible for the exposure value.
/// </summary>
/// <param name="callback">The callback to set.</param>	
GLvoid PBRViewerGraphicSettings::SetExposureSliderCallback( const std::function<GLvoid( GLfloat )>& callback ) const
{
	myExposureSlider->SetSliderCallback(callback);
}
