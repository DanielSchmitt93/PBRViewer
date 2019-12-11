#pragma once

#include "PBRViewerBXDFWidget.h"
#include "PBRViewerEnumerations.h"

#include <nanogui/widget.h>
#include <nanogui/combobox.h>

/// <summary>
/// This class represents the widget used to display runtime values of shader instances which might be useful for debugging purposes.
/// </summary>
class PBRViewerDebugWidget : public PBRViewerBXDFWidget
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="PBRViewerDebugWidget"/> class.
	/// </summary>
	/// <param name="parent">The parent widget.</param>
	explicit PBRViewerDebugWidget(Widget* parent);

	/// <summary>
	/// Sets the callback for the combobox representing the debug value.
	/// </summary>
	/// <param name="callback">The callback to set.</param>
	GLvoid SetOutputComboBoxCallback(const std::function<GLvoid(PBRViewerEnumerations::DebugOutput)>& callback);
	
private:
	nanogui::ComboBox* myOutputComboBox;	
};
