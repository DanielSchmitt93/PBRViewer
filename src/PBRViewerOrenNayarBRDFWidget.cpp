#include "PBRViewerOrenNayarBRDFWidget.h"

#include "PBRViewerOverlayConstants.h"

#include <nanogui/label.h>

/// <summary>
/// Initializes a new instance of the <see cref="PBRViewerOrenNayarBRDFWidget"/> class.
/// </summary>
/// <param name="parent">The parent widget.</param>
PBRViewerOrenNayarBRDFWidget::PBRViewerOrenNayarBRDFWidget( Widget* parent ) : PBRViewerBXDFWidget(parent)
{
	ActivationButton = new nanogui::Button(this, "Activate Oren-Nayar");
	ActivationButton->setFlags(nanogui::Button::ToggleButton);
	ActivationButton->setPushed(GL_FALSE);
	ActivationButton->setIcon(ENTYPO_ICON_CROSS);
	ActivationButton->setFontSize(PBRViewerOverlayConstants::ButtonFontSize);
	ActivationButton->setFixedHeight(PBRViewerOverlayConstants::ButtonHeight);
	ActivationButton->setTooltip("Activate Oren-Nayar shading for the selected model.");

	// ------------------------------------------------------------
	//                    --- Custom material values ---
	// ------------------------------------------------------------	
	new nanogui::Label(this, "", "sans");
	myEnableCustomMaterialValuesCheckBox = new nanogui::CheckBox(this, "Enable custom values");
	myEnableCustomMaterialValuesCheckBox->setPushed(GL_FALSE);
	myEnableCustomMaterialValuesCheckBox->setTooltip("Enables/Disables custom roughness values.");

	auto* roughnessLabel = new nanogui::Label(this, "Custom roughness", "sans");
	roughnessLabel->setTooltip(std::string("Please note: Oren-Nayar needs the roughness values in the intervall [0, Pi / 2]"));

	myCustomRoughnessSlider = new PBRViewerScalarSlider<GLfloat>(this, std::make_pair(0.0f, 1.5707f), 0.7853f);
	myCustomRoughnessSlider->SetEnabledRecursive(myEnableCustomMaterialValuesCheckBox->pushed());
}

/// <summary>
/// Sets the callback for the checkbox indicating if custom materials should be used.
/// </summary>
/// <param name="callback">The callback to set.</param>
GLvoid PBRViewerOrenNayarBRDFWidget::SetEnableCustomMaterialValuesCheckBoxCallback( const std::function<GLvoid( GLboolean )>& callback )
{
	myEnableCustomMaterialValuesCheckBox->setCallback([this, callback]( const GLboolean activated )
	{
		callback(activated);

		myCustomRoughnessSlider->SetEnabledRecursive(activated);
	});
}

/// <summary>
/// Sets the roughness parameter slider callback.
/// </summary>
/// <param name="callback">The callback to set.</param>	
GLvoid PBRViewerOrenNayarBRDFWidget::SetCustomRoughnessSliderCallback( const std::function<GLvoid( GLfloat )>& callback ) const
{
	myCustomRoughnessSlider->SetSliderCallback(callback);
}
