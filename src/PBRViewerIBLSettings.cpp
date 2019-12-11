# include "PBRViewerIBLSettings.h"

#include <nanogui/layout.h>
#include <nanogui/label.h>

#include "PBRViewerOverlayConstants.h"

/// <summary>
/// Initializes a new instance of the <see cref="PBRViewerIBLSettings"/> class.
/// </summary>
/// <param name="parent">The parent widget.</param>
PBRViewerIBLSettings::PBRViewerIBLSettings( Widget* parent ) : Window(parent, "IBL settings")
{
	setLayout(new nanogui::GroupLayout(15, 6, PBRViewerOverlayConstants::GroupLayoutSpacingBetweenLabels, 20));

	new nanogui::Label(this, "Skybox texture");
	mySkyboxTextureComboBox = new nanogui::ComboBox(this);
	
	mySkyboxTextureComboBox->setItems({"Environment", "Irradiance", "PreFilteredEnvironment"},
	                                  {"Environment", "Irradiance", "PreFiltered"});
	mySkyboxTextureComboBox->setSelectedIndex(PBRViewerEnumerations::SkyboxTexture::Environment);
	mySkyboxTextureComboBox->setFontSize(PBRViewerOverlayConstants::ButtonFontSize);
	mySkyboxTextureComboBox->setFixedWidth(200);
	mySkyboxTextureComboBox->setFixedHeight(PBRViewerOverlayConstants::ButtonHeight);
	mySkyboxTextureComboBox->setSide(nanogui::Popup::Left);	
	mySkyboxTextureComboBox->popup()->setAnchorHeight(73);
	mySkyboxTextureComboBox->setTooltip("The texture of the skybox.");

	new nanogui::Label(this, "MipMap level");
	myMipMapLevelSlider = new PBRViewerScalarSlider<GLuint>(this, std::make_pair(0, 4));	
}

/// <summary>
/// Sets the callback for the combobox representing the currently displayed texture.
/// </summary>
/// <param name="callback">The callback to set.</param>
GLvoid PBRViewerIBLSettings::SetSkyboxTextureComboBoxCallback(const std::function<GLvoid( PBRViewerEnumerations::SkyboxTexture )>& callback ) const
{
	mySkyboxTextureComboBox->setCallback([callback]( const GLint currentSkyboxTexture )
	{
		callback(static_cast<PBRViewerEnumerations::SkyboxTexture>(currentSkyboxTexture));
	});
}

/// <summary>
/// Sets the callback for the slider adjusting the currently displayed mip map level of a texture.
/// </summary>
/// <param name="callback">The callback to set.</param>
GLvoid PBRViewerIBLSettings::SetMipMapLevelSliderCallback(const std::function<GLvoid(GLuint)>& callback) const
{
	myMipMapLevelSlider->SetSliderCallback(callback);
}