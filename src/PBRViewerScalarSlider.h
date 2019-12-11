#pragma once

#include "glad/glad.h"

#include "nanogui/slider.h"
#include "nanogui/textbox.h"

template<class T> class PBRViewerScalarSlider : public nanogui::Widget
{
public:
	PBRViewerScalarSlider( Widget* parent,
	                       std::pair<T, T> range,
	                       T defaultValue = T() );

	GLvoid SetSliderCallback( const std::function<GLvoid( T )>& callback ) const;

	/// <summary>
	/// Sets the enabled flag recursive for all childs of this widget.
	/// Unfortunately, nanogui does not provide a function for this so we define it ourselves.
	/// </summary>
	/// <param name="enabled">if set to <c>true</c> [enabled].</param>
	GLvoid SetEnabledRecursive(GLboolean enabled) const;

private:
	nanogui::Slider* mySlider;
	nanogui::TextBox* myTextbox;
	T myDefaultValue;

	std::string TemplateToStringWithPrecision( T value, GLint precision ) const;
};