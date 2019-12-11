#include "PBRViewerDisneyBRDF.h"
#include "PBRViewerOverlayConstants.h"

#include <nanogui/label.h>

PBRViewerDisneyBRDF::PBRViewerDisneyBRDF( Widget* parent ) : PBRViewerBXDFWidget(parent)
{
	setLayout(new nanogui::GroupLayout(15, 3, 4, 5));

	ActivationButton = new nanogui::Button(this, "Activate Disney BRDF");
	ActivationButton->setFlags(nanogui::Button::ToggleButton);
	ActivationButton->setPushed(GL_FALSE);
	ActivationButton->setIcon(ENTYPO_ICON_CROSS);
	ActivationButton->setFontSize(PBRViewerOverlayConstants::ButtonFontSize);
	ActivationButton->setFixedHeight(PBRViewerOverlayConstants::ButtonHeight);
	ActivationButton->setTooltip("Activate Disney shading with integrated subsurface scattering for the selected model.");

	new nanogui::Label(this, "Subsurface");
	mySubsurfaceSlider = new PBRViewerScalarSlider<GLfloat>(this, std::make_pair(0.0f, 1.0f), 0.5f);

	new nanogui::Label(this, "Metallic");
	myMetallicSlider = new PBRViewerScalarSlider<GLfloat>(this, std::make_pair(0.0f, 1.0f), 0.5f);

	new nanogui::Label(this, "Specular");
	mySpecularSlider = new PBRViewerScalarSlider<GLfloat>(this, std::make_pair(0.0f, 1.0f), 0.5f);

	new nanogui::Label(this, "Specular tint");
	mySpecularTintSlider = new PBRViewerScalarSlider<GLfloat>(this, std::make_pair(0.0f, 1.0f), 0.5f);

	new nanogui::Label(this, "Roughness");
	myRoughnessSlider = new PBRViewerScalarSlider<GLfloat>(this, std::make_pair(0.001f, 1.0f), 0.5f);

	new nanogui::Label(this, "Anisotropic");
	myAnisotropicSlider = new PBRViewerScalarSlider<GLfloat>(this, std::make_pair(0.0f, 1.0f), 0.5f);

	new nanogui::Label(this, "Sheen");
	mySheenSlider = new PBRViewerScalarSlider<GLfloat>(this, std::make_pair(0.0f, 1.0f), 0.5f);

	new nanogui::Label(this, "Sheen tint");
	mySheenTintSlider = new PBRViewerScalarSlider<GLfloat>(this, std::make_pair(0.0f, 1.0f), 0.5f);

	new nanogui::Label(this, "Clearcoat");
	myClearcoatSlider = new PBRViewerScalarSlider<GLfloat>(this, std::make_pair(0.0f, 1.0f), 0.5f);

	new nanogui::Label(this, "Clearcoat gloss");
	myClearcoatGlossSlider = new PBRViewerScalarSlider<GLfloat>(this, std::make_pair(0.0f, 1.0f), 0.5f);
}

GLvoid PBRViewerDisneyBRDF::SetSubsurfaceSliderCallback( const std::function<GLvoid(GLfloat)>& callback ) const
{
	mySubsurfaceSlider->SetSliderCallback(callback);
}

GLvoid PBRViewerDisneyBRDF::SetMetallicSliderCallback( const std::function<GLvoid(GLfloat)>& callback ) const
{
	myMetallicSlider->SetSliderCallback(callback);
}

GLvoid PBRViewerDisneyBRDF::SetSpecularSliderCallback( const std::function<GLvoid(GLfloat)>& callback ) const
{
	mySpecularSlider->SetSliderCallback(callback);
}

GLvoid PBRViewerDisneyBRDF::SetSpecularTintSliderCallback( const std::function<GLvoid(GLfloat)>& callback ) const
{
	mySpecularTintSlider->SetSliderCallback(callback);
}

GLvoid PBRViewerDisneyBRDF::SetRoughnessSliderCallback( const std::function<GLvoid(GLfloat)>& callback ) const
{
	myRoughnessSlider->SetSliderCallback(callback);
}

GLvoid PBRViewerDisneyBRDF::SetAnisotropicSliderCallback( const std::function<GLvoid(GLfloat)>& callback ) const
{
	myAnisotropicSlider->SetSliderCallback(callback);
}

GLvoid PBRViewerDisneyBRDF::SetSheenSliderCallback( const std::function<GLvoid(GLfloat)>& callback ) const
{
	mySheenSlider->SetSliderCallback(callback);
}

GLvoid PBRViewerDisneyBRDF::SetSheenTintSliderCallback( const std::function<GLvoid(GLfloat)>& callback ) const
{
	mySheenTintSlider->SetSliderCallback(callback);
}

GLvoid PBRViewerDisneyBRDF::SetClearcoatSliderCallback( const std::function<GLvoid(GLfloat)>& callback ) const
{
	myClearcoatSlider->SetSliderCallback(callback);
}

GLvoid PBRViewerDisneyBRDF::SetClearcoatGlossSliderCallback( const std::function<GLvoid(GLfloat)>& callback ) const
{
	myClearcoatGlossSlider->SetSliderCallback(callback);
}
