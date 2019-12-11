#include "PBRViewerAshikhminShirleyBRDFWidget.h"

#include "PBRViewerOverlayConstants.h"

#include <nanogui/label.h>

/// <summary>
/// Initializes a new instance of the <see cref="PBRViewerAshikhminShirleyBRDFWidget"/> class.
/// </summary>
/// <param name="parent">The parent widget.</param>
PBRViewerAshikhminShirleyBRDFWidget::PBRViewerAshikhminShirleyBRDFWidget( Widget* parent ) : PBRViewerBXDFWidget(parent)
{
	ActivationButton = new nanogui::Button(this, "Activate Ashikhmin-Shirley");
	ActivationButton->setFlags(nanogui::Button::ToggleButton);
	ActivationButton->setPushed(GL_FALSE);
	ActivationButton->setIcon(ENTYPO_ICON_CROSS);
	ActivationButton->setFontSize(PBRViewerOverlayConstants::ButtonFontSize);
	ActivationButton->setFixedHeight(PBRViewerOverlayConstants::ButtonHeight);
	ActivationButton->setTooltip("Activate Ashikhmin-Shirley shading for the selected model.");

	new nanogui::Label(this, "Anisotropic scaling");
	new nanogui::Label(this, "n_u (will be multiplied by 10)");
	myNuSlider = new PBRViewerScalarSlider<GLuint>(this, std::make_pair(1u, 1000u), 500u);

	new nanogui::Label(this, "n_v (will be multiplied by 10)");
	myNvSlider = new PBRViewerScalarSlider<GLuint>(this, std::make_pair(1u, 1000u), 500u);
}

/// <summary>
/// Sets the n_u parameter slider callback.
/// </summary>
/// <param name="callback">The callback to set.</param>	
GLvoid PBRViewerAshikhminShirleyBRDFWidget::SetNuSliderCallback( const std::function<GLvoid(GLuint)>& callback ) const
{
	myNuSlider->SetSliderCallback(callback);
}

/// <summary>
/// Sets the n_v parameter slider callback.
/// </summary>
/// <param name="callback">The callback to set.</param>
GLvoid PBRViewerAshikhminShirleyBRDFWidget::SetNvSliderCallback( const std::function<GLvoid(GLuint)>& callback ) const
{
	myNvSlider->SetSliderCallback(callback);
}
