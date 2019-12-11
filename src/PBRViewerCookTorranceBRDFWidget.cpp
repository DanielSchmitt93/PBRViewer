#include "PBRViewerCookTorranceBRDFWidget.h"

#include "PBRViewerOverlayConstants.h"

#include <nanogui/label.h>

/// <summary>
/// Initializes a new instance of the <see cref="PBRViewerCookTorranceBRDFWidget"/> class.
/// </summary>
/// <param name="parent">The parent widget.</param>
PBRViewerCookTorranceBRDFWidget::PBRViewerCookTorranceBRDFWidget( Widget* parent ) : PBRViewerBXDFWidget(parent)
{
	ActivationButton = new nanogui::Button(this, "Activate Cook-Torrance");
	ActivationButton->setFlags(nanogui::Button::ToggleButton);
	ActivationButton->setPushed(GL_TRUE);
	ActivationButton->setIcon(ENTYPO_ICON_CHECK);
	ActivationButton->setFontSize(PBRViewerOverlayConstants::ButtonFontSize);
	ActivationButton->setFixedHeight(PBRViewerOverlayConstants::ButtonHeight);
	ActivationButton->setTooltip("Activate Cook-Torrance shading for the selected model.");

	// ------------------------------------------------------------
	//                    --- Diffuse term ---
	// ------------------------------------------------------------
	new nanogui::Label(this, "Diffuse term", "sans");
	myDiffuseTerm = new nanogui::ComboBox(this);
	myDiffuseTerm->setItems({"No diffuse term", "Lambertian", "LambertianEnergyConserving", "Burley", "Shirley et al.", "AshikhminShirley"},
	                        {"None", "Lambert", "LambertEnergyCons", "Burley", "Shirley et al.", "AshikhminShirley"});
	myDiffuseTerm->setSelectedIndex(PBRViewerEnumerations::DiffuseTerm::Burley);
	myDiffuseTerm->setFontSize(PBRViewerOverlayConstants::ButtonFontSize);
	myDiffuseTerm->setFixedHeight(PBRViewerOverlayConstants::ButtonHeight);
	myDiffuseTerm->setTooltip("The diffuse term models the portion of light that returns to the surface through subsurface scattering.");

	// ------------------------------------------------------------
	//                    --- Fresnel term ---
	// ------------------------------------------------------------
	new nanogui::Label(this, "Fresnel term", "sans");
	myFresnelTerm = new nanogui::ComboBox(this);
	myFresnelTerm->setItems({"Normal incidence", "Schlick's approximation"},
	                        {"Normal incidence", "Schlick"});
	myFresnelTerm->setSelectedIndex(PBRViewerEnumerations::FresnelTerm::Schlick);
	myFresnelTerm->setFontSize(PBRViewerOverlayConstants::ButtonFontSize);
	myFresnelTerm->setFixedHeight(PBRViewerOverlayConstants::ButtonHeight);
	myFresnelTerm->setTooltip("The Fresnel term describes the ratio of light that gets reflected over the light that gets refracted.");

	// ------------------------------------------------------------
	//                    --- Normal distribution term ---
	// ------------------------------------------------------------
	new nanogui::Label(this, "Normal distribution term", "sans");
	myNormalDistributionTerm = new nanogui::ComboBox(this);
	myNormalDistributionTerm->setItems({"Constant value", "Trowbridge-Reitz GGX", "Beckmann", "Blinn/Phong"},
	                                   {"Constant", "Trowbridge-Reitz", "Beckmann", "Blinn/Phong"});
	myNormalDistributionTerm->setSelectedIndex(PBRViewerEnumerations::NormalDistributionTerm::TrowbridgeReitzGGX);
	myNormalDistributionTerm->setFontSize(PBRViewerOverlayConstants::ButtonFontSize);
	myNormalDistributionTerm->setFixedHeight(PBRViewerOverlayConstants::ButtonHeight);
	myNormalDistributionTerm->setTooltip(
		"The normal distribution function D statistically approximates the relative surface area of microfacets exactly aligned to the halfway vector.");

	new nanogui::Label(this, "Geometry term", "sans");
	myGeometryTerm = new nanogui::ComboBox(this);
	myGeometryTerm->setItems({
		                         "Constant term", "No-G model", "Separable Schlick GGX", "Separable Smith GGX", "Separable Smith Beckmann",
		                         "Smith Height Correlated GGX", "HeitzSmith Height-Direction Correlated GGX"
	                         },
	                         {"Constant", "No-G model", "Schlick GGX", "Smith GGX", "Smith Beckmann", "Smith Height GGX", "HeitzSmith GGX"});
	myGeometryTerm->setSelectedIndex(PBRViewerEnumerations::GeometryTerm::SeparableSchlick_GGX);
	myGeometryTerm->setFontSize(PBRViewerOverlayConstants::ButtonFontSize);
	myGeometryTerm->setFixedHeight(PBRViewerOverlayConstants::ButtonHeight);
	myGeometryTerm->setTooltip(
		"The geometry function statistically approximates the relative surface area where its micro surface-details overshadow each other causing light rays to be occluded.");

	// ------------------------------------------------------------
	//                    --- Custom material values ---
	// ------------------------------------------------------------	
	new nanogui::Label(this, "", "sans");
	myEnableCustomMaterialValuesCheckBox = new nanogui::CheckBox(this, "Enable custom values");
	myEnableCustomMaterialValuesCheckBox->setPushed(GL_FALSE);
	myEnableCustomMaterialValuesCheckBox->setTooltip("Enables/Disables custom roughness and metalness values.");

	new nanogui::Label(this, "Custom metalness", "sans");
	myCustomMetalnessSlider = new PBRViewerScalarSlider<GLfloat>(this, std::make_pair(0.0f, 1.0f), 0.5f);
	myCustomMetalnessSlider->SetEnabledRecursive(myEnableCustomMaterialValuesCheckBox->pushed());

	auto* roughnessLabel = new nanogui::Label(this, "Custom roughness", "sans");
	roughnessLabel->setTooltip(
		std::string(
			"Please note: we reversed the roughness mapping for the Blinn/Phong NDF which means that a high roughness value equals a rough surface")
		+ std::string("(it is more consistent with the other NDFs and allows a direct comparison without changing the roughness value)."));

	myCustomRoughnessSlider = new PBRViewerScalarSlider<GLfloat>(this, std::make_pair(0.001f, 1.0f), 0.5f);
	myCustomRoughnessSlider->SetEnabledRecursive(myEnableCustomMaterialValuesCheckBox->pushed());
}

/// <summary>
/// Sets the callback for the combobox representing the diffuse term.
/// </summary>
/// <param name="callback">The callback to set.</param>
GLvoid PBRViewerCookTorranceBRDFWidget::SetDiffuseTermComboBoxCallback(
	const std::function<GLvoid( PBRViewerEnumerations::DiffuseTerm )>& callback ) const
{
	myDiffuseTerm->setCallback([callback]( const GLint currentDiffuseTerm )
	{
		callback(static_cast<PBRViewerEnumerations::DiffuseTerm>(currentDiffuseTerm));
	});
}

/// <summary>
/// Sets the callback for the combobox representing the fresnel term.
/// </summary>
/// <param name="callback">The callback to set.</param>
GLvoid PBRViewerCookTorranceBRDFWidget::SetFresnelTermComboBoxCallback(
	const std::function<GLvoid( PBRViewerEnumerations::FresnelTerm )>& callback ) const
{
	myFresnelTerm->setCallback([callback]( const GLint currentFresnelTerm )
	{
		callback(static_cast<PBRViewerEnumerations::FresnelTerm>(currentFresnelTerm));
	});
}

/// <summary>
/// Sets the callback for the combobox representing the normal distribution function.
/// </summary>
/// <param name="callback">The callback to set.</param>
GLvoid PBRViewerCookTorranceBRDFWidget::SetNormalDistributionComboBoxCallback(
	const std::function<GLvoid( PBRViewerEnumerations::NormalDistributionTerm )>& callback ) const
{
	myNormalDistributionTerm->setCallback([callback]( const GLint currentNormalDistributionTerm )
	{
		callback(static_cast<PBRViewerEnumerations::NormalDistributionTerm>(currentNormalDistributionTerm));
	});
}

/// <summary>
/// Sets the callback for the combobox representing the geometry function.
/// </summary>
/// <param name="callback">The callback to set.</param>
GLvoid PBRViewerCookTorranceBRDFWidget::SetGeometryTermComboBoxCallback(
	const std::function<GLvoid( PBRViewerEnumerations::GeometryTerm )>& callback ) const
{
	myGeometryTerm->setCallback([callback]( const GLint currentGeometryTerm )
	{
		callback(static_cast<PBRViewerEnumerations::GeometryTerm>(currentGeometryTerm));
	});
}

/// <summary>
/// Sets the callback for the checkbox indicating if custom materials should be used.
/// </summary>
/// <param name="callback">The callback to set.</param>
GLvoid PBRViewerCookTorranceBRDFWidget::SetEnableCustomMaterialValuesCheckBoxCallback( const std::function<GLvoid( GLboolean )>& callback )
{
	myEnableCustomMaterialValuesCheckBox->setCallback([this, callback]( const GLboolean activated )
	{
		callback(activated);

		myCustomRoughnessSlider->SetEnabledRecursive(activated);
		myCustomMetalnessSlider->SetEnabledRecursive(activated);
	});
}

/// <summary>
/// Sets the metalness parameter slider callback.
/// </summary>
/// <param name="callback">The callback to set.</param>	
GLvoid PBRViewerCookTorranceBRDFWidget::SetCustomMetalnessSliderCallback( const std::function<GLvoid( GLfloat )>& callback ) const
{
	myCustomMetalnessSlider->SetSliderCallback(callback);
}

/// <summary>
/// Sets the roughness parameter slider callback.
/// </summary>
/// <param name="callback">The callback to set.</param>	
GLvoid PBRViewerCookTorranceBRDFWidget::SetCustomRoughnessSliderCallback( const std::function<GLvoid( GLfloat )>& callback ) const
{
	myCustomRoughnessSlider->SetSliderCallback(callback);
}
