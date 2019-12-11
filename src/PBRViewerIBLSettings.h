#pragma once

#include <glad/glad.h>

#include <nanogui/window.h>
#include <nanogui/combobox.h>

#include "PBRViewerEnumerations.h"
#include "PBRViewerScalarSlider.h"

/// <summary>
/// This class represents the window used to configure Image Based Lighting (IBL).
/// </summary>
class PBRViewerIBLSettings : public nanogui::Window
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="PBRViewerIBLSettings"/> class.
	/// </summary>
	/// <param name="parent">The parent widget.</param>
	PBRViewerIBLSettings(Widget* parent);

	/// <summary>
	/// Sets the callback for the combobox representing the currently displayed texture.
	/// </summary>
	/// <param name="callback">The callback to set.</param>
	GLvoid SetSkyboxTextureComboBoxCallback(const std::function<GLvoid(PBRViewerEnumerations::SkyboxTexture)>& callback) const;

	/// <summary>
	/// Sets the callback for the slider adjusting the currently displayed mip map level of a texture.
	/// </summary>
	/// <param name="callback">The callback to set.</param>
	GLvoid SetMipMapLevelSliderCallback(const std::function<GLvoid(GLuint)>& callback) const;

private:
	nanogui::ComboBox* mySkyboxTextureComboBox;
	PBRViewerScalarSlider<GLuint>* myMipMapLevelSlider;
};
