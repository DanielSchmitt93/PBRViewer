#include "PBRViewerBlinnPhongWidget.h"

#include "PBRViewerOverlayConstants.h"

#include "nanogui/label.h"

/// <summary>
/// Initializes a new instance of the <see cref="PBRViewerBlinnPhongWidget"/> class.
/// </summary>
/// <param name="parent">The parent widget.</param>
PBRViewerBlinnPhongWidget::PBRViewerBlinnPhongWidget( Widget* parent ) : PBRViewerBXDFWidget(parent)
{
	ActivationButton = new nanogui::Button(this, "Activate Blinn-Phong");
	ActivationButton->setFlags(nanogui::Button::ToggleButton);
	ActivationButton->setPushed(GL_FALSE);
	ActivationButton->setIcon(ENTYPO_ICON_CROSS);
	ActivationButton->setFontSize(PBRViewerOverlayConstants::ButtonFontSize);
	ActivationButton->setFixedHeight(PBRViewerOverlayConstants::ButtonHeight);
	ActivationButton->setTooltip("Activate Blinn-Phong shading for the selected model.");

	new nanogui::Label(this, "Exponent");
	myExponentSlider = new PBRViewerScalarSlider<GLuint>(this, std::make_pair(2u, 128u), 64u);
}

/// <summary>
/// Sets the exponent parameter slider callback.
/// </summary>
/// <param name="callback">The callback to set.</param>	
GLvoid PBRViewerBlinnPhongWidget::SetExponentSliderCallback( const std::function<GLvoid(GLuint)>& callback ) const
{
	myExponentSlider->SetSliderCallback(callback);
}
