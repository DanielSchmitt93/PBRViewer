# include "PBRViewerScalarSlider.h"

#include <nanogui/layout.h>
#include <nanogui/textbox.h>

#include <iomanip>
#include <sstream>

// Explicit instantiations to avoid linker errors with template class.
// See https://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
template class PBRViewerScalarSlider<GLshort>;
template class PBRViewerScalarSlider<GLushort>;
template class PBRViewerScalarSlider<GLint>;
template class PBRViewerScalarSlider<GLuint>;
template class PBRViewerScalarSlider<GLint64>;
template class PBRViewerScalarSlider<GLuint64>;
template class PBRViewerScalarSlider<GLfloat>;
template class PBRViewerScalarSlider<GLdouble>;

template <class T>
PBRViewerScalarSlider<T>::PBRViewerScalarSlider( Widget* parent,
                                                 std::pair<T, T> range,
                                                 T defaultValue ) : Widget(parent), myDefaultValue(defaultValue)
{
	setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal, nanogui::Alignment::Maximum, 0, 8));

	mySlider = new nanogui::Slider(this);

	mySlider->setRange(range);
	mySlider->setValue(static_cast<GLfloat>(myDefaultValue));
	mySlider->setFixedWidth(120);

	myTextbox = new nanogui::TextBox(this, "");
	myTextbox->setFixedSize(Eigen::Vector2i(35, 22));
	myTextbox->setFontSize(16);
	myTextbox->setEditable(GL_TRUE);
	myTextbox->setAlignment(nanogui::TextBox::Alignment::Center);
	myTextbox->setValue(TemplateToStringWithPrecision(myDefaultValue, 2));

	// Define a function so the user is able to input values directly into the textbox.
	myTextbox->setCallback([this]( std::string value )
	{
		const GLfloat enteredValue = strtof(value.c_str(), nullptr);
		if (enteredValue == mySlider->value())
		{
			return GL_FALSE;
		}

		// Check for slider range to avoid not defined values.
		if (enteredValue < mySlider->range().first || enteredValue > mySlider->range().second)
		{
			return GL_FALSE;
		}

		// Inform the controller that the user entered a value manually.
		// We just use the callback of the slider here to avoid unnecessary many callback functions.
		mySlider->callback()(enteredValue);
		mySlider->setValue(enteredValue);
		return GL_TRUE;
	});
}

template <class T>
GLvoid PBRViewerScalarSlider<T>::SetSliderCallback( const std::function<GLvoid( T )>& callback ) const
{
	mySlider->setCallback([this, callback]( T value )
	{
		callback(value);

		// Only show two decimal digits
		myTextbox->setValue(TemplateToStringWithPrecision(value, 2));
	});
}

template <class T>
GLvoid PBRViewerScalarSlider<T>::SetEnabledRecursive( const GLboolean enabled ) const
{
	mySlider->setEnabled(enabled);

	myTextbox->setEnabled(enabled);
	myTextbox->setEditable(enabled);
}

template <class T>
std::string PBRViewerScalarSlider<T>::TemplateToStringWithPrecision( T value, const GLint precision ) const
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(precision) << value;
	return stream.str();
}
