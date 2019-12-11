#pragma once

#include <nanogui/window.h>
#include "PBRViewerEnumerations.h"
#include "PBRViewerScalarSlider.h"

#include "PBRViewerBXDFWidget.h"
#include "PBRViewerBlinnPhongWidget.h"
#include "PBRViewerDebugWidget.h"
#include "PBRViewerCookTorranceBRDFWidget.h"
#include "PBRViewerOrenNayarBRDFWidget.h"
#include "PBRViewerAshikhminShirleyBRDFWidget.h"
#include "PBRViewerDisneyBRDF.h"

/// <summary>
/// This class represents the window used to choose a lighting algorithm and to configure general graphic settings.
/// </summary>
class PBRViewerGraphicSettings : public nanogui::Window
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="PBRViewerGraphicSettings"/> class.
	/// </summary>
	/// <param name="parent">The parent widget.</param>
	explicit PBRViewerGraphicSettings( Widget* parent );

	// ---------------------------------------------
	//              --- Lighting ---
	// ---------------------------------------------

	/// <summary>
	/// Sets the callback of the color wheel which is responsible for the light color.
	/// </summary>
	/// <param name="callback">The callback to set.</param>	
	GLvoid SetColorWheelCallback( const std::function<GLvoid( const nanogui::Color& )>& callback ) const;

	/// <summary>
	/// Sets the callback of the checkbox representing the first light.
	/// </summary>
	/// <param name="callback">The callback to set.</param>	
	GLvoid SetFirstLightCheckboxCallback( const std::function<GLvoid( GLboolean )>& callback ) const;

	/// <summary>
	/// Sets the callback of the checkbox representing the second light.
	/// </summary>
	/// <param name="callback">The callback to set.</param>	
	GLvoid SetSecondLightCheckboxCallback( const std::function<GLvoid( GLboolean )>& callback ) const;

	/// <summary>
	/// Sets the allback of the checkbox representing the third light.
	/// </summary>
	/// <param name="callback">The callback to set.</param>	
	GLvoid SetThirdLightCheckboxCallback( const std::function<GLvoid( GLboolean )>& callback ) const;

	/// <summary>
	/// Sets the allback of the checkbox representing the fourth light.
	/// </summary>
	/// <param name="callback">The callback to set.</param>	
	GLvoid SetFourthLightCheckboxCallback( const std::function<GLvoid( GLboolean )>& callback ) const;

	/// <summary>
	/// Sets the callback of the combobox representing the render output.
	/// </summary>
	/// <param name="callback">The callback to set.</param>
	GLvoid SetRenderOutputComboBoxCallback(
		const std::function<GLvoid( PBRViewerEnumerations::RenderOutput )>& callback ) const;

	/// <summary>
	/// Sets the callback of the checkbox which enables/disables shadows .
	/// </summary>
	/// <param name="callback">The callback to set.</param>	
	GLvoid SetEnableShadowsCheckBoxCallback( const std::function<GLvoid( GLboolean )>& callback ) const;

	// ---------------------------------------------
	//          --- Image manipulation ---
	// ---------------------------------------------

	/// <summary>
	/// Sets the callback of the slider responsible for the gamma value.
	/// </summary>
	/// <param name="callback">The callback to set.</param>	
	GLvoid SetGammaSliderCallback( const std::function<GLvoid( GLfloat )>& callback ) const;

	/// <summary>
	/// Sets the callback of the slider responsible for the exposure value.
	/// </summary>
	/// <param name="callback">The callback to set.</param>	
	GLvoid SetExposureSliderCallback( const std::function<GLvoid( GLfloat )>& callback ) const;

	// ---------------------------------------------
	//          --- Child components ---
	// ---------------------------------------------

	PBRViewerBlinnPhongWidget* BlinnPhongTab;
	PBRViewerCookTorranceBRDFWidget* CookTorranceTab;
	PBRViewerOrenNayarBRDFWidget* OrenNayarTab;
	PBRViewerAshikhminShirleyBRDFWidget* AshikhminShirleyTab;
	PBRViewerDisneyBRDF* DisneyTab;
	PBRViewerDebugWidget* DebugTab;

	/// <summary>
	/// A std::vector of all tabs used within PBRViewer.
	/// </summary>
	std::vector<PBRViewerBXDFWidget*> Tabs;

private:
	// Lighting
	nanogui::ColorWheel* myLightingColorWheel;
	nanogui::CheckBox* myFirstLight;
	nanogui::CheckBox* mySecondLight;
	nanogui::CheckBox* myThirdLight;
	nanogui::CheckBox* myFourthLight;

	// Shadows
	nanogui::CheckBox* myEnableShadowCheckBox;

	// Output
	nanogui::ComboBox* myRenderOutputComboBox;

	// Image manipulation
	PBRViewerScalarSlider<GLfloat>* myGammaSlider;
	PBRViewerScalarSlider<GLfloat>* myExposureSlider;
};
