#pragma once

#include "PBRViewerBXDFWidget.h"
#include "PBRViewerScalarSlider.h"

#include <nanogui/checkbox.h>

/// <summary>
/// This class represents the widget used to configure the BRDF developed by Michael Oren and Shree K. Nayar.
/// </summary>
class PBRViewerOrenNayarBRDFWidget : public PBRViewerBXDFWidget
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="PBRViewerOrenNayarBRDFWidget"/> class.
	/// </summary>
	/// <param name="parent">The parent widget.</param>
	explicit PBRViewerOrenNayarBRDFWidget(Widget* parent);

	/// <summary>
	/// Sets the callback for the checkbox indicating if custom materials should be used.
	/// </summary>
	/// <param name="callback">The callback to set.</param>
	GLvoid SetEnableCustomMaterialValuesCheckBoxCallback(const std::function<GLvoid(GLboolean)>& callback);

	/// <summary>
	/// Sets the roughness parameter slider callback.
	/// </summary>
	/// <param name="callback">The callback to set.</param>	
	GLvoid SetCustomRoughnessSliderCallback(const std::function<GLvoid(GLfloat)>& callback) const;

private:
	// Materials
	nanogui::CheckBox* myEnableCustomMaterialValuesCheckBox;
	PBRViewerScalarSlider<GLfloat>* myCustomRoughnessSlider;
};
