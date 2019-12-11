#pragma once

#include "PBRViewerBXDFWidget.h"
#include "PBRViewerScalarSlider.h"
#include "PBRViewerEnumerations.h"

#include <nanogui/combobox.h>
#include <nanogui/checkbox.h>

/// <summary>
/// This class represents the widget used to configure the BRDF developed by Robert L. Cook and Kenneth E. Torrance.
/// </summary>
class PBRViewerCookTorranceBRDFWidget: public PBRViewerBXDFWidget
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="PBRViewerCookTorranceBRDFWidget"/> class.
	/// </summary>
	/// <param name="parent">The parent widget.</param>
	explicit PBRViewerCookTorranceBRDFWidget(Widget* parent);

	/// <summary>
	/// Sets the callback for the combobox representing the diffuse term.
	/// </summary>
	/// <param name="callback">The callback to set.</param>
	GLvoid SetDiffuseTermComboBoxCallback(const std::function<GLvoid(PBRViewerEnumerations::DiffuseTerm)>& callback) const;

	/// <summary>
	/// Sets the callback for the combobox representing the fresnel term.
	/// </summary>
	/// <param name="callback">The callback to set.</param>
	GLvoid SetFresnelTermComboBoxCallback( const std::function<GLvoid( PBRViewerEnumerations::FresnelTerm )>& callback ) const;

	/// <summary>
	/// Sets the callback for the combobox representing the normal distribution function.
	/// </summary>
	/// <param name="callback">The callback to set.</param>
	GLvoid SetNormalDistributionComboBoxCallback(
		const std::function<GLvoid( PBRViewerEnumerations::NormalDistributionTerm )>& callback ) const;

	/// <summary>
	/// Sets the callback for the combobox representing the geometry function.
	/// </summary>
	/// <param name="callback">The callback to set.</param>
	GLvoid SetGeometryTermComboBoxCallback( const std::function<GLvoid( PBRViewerEnumerations::GeometryTerm )>& callback ) const;
	
	/// <summary>
	/// Sets the callback for the checkbox indicating if custom materials should be used.
	/// </summary>
	/// <param name="callback">The callback to set.</param>
	GLvoid SetEnableCustomMaterialValuesCheckBoxCallback(const std::function<GLvoid(GLboolean)>& callback);

	/// <summary>
	/// Sets the metalness parameter slider callback.
	/// </summary>
	/// <param name="callback">The callback to set.</param>	
	GLvoid SetCustomMetalnessSliderCallback( const std::function<GLvoid( GLfloat )>& callback ) const;

	/// <summary>
	/// Sets the roughness parameter slider callback.
	/// </summary>
	/// <param name="callback">The callback to set.</param>	
	GLvoid SetCustomRoughnessSliderCallback( const std::function<GLvoid( GLfloat )>& callback ) const;

private:
	nanogui::ComboBox* myDiffuseTerm;
	nanogui::ComboBox* myFresnelTerm;
	nanogui::ComboBox* myNormalDistributionTerm;
	nanogui::ComboBox* myGeometryTerm;

	// Materials
	nanogui::CheckBox* myEnableCustomMaterialValuesCheckBox;
	PBRViewerScalarSlider<GLfloat>* myCustomMetalnessSlider;
	PBRViewerScalarSlider<GLfloat>* myCustomRoughnessSlider;
};
