#pragma once
#include "PBRViewerBXDFWidget.h"

#include <nanogui/widget.h>
#include "PBRViewerScalarSlider.h"

class PBRViewerDisneyBRDF : public PBRViewerBXDFWidget
{
public:
	explicit PBRViewerDisneyBRDF(Widget* parent);

	GLvoid SetSubsurfaceSliderCallback(const std::function<GLvoid(GLfloat)>& callback) const;

	GLvoid SetMetallicSliderCallback(const std::function<GLvoid(GLfloat)>& callback) const;

	GLvoid SetSpecularSliderCallback(const std::function<GLvoid(GLfloat)>& callback) const;

	GLvoid SetSpecularTintSliderCallback(const std::function<GLvoid(GLfloat)>& callback) const;

	GLvoid SetRoughnessSliderCallback(const std::function<GLvoid(GLfloat)>& callback) const;

	GLvoid SetAnisotropicSliderCallback(const std::function<GLvoid(GLfloat)>& callback) const;

	GLvoid SetSheenSliderCallback(const std::function<GLvoid(GLfloat)>& callback) const;

	GLvoid SetSheenTintSliderCallback(const std::function<GLvoid(GLfloat)>& callback) const;

	GLvoid SetClearcoatSliderCallback(const std::function<GLvoid(GLfloat)>& callback) const;

	GLvoid SetClearcoatGlossSliderCallback(const std::function<GLvoid(GLfloat)>& callback) const;

private:
	PBRViewerScalarSlider<GLfloat>* mySubsurfaceSlider;
	PBRViewerScalarSlider<GLfloat>* myMetallicSlider;
	PBRViewerScalarSlider<GLfloat>* mySpecularSlider;
	PBRViewerScalarSlider<GLfloat>* mySpecularTintSlider;
	PBRViewerScalarSlider<GLfloat>* myRoughnessSlider;
	PBRViewerScalarSlider<GLfloat>* myAnisotropicSlider;
	PBRViewerScalarSlider<GLfloat>* mySheenSlider;
	PBRViewerScalarSlider<GLfloat>* mySheenTintSlider;
	PBRViewerScalarSlider<GLfloat>* myClearcoatSlider;
	PBRViewerScalarSlider<GLfloat>* myClearcoatGlossSlider;
};
