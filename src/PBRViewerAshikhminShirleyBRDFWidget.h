#pragma once
#include "PBRViewerBXDFWidget.h"

#include "PBRViewerScalarSlider.h"

#include <nanogui/widget.h>

/// <summary>
/// This class represents the widget used to configure the BRDF developed by Michael Ashikhmin and Peter Shirley 
/// in their paper "An Anisotropic Phong BRDF Model".
/// </summary>
class PBRViewerAshikhminShirleyBRDFWidget : public PBRViewerBXDFWidget
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="PBRViewerAshikhminShirleyBRDFWidget"/> class.
	/// </summary>
	/// <param name="parent">The parent widget.</param>
	explicit PBRViewerAshikhminShirleyBRDFWidget( Widget* parent );

	/// <summary>
	/// Sets the n_u parameter slider callback.
	/// </summary>
	/// <param name="callback">The callback to set.</param>	
	GLvoid SetNuSliderCallback( const std::function<GLvoid( GLuint )>& callback ) const;

	/// <summary>
	/// Sets the n_v parameter slider callback.
	/// </summary>
	/// <param name="callback">The callback to set.</param>
	GLvoid SetNvSliderCallback( const std::function<GLvoid( GLuint )>& callback ) const;

private:
	// n_u and n_v are the names of the variables controlling the anisotropic highlight in the original paper by Michael Ashikmin and Peter Shirley.
	PBRViewerScalarSlider<GLuint>* myNuSlider;
	PBRViewerScalarSlider<GLuint>* myNvSlider;
};
