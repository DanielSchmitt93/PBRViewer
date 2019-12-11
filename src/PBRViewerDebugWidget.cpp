#include "PBRViewerDebugWidget.h"

#include "PBRViewerOverlayConstants.h"

#include <nanogui/label.h>
#include <nanogui/messagedialog.h>

// ReSharper disable once CppUnusedIncludeDirective
// This include is needed to recognize the screen() method.
#include <nanogui/screen.h>

/// <summary>
/// Initializes a new instance of the <see cref="PBRViewerDebugWidget"/> class.
/// </summary>
/// <param name="parent">The parent widget.</param>
PBRViewerDebugWidget::PBRViewerDebugWidget( Widget* parent ) : PBRViewerBXDFWidget(parent)
{
	ActivationButton = new nanogui::Button(this, "Activate Debug");
	ActivationButton->setFlags(nanogui::Button::ToggleButton);
	ActivationButton->setPushed(GL_FALSE);
	ActivationButton->setIcon(ENTYPO_ICON_CROSS);
	ActivationButton->setFontSize(PBRViewerOverlayConstants::ButtonFontSize);
	ActivationButton->setFixedHeight(PBRViewerOverlayConstants::ButtonHeight);
	ActivationButton->setTooltip("Activate debug shading for the selected model.");

	new nanogui::Label(this, "Debug output");
	const std::vector<std::string> debugComboBoxItems = {
		"Negative n dot l", "Negative n dot v", "Normal vectors", "Tangent vectors", "Bitangent vectors", "WWFT GGX"
	};
	myOutputComboBox = new nanogui::ComboBox(this, debugComboBoxItems);
	myOutputComboBox->setSelectedIndex(PBRViewerEnumerations::DebugOutput::NegativeNDotL);
	myOutputComboBox->setFontSize(PBRViewerOverlayConstants::ButtonFontSize);
	myOutputComboBox->setFixedHeight(PBRViewerOverlayConstants::ButtonHeight);
	myOutputComboBox->setTooltip("Defines the output of the debug shader.");
}

/// <summary>
/// Sets the callback for the combobox representing the debug value.
/// </summary>
/// <param name="callback">The callback to set.</param>
GLvoid PBRViewerDebugWidget::SetOutputComboBoxCallback( const std::function<GLvoid( PBRViewerEnumerations::DebugOutput )>& callback )
{
	myOutputComboBox->setCallback([this, callback]( const GLint currentDebugOutput )
	{
		const auto castOutput = static_cast<PBRViewerEnumerations::DebugOutput>(currentDebugOutput);

		// Warn the user before calculating the Weak White Furnance Test because it is very expensive (program may stop responding on weaker computers).
		if (PBRViewerEnumerations::DebugOutput::WWFT_GGX == castOutput)
		{
			auto* messageDialog = new nanogui::MessageDialog(screen(),
			                                                 nanogui::MessageDialog::Type::Question,
			                                                 "Perform Weak White Furnance Test",
			                                                 "Do you really would like to perform the Weak White Furnance Test? This test is very expensive to calculate and can significantly reduce the frame rate.",
			                                                 "Execute",
			                                                 "Cancel",
			                                                 GL_TRUE);
			messageDialog->setModal(GL_TRUE);
			messageDialog->setCallback([this, callback, castOutput]( const GLint result )
			{
				if (0 == result)
				{
					callback(castOutput);
				}
				else
				{
					// Reset state of combobox to first entry.
					myOutputComboBox->setSelectedIndex(0);
					const auto defaultOutputAfterCancel = static_cast<PBRViewerEnumerations::DebugOutput>(0);
					callback(defaultOutputAfterCancel);
				}
			});
		}
		else
		{
			callback(castOutput);
		}
	});
}