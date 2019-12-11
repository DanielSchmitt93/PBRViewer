#include "PBRViewerPointLight.h"
#include "PBRViewerLogger.h"

const static std::string DefaultModelPath = R"(..\..\resources\objects\CeilingLight\CeilingLight.gltf)";

// Only one shader and one model for all instance of a light.
static std::shared_ptr<PBRViewerShader> myShader;
static std::shared_ptr<PBRViewerScene> myModel;

/// <summary>
/// Initializes a new instance of the <see cref="PBRViewerPointLight"/> class.
/// Uses the default 3D model as representation.
/// The light is inactive by default.
/// </summary>
PBRViewerPointLight::PBRViewerPointLight() : PBRViewerPointLight(DefaultModelPath) {}

/// <summary>
/// Initializes a new instance of the <see cref="PBRViewerPointLight"/> class.
/// The light is inactive by default.
/// </summary>
/// <param name="pathToModel">The filepath to the 3D model representing the light source.</param>
PBRViewerPointLight::PBRViewerPointLight( const std::string& pathToModel )
{
	if (nullptr == myShader)
	{
		myShader = std::make_shared<PBRViewerShader>("CommonVertexShader.vert", "lightsource.frag");
		myShader->Compile();
	}

	if (nullptr == myModel)
	{
		myModel = std::make_shared<PBRViewerScene>(pathToModel);
		if(GL_FALSE == myModel->IsReady())
		{
			PBRViewerLogger::PrintErrorMessage(__FILE__, __LINE__, 
				"The default light model could not be loaded. Did you unpack the resources_compressed folder in project root? The application might not work correctly in this case.");
		}
	}	
}

/// <summary>
/// Gets a flag indicating if the light is currently active.
/// </summary>
/// <returns>True if the light is switched on, false if not.</returns>
GLboolean PBRViewerPointLight::GetIsActive() const
{
	return myIsActive;
}

/// <summary>
/// Sets the light as active or inactive.
/// </summary>
/// <param name="newValue">True if the light is switched on, false if not.</param>	
GLvoid PBRViewerPointLight::SetIsActive( const GLboolean newValue )
{
	myIsActive = newValue;
}

/// <summary>
/// Gets the position of the light source.
/// </summary>
/// <returns>A three dimensional vector representing the position of the light source in world coordinates.</returns>
glm::vec3 PBRViewerPointLight::GetPosition() const
{
	return myPosition;
}

/// <summary>
/// Sets the position of the light source.
/// </summary>
/// <param name="newPosition">A three dimensional vector representing the position of the light source in world coordinates.</param>	
GLvoid PBRViewerPointLight::SetPosition( const glm::vec3 newPosition )
{
	myPosition = newPosition;
}

/// <summary>
/// Gets the color of the light.
/// </summary>
/// <returns>A three dimensional vector representing the color of the light as RGB values. The values are NOT clamped between [0, 1].</returns>
glm::vec3 PBRViewerPointLight::GetLightColor() const
{
	return myLightColor;
}

/// <summary>
/// Sets the color of the light.
/// </summary>
/// <param name="newColor">
/// A three dimensional vector representing the color of the light as RGB values. 
/// The values do not have to be clamped between [0, 1].
/// Higher values equal higher intensity.
/// </param>	
GLvoid PBRViewerPointLight::SetLightColor( const glm::vec3 newColor )
{
	myLightColor = newColor;
}

/// <summary>
/// Draws the light source.
/// </summary>
/// <param name="modelMatrix">The model matrix used to draw the light.</param>
/// <param name="viewMatrix">The view matrix used to draw the light.</param>
/// <param name="projectionMatrix">The projection matrix used to draw the light.</param>	
GLvoid PBRViewerPointLight::Draw( const glm::mat4 modelMatrix,
                                  const glm::mat4 viewMatrix,
                                  const glm::mat4 projectionMatrix ) const
{
	if (GL_FALSE == myIsActive)
	{
		return;
	}

	myShader->Use();

	myShader->setMat4("model", modelMatrix);
	myShader->setMat4("view", viewMatrix);
	myShader->setMat4("projection", projectionMatrix);

	myShader->setVec3("lightColor", myLightColor);

	myModel->Draw(myShader);
}
