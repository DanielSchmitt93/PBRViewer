#pragma once

#include "PBRViewerBXDFWidget.h"
#include "PBRViewerScalarSlider.h"

/// <summary>
/// This class represents the widget used to configure the Blinn/Phong model developed by Bui Tuong Phong and Jim Blinn.
/// </summary>
class PBRViewerBlinnPhongWidget : public PBRViewerBXDFWidget
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="PBRViewerBlinnPhongWidget"/> class.
	/// </summary>
	/// <param name="parent">The parent widget.</param>
	explicit PBRViewerBlinnPhongWidget(Widget* parent);

	/// <summary>
	/// Sets the exponent parameter slider callback.
	/// </summary>
	/// <param name="callback">The callback to set.</param>	
	GLvoid SetExponentSliderCallback(const std::function<GLvoid(GLuint)>& callback) const;

private:
	PBRViewerScalarSlider<GLuint>* myExponentSlider;
};
