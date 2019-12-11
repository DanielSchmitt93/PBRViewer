#pragma once

#include <glad/glad.h>

#include <nanogui/widget.h>
#include <nanogui/button.h>
#include <nanogui/entypo.h>
#include <nanogui/layout.h>

/// <summary>
/// This is the base class for every widget representing a lighting model.
/// Every lighting widget is derived from this class and has to initialize the activation button.
/// This class should not be instantiated as a standalone object.
/// </summary>
class PBRViewerBXDFWidget : public nanogui::Widget
{
public:
	/// <summary>
	/// Sets the activation button callback.
	/// </summary>
	/// <param name="callback">The callback to set.</param>
	GLvoid SetActivationButtonCallback(const std::function<GLvoid(GLboolean)>& callback) const
	{
		ActivationButton->setChangeCallback([callback](const GLboolean activated)
		{
			callback(activated);
		});
	}

	/// <summary>
	/// Enables or disables the activation button.
	/// </summary>
	/// <param name="activated">The state of the activation button.</param>	
	GLvoid SetActivationButtonActive(const GLboolean activated) const
	{
		if(activated)
		{
			ActivationButton->setIcon(ENTYPO_ICON_CHECK);
			ActivationButton->setPushed(GL_TRUE);
		}
		else
		{
			ActivationButton->setIcon(ENTYPO_ICON_CROSS);
			ActivationButton->setPushed(GL_FALSE);
		}		
	}	

protected:
	/// <summary>
	/// The activation button of the lighting model.
	/// </summary>
	nanogui::Button* ActivationButton = nullptr;

	/// <summary>
	/// Initializes a new instance of the <see cref="PBRViewerBXDFWidget"/> class.
	/// </summary>
	/// <param name="parent">The parent widget.</param>
	explicit PBRViewerBXDFWidget(Widget* parent) : Widget(parent)
	{
		setLayout(new nanogui::GroupLayout());
	}
};
