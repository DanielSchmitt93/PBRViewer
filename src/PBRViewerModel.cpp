# include "PBRViewerModel.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "PBRViewerOpenGLUtilities.h"
#include "PBRViewerLogger.h"
#include <stb_image.h>

GLvoid PBRViewerModel::CreateShader()
{
	// Read needed files
	myNoLightingShader = std::make_shared<PBRViewerShader>("NoLighting.vert", "NoLighting.frag");
	myBlinnPhongShader = std::make_shared<PBRViewerShader>("CommonVertexShader.vert", "BlinnPhong.frag");
	myPbrCookTorranceShader = std::make_shared<PBRViewerShader>("CommonVertexShader.vert", "CookTorrance.frag");
	myOrenNayarShader = std::make_shared<PBRViewerShader>("CommonVertexShader.vert", "OrenNayar.frag");
	myAshikhminShirleyShader = std::make_shared<PBRViewerShader>("CommonVertexShader.vert", "AshikhminShirley.frag");
	mySkyboxShader = std::make_shared<PBRViewerShader>("Skybox.vert", "Skybox.frag");
	myDebugNormalVectorShader = std::make_shared<PBRViewerShader>("NormalVector.vert", "NormalVector.frag", "NormalVector.geom");
	myDebugShader = std::make_shared<PBRViewerShader>("CommonVertexShader.vert", "Debug.frag");
	myDisneyShader = std::make_shared<PBRViewerShader>("CommonVertexShader.vert", "DisneyBRDF.frag");

	myLightingShaders = {myBlinnPhongShader, myPbrCookTorranceShader, myOrenNayarShader, myAshikhminShirleyShader, myDisneyShader};
	myPBRShaders = {myPbrCookTorranceShader, myOrenNayarShader, myAshikhminShirleyShader, myDisneyShader};
	myShaders =
	{
		myNoLightingShader, myBlinnPhongShader, myPbrCookTorranceShader, myOrenNayarShader, myAshikhminShirleyShader,
		mySkyboxShader, myDebugNormalVectorShader, myDebugShader, myDisneyShader
	};

	// Append common code implementations to lighting shaders
	for (const auto& lightingShader : myLightingShaders)
	{
		lightingShader->AddFileAtTheEnd(GL_FRAGMENT_SHADER, "GetNormalFromMap.gl");
		lightingShader->AddFileAtTheEnd(GL_FRAGMENT_SHADER, "CalculateShadowCoverage.gl");
		lightingShader->AddFileAtTheEnd(GL_FRAGMENT_SHADER, "ChooseRenderOutput.gl");
		lightingShader->AddFileAtTheEnd(GL_FRAGMENT_SHADER, "VectorTransformation.gl");
		lightingShader->AddFileAtTheEnd(GL_FRAGMENT_SHADER, "FresnelApproximations.gl");
	}

	// Append common code implementations to PBR shaders
	for (const auto& pbrShader : myPBRShaders)
	{		
		pbrShader->AddFileAtTheEnd(GL_FRAGMENT_SHADER, "NormalDistributionFunctions.gl");
		pbrShader->AddFileAtTheEnd(GL_FRAGMENT_SHADER, "GeometryFunctions.gl");
	}

	// Append common code implementations to single shaders
	myDebugShader->AddFileAtTheEnd(GL_FRAGMENT_SHADER, "GetNormalFromMap.gl");

	// Compile shaders afterwards
	for (const auto& shader : myShaders)
	{
		shader->Compile();
	}
}

GLvoid PBRViewerModel::CreateLightSources()
{
	const GLuint numberOfLights = 4;
	myLightSources.reserve(numberOfLights);

	// All lights share the same color.
	const glm::vec3 lightColor = glm::vec3(5.0f);

	PBRViewerPointLight firstLight;
	firstLight.SetPosition(glm::vec3(-1.0f, 1.0f, 1.0f));
	firstLight.SetIsActive(GL_FALSE);
	firstLight.SetLightColor(lightColor);

	PBRViewerPointLight secondLight;
	secondLight.SetPosition(glm::vec3(1.0f, 1.0f, 1.0f));
	secondLight.SetIsActive(GL_FALSE);
	secondLight.SetLightColor(lightColor);

	PBRViewerPointLight thirdLight;
	thirdLight.SetPosition(glm::vec3(-1.0f, 1.0f, -1.0f));
	thirdLight.SetIsActive(GL_FALSE);
	thirdLight.SetLightColor(lightColor);

	PBRViewerPointLight fourthLight;
	fourthLight.SetPosition(glm::vec3(1.0f, 1.0f, -1.0f));
	fourthLight.SetIsActive(GL_FALSE);
	fourthLight.SetLightColor(lightColor);

	myLightSources.push_back(firstLight);
	myLightSources.push_back(secondLight);
	myLightSources.push_back(thirdLight);
	myLightSources.push_back(fourthLight);
}

GLvoid PBRViewerModel::RemoveShadowTexturesFromModel()
{
	for (const PBRViewerTexture& texture : myShadowDepthCubeMaps)
	{
		myLoadedModel->RemoveTextureFromAllMeshes(texture);
	}
}

GLboolean PBRViewerModel::CreateGlfwWindow()
{
	if (GLFW_FALSE == glfwInit())
	{
		PBRViewerLogger::PrintErrorMessage(__FILE__, __LINE__, "Could not initialize GLFW");
		return GL_FALSE;
	}

	glfwSetTime(0.0);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	// Enable MSAA with four samples per fragment instead of one to prevent aliasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#if _DEBUG
	// Get a debug context from GLFW for better error messages than glGetError()
	// This call is responsible for the required OpenGL version of 4.3
	// See https://learnopengl.com/In-Practice/Debugging
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	// Create a GLFWwindow object
	myWindowContext = glfwCreateWindow(InitialWindowWidth, InitialWindowHeight, "PBRViewer", nullptr, nullptr);
	if (myWindowContext == nullptr)
	{
		PBRViewerLogger::PrintErrorMessage(__FILE__, __LINE__, "Failed to create GLFW window");
		glfwTerminate();
		return GL_FALSE;
	}

	glfwMakeContextCurrent(myWindowContext);

	// Set a custom window pointer to access instance variables within the GLFW callback functions.
	glfwSetWindowUserPointer(myWindowContext, reinterpret_cast<GLvoid*>(this));
	glfwSetScrollCallback(myWindowContext, []( GLFWwindow* window, const GLdouble, const GLdouble yOffset )
	{
		auto* instance = reinterpret_cast<PBRViewerModel*>(glfwGetWindowUserPointer(window));
		instance->myCamera->ProcessMouseScroll(yOffset);
	});

	// Set application icon in window bar with GLFW	
	GLFWimage images[1];
	images[0].pixels = stbi_load("icon.png", &images[0].width, &images[0].height, nullptr, 4);
	glfwSetWindowIcon(myWindowContext, 1, images);
	stbi_image_free(images[0].pixels);

	return GL_TRUE;
}

/// <summary>
/// Init the OpenGL window and setup everything for rendering.
/// Call this method before using the PBRViewerModel.
/// </summary>
/// <returns>True if the initialization was successful, false if not.</returns>
GLboolean PBRViewerModel::Init()
{
	if (GL_FALSE == CreateGlfwWindow())
	{
		return GL_FALSE;
	}

	if (GL_FALSE == gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		throw std::runtime_error("Could not initialize GLAD!");
	}

#if _DEBUG
	// Check if we got a debug context
	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(PBRViewerLogger::PrintOpenGLDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	// Only show messages with a high priority
	glDebugMessageControl(GL_DEBUG_SOURCE_API,
	                      GL_DEBUG_TYPE_ERROR,
	                      GL_DEBUG_SEVERITY_HIGH,
	                      0, nullptr, GL_TRUE);
#endif

	glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	CreateLightSources();
	CreateShader();

	myCamera = std::make_shared<PBRViewerArcballCamera>(glm::vec3(0.0f, 0.0f, 3.0f));

	myLastXPositionMouse = InitialWindowWidth * 0.5f;
	myLastYPositionMouse = InitialWindowHeight * 0.5f;
	myNewPressedMouseCmd = GL_TRUE;

	GLint width, height;
	glfwGetWindowSize(myWindowContext, &width, &height);
	glViewport(0, 0, width, height);
	glfwSwapInterval(0);
	glfwSwapBuffers(myWindowContext);

	return GL_TRUE;
}

GLvoid PBRViewerModel::SetFrameTime()
{
	const GLdouble myCurrentTime = glfwGetTime();
	myDeltaTime = myCurrentTime - myLastTime;
	myLastTime = myCurrentTime;
}

/// <summary>
/// Draws the OpenGL context.
/// This method will be called by the controller within the render loop.
/// It renders the actual OpenGL model with the defined lighting setup.
/// </summary>
GLvoid PBRViewerModel::DrawOpenGL()
{
	SetFrameTime();

	// Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
	glfwPollEvents();

	if (myNewModelShouldBeLoaded)
	{
		if (myLoadedModel)
		{
			myLoadedModel->Cleanup();
			myLoadedModel.reset();
		}

		myLoadedModel = std::make_shared<PBRViewerScene>(myNewModelFilepath);

		if (mySkybox)
		{
			// Set IBL textures for ambient lighting within PBR shader if a model is already available.
			myLoadedModel->AddTextureToAllMeshes(mySkybox->GetIrradianceTexture());
			myLoadedModel->AddTextureToAllMeshes(mySkybox->GetPreFilteredEnvironmentMap());
			myLoadedModel->AddTextureToAllMeshes(mySkybox->GetBRDFLookupTexture());
		}

		if (myShadows)
		{
			myShadows->Cleanup();
			myShadows.reset();
		}

		// Generate shadow textures for the new model
		myShadows = std::make_unique<PBRViewerShadows>(myLoadedModel);

		std::vector<PBRViewerTexture> shadowTextures = myShadows->CreateSelfShadowingTextures(static_cast<GLuint>(myLightSources.size()));
		for (const PBRViewerTexture& texture : shadowTextures)
		{
			myLoadedModel->AddTextureToAllMeshes(texture);
		}

		myNewModelShouldBeLoaded = GL_FALSE;
	}

	if (myNewSkyboxShouldBeLoaded)
	{
		if (mySkybox)
		{
			mySkybox->Cleanup();
			mySkybox.reset();
		}

		mySkybox = std::make_unique<PBRViewerSkybox>(myNewSkyboxFilepath);
		if (GL_FALSE == mySkybox->Init())
		{
			mySkybox->Cleanup();
			mySkybox.reset();
			myNewSkyboxShouldBeLoaded = GL_FALSE;
			return;
		}

		if (myLoadedModel)
		{
			// Set IBL textures for ambient lighting within PBR shader if a model is already available.
			myLoadedModel->AddTextureToAllMeshes(mySkybox->GetIrradianceTexture());
			myLoadedModel->AddTextureToAllMeshes(mySkybox->GetPreFilteredEnvironmentMap());
			myLoadedModel->AddTextureToAllMeshes(mySkybox->GetBRDFLookupTexture());
		}

		myNewSkyboxShouldBeLoaded = GL_FALSE;
	}

	GLint currentWindowWidth, currentWindowHeight;
	glfwGetWindowSize(myWindowContext, &currentWindowWidth, &currentWindowHeight);

	// Prevent divide-by-zero error within glm (aspect ratio).
	if (currentWindowHeight == 0)
	{
		currentWindowHeight = 1;
	}

	const glm::mat4 projection = glm::perspective(glm::radians(myCamera->GetZoom()),
	                                              static_cast<GLfloat>(currentWindowWidth) /
	                                              static_cast<GLfloat>(currentWindowHeight),
	                                              0.1f,
	                                              50.0f);
	const glm::mat4 view = myCamera->GetViewMatrix();

	DrawLightSources(view, projection);

	if (myLoadedModel)
	{
		// Calculate self shadowing	
		if (myAreShadowsEnabled)
		{
			myShadows->CalculateSelfShadowing(currentWindowWidth, currentWindowHeight, myLightSources);
		}

		SetLightingShader();
		DrawModel(view, projection);
		RemoveShadowTexturesFromModel();
	}

	// Draw the skybox last
	if (mySkybox)
	{
		DrawSkybox(projection);
	}
}

/// <summary>
/// Gets the current GLFW window context.
/// </summary>
/// <returns>The current GLFW window context.</returns>
GLFWwindow* PBRViewerModel::GetWindowContext() const
{
	return myWindowContext;
}

/// <summary>
/// Gets the current lighting variant.
/// </summary>
/// <returns>The current lighting variant.</returns>
PBRViewerEnumerations::LightingVariant PBRViewerModel::GetCurrentLightingVariant() const
{
	return myLightingVariant;
}

GLvoid PBRViewerModel::DrawSkybox( const glm::mat4 projection ) const
{
	mySkyboxShader->Use();

	// Remove translation from the view matrix
	const glm::mat4 viewMatrixSkybox = glm::mat4(glm::mat3(myCamera->GetViewMatrix()));
	mySkyboxShader->setMat4("view", viewMatrixSkybox);
	mySkyboxShader->setMat4("projection", projection);
	mySkyboxShader->setFloat("gamma", myGamma);
	mySkyboxShader->setFloat("exposure", myExposure);

	mySkybox->Draw(mySkyboxShader);
}

GLvoid PBRViewerModel::DrawLightSources( const glm::mat4 view, const glm::mat4 projection ) const
{
	for (const PBRViewerPointLight& light : myLightSources)
	{
		glm::mat4 model = glm::identity<glm::mat4>();
		model = translate(model, light.GetPosition());

		// =================================================================================================
		// The translation and scaling values are valid for the default light 3D model.
		// The translation ensures that the position of the point light source is in the center of the bulb of the 3D model.
		// If you need to change the 3D model, please keep this section in mind and change it, if necessary.
		// =================================================================================================
		model = translate(model, glm::vec3(0.0f, 0.5f, 0.0f));

		light.Draw(model, view, projection);
	}
}

GLvoid PBRViewerModel::DrawModel( const glm::mat4 view, const glm::mat4 projection ) const
{
	myCurrentLightShader->Use();
	myCurrentLightShader->setMat4("model", myLoadedModel->GetModelMatrix());
	myCurrentLightShader->setMat4("view", view);
	myCurrentLightShader->setMat4("projection", projection);
	myCurrentLightShader->setVec3("camPos", myCamera->GetCameraPosition());

	for (GLuint i = 0; i < myLightSources.size(); ++i)
	{
		myCurrentLightShader->setVec3("lightPositions[" + std::to_string(i) + "]", myLightSources[i].GetPosition());
		myCurrentLightShader->setVec3("lightColors[" + std::to_string(i) + "]", myLightSources[i].GetLightColor());
		myCurrentLightShader->setBool("isLightActive[" + std::to_string(i) + "]", myLightSources[i].GetIsActive());

		if (myLightSources[i].GetIsActive())
		{
			// Light space matrices for shadow calculation.
			glm::mat4 lightSpaceMatrix = myShadows->GetShadowProjectionMatrix() * lookAt(myLightSources[i].GetPosition(),
			                                                                             glm::vec3(0.0f),
			                                                                             glm::vec3(0.0f, 1.0f, 0.0f));

			myCurrentLightShader->setMat4("lightSpaceMatrices[" + std::to_string(i) + "]", lightSpaceMatrix);
		}
	}

	// Blinn-Phong
	myCurrentLightShader->setInt("blinnPhongExponent", myBlinnPhongExponent);

	// Ashikhmin-Shirley
	myCurrentLightShader->setInt("n_u", myAshikhminShirleyNu);
	myCurrentLightShader->setInt("n_v", myAshikhminShirleyNv);

	// Cook-Torrance
	myCurrentLightShader->setInt("diffuseTerm", myCookTorranceDiffuseTerm);
	myCurrentLightShader->setInt("fresnelTerm", myCookTorranceFresnelTerm);
	myCurrentLightShader->setInt("normalDistributionTerm", myCookTorranceNormalDistributionTerm);
	myCurrentLightShader->setInt("geometryTerm", myCookTorranceGeometryTerm);

	if (myCookTorranceAreCustomMaterialValuesEnabled)
	{
		myCurrentLightShader->setBool("customMaterialValuesEnabled", GL_TRUE);
		myCurrentLightShader->setFloat("customMetalness", myCookTorranceMetalness);
		myCurrentLightShader->setFloat("customRoughness", myCookTorranceRoughness);
	}
	else
	{
		myCurrentLightShader->setBool("customMaterialValuesEnabled", GL_FALSE);
	}

	// Disney
	myCurrentLightShader->setFloat("subsurface", myDisneySubsurface);
	myCurrentLightShader->setFloat("metallic", myDisneyMetallic);
	myCurrentLightShader->setFloat("specular", myDisneySpecular);
	myCurrentLightShader->setFloat("specularTint", myDisneySpecularTint);
	myCurrentLightShader->setFloat("roughness", myDisneyRoughness);
	myCurrentLightShader->setFloat("anisotropic", myDisneyAnisotropic);
	myCurrentLightShader->setFloat("sheen", myDisneySheen);
	myCurrentLightShader->setFloat("sheenTint", myDisneySheenTint);
	myCurrentLightShader->setFloat("clearcoat", myDisneyClearcoat);
	myCurrentLightShader->setFloat("clearcoatGloss", myDisneyClearcoatGloss);

	// General settings
	myCurrentLightShader->setBool("shadowsEnabled", myAreShadowsEnabled);

	myCurrentLightShader->setInt("renderOutput", myRenderOutput);
	myCurrentLightShader->setInt("debugOutput", myDebugOutput);

	myCurrentLightShader->setFloat("gamma", myGamma);
	myCurrentLightShader->setFloat("exposure", myExposure);

	myLoadedModel->Draw(myCurrentLightShader);
}

GLvoid PBRViewerModel::SetLightingShader()
{
	switch (myLightingVariant)
	{
		case PBRViewerEnumerations::LightingVariant::NoLighting:
			myCurrentLightShader = myNoLightingShader;
			break;
		case PBRViewerEnumerations::LightingVariant::BlinnPhong:
			myCurrentLightShader = myBlinnPhongShader;
			break;
		case PBRViewerEnumerations::LightingVariant::CookTorrance:
			myCurrentLightShader = myPbrCookTorranceShader;
			break;
		case PBRViewerEnumerations::LightingVariant::OrenNayar:
			myCurrentLightShader = myOrenNayarShader;
			break;
		case PBRViewerEnumerations::LightingVariant::AshikhminShirley:
			myCurrentLightShader = myAshikhminShirleyShader;
			break;
		case PBRViewerEnumerations::LightingVariant::Debug:
			myCurrentLightShader = myDebugShader;
			break;
		case PBRViewerEnumerations::LightingVariant::Disney:
			myCurrentLightShader = myDisneyShader;
			break;
		default:
			myCurrentLightShader = myNoLightingShader;
			break;
	}
}

/// <summary>
/// Load a new model from the specified filepath.
/// </summary>
/// <param name="filepathNewModel">The filepath of the new model.</param>
GLvoid PBRViewerModel::LoadNewModel( const std::string& filepathNewModel )
{
	myNewModelFilepath = filepathNewModel;
	myNewModelShouldBeLoaded = GL_TRUE;

	// Reset transformations in case a model was loaded beforehand.
	ResetModelTransformations();
}

/// <summary>
/// Clears the model.
/// </summary>
GLvoid PBRViewerModel::ClearModel()
{
	if (mySkybox)
	{
		myLoadedModel->RemoveTextureFromAllMeshes(mySkybox->GetIrradianceTexture());
	}

	myLoadedModel.reset();
}

/// <summary>
/// Loads a new skybox from the specified filepath.
/// </summary>
/// <param name="filepathNewSkybox">The filepath of the new skybox.</param>
GLvoid PBRViewerModel::LoadNewSkybox( const std::string& filepathNewSkybox )
{
	myNewSkyboxFilepath = filepathNewSkybox;
	myNewSkyboxShouldBeLoaded = GL_TRUE;
}

/// <summary>
/// Clears the skybox.
/// </summary>
GLvoid PBRViewerModel::ClearSkybox()
{
	if (myLoadedModel)
	{
		myLoadedModel->RemoveTextureFromAllMeshes(mySkybox->GetIrradianceTexture());
		myLoadedModel->RemoveTextureFromAllMeshes(mySkybox->GetPreFilteredEnvironmentMap());
		myLoadedModel->RemoveTextureFromAllMeshes(mySkybox->GetBRDFLookupTexture());
	}

	mySkybox->Cleanup();
	mySkybox.reset();
}

/// <summary>
/// Changes the lighting variant (e.g. Blinn/Phong, Cook-Torrance, Debug or no lighting at all).
/// </summary>
/// <param name="lightingVariant">The lighting variant.</param>
GLvoid PBRViewerModel::ChangeLighting( const PBRViewerEnumerations::LightingVariant lightingVariant )
{
	myLightingVariant = lightingVariant;
}

/// <summary>
/// Changes the normal distribution term.
/// </summary>
/// <param name="normalDistributionTerm">The normal distribution term.</param>
GLvoid PBRViewerModel::ChangeNormalDistributionTerm(
	const PBRViewerEnumerations::NormalDistributionTerm normalDistributionTerm )
{
	myCookTorranceNormalDistributionTerm = normalDistributionTerm;
}

/// <summary>
/// Sets the color of the lighting.
/// </summary>
/// <param name="rgb">The RGB.</param>
GLvoid PBRViewerModel::SetLightingColor( glm::vec3 rgb )
{
	for (PBRViewerPointLight& light : myLightSources)
	{
		// Multiply by five because our light strength equals five and not one.
		light.SetLightColor(rgb.operator*=(5));
	}
}

/// <summary>
/// Changes the render output.
/// </summary>
/// <param name="renderOutput">The render output.</param>
GLvoid PBRViewerModel::ChangeRenderOutput( const PBRViewerEnumerations::RenderOutput renderOutput )
{
	myRenderOutput = renderOutput;
}

/// <summary>
/// Sets the mouse processing.
/// </summary>
/// <param name="mouseShouldBeProcessed">True if mouse interactions should be processed, false if not.</param>
GLvoid PBRViewerModel::SetMouseProcessing( const GLboolean mouseShouldBeProcessed )
{
	myMouseShouldBeProcessed = mouseShouldBeProcessed;
}

/// <summary>
/// Gets a flag indicating if the mouse should be processed.
/// For example, if the mouse is over an UI element it should not be processed.
/// </summary>
/// <returns>True if the mouse should be processed, false if not.</returns>
GLboolean PBRViewerModel::GetMouseProcessing() const
{
	return myMouseShouldBeProcessed;
}

/// <summary>
/// Rotates the model.
/// </summary>
/// <param name="rotationMatrix">The rotation matrix.</param>	
GLvoid PBRViewerModel::RotateModel( const glm::mat4 rotationMatrix ) const
{
	if (nullptr == myLoadedModel || rotationMatrix == glm::identity<glm::mat4>())
	{
		return;
	}

	myLoadedModel->RotateModel(rotationMatrix);
}

/// <summary>
/// Rotates the model incremental around the x-axis.
/// </summary>
/// <param name="rotationAroundX">The rotation around the x-axis in degrees.</param>
GLvoid PBRViewerModel::RotateModelX( const GLfloat rotationAroundX ) const
{
	if (nullptr == myLoadedModel || rotationAroundX == 0.0f)
	{
		return;
	}

	myLoadedModel->RotateModelX(rotationAroundX);
}

/// <summary>
/// Rotates the model incremental around the y-axis.
/// </summary>
/// <param name="rotationAroundY">The rotation around the y-axis in degrees.</param>
GLvoid PBRViewerModel::RotateModelY( const GLfloat rotationAroundY ) const
{
	if (nullptr == myLoadedModel || rotationAroundY == 0.0f)
	{
		return;
	}

	myLoadedModel->RotateModelY(rotationAroundY);
}

/// <summary>
/// Rotates the model incremental around the z-axis.
/// </summary>
/// <param name="rotationAroundZ">The rotation around the z-axis in degrees.</param>
GLvoid PBRViewerModel::RotateModelZ( const GLfloat rotationAroundZ ) const
{
	if (nullptr == myLoadedModel || rotationAroundZ == 0.0f)
	{
		return;
	}

	myLoadedModel->RotateModelZ(rotationAroundZ);
}

/// <summary>
/// Sets the position of the loaded model.
/// </summary>
/// <param name="newPosition">The new position.</param>	
GLvoid PBRViewerModel::SetPosition( const glm::vec3 newPosition ) const
{
	if (nullptr == myLoadedModel || newPosition == glm::vec3(0.0f))
	{
		return;
	}

	myLoadedModel->SetPosition(newPosition);
}

/// <summary>
/// Sets the model matrix of the loaded 3D model.
/// </summary>
/// <param name="newModelMatrix">The new model matrix.</param>	
GLvoid PBRViewerModel::SetModelMatrix( const glm::mat4 newModelMatrix ) const
{
	if (nullptr == myLoadedModel)
	{
		return;
	}

	myLoadedModel->SetModelMatrix(newModelMatrix);
}

/// <summary>
/// Gets the current camera instance.
/// </summary>
/// <returns>A weak_ptr of the current camera instance.</returns>
std::weak_ptr<PBRViewerArcballCamera> PBRViewerModel::GetCamera() const
{
	return std::weak_ptr<PBRViewerArcballCamera>(myCamera);
}

/// <summary>
/// Gets the current scene.
/// </summary>
/// <returns>A weak_ptr of the current scene.</returns>
std::weak_ptr<PBRViewerScene> PBRViewerModel::GetScene() const
{
	return std::weak_ptr<PBRViewerScene>(myLoadedModel);
}

/// <summary>
/// Scales the model incrementally.
/// </summary>
/// <param name="value">The value to scale.</param>
/// <param name="scaleOperation">Either increase or decrease the model.</param>
GLvoid PBRViewerModel::ScaleIsotropically( const GLfloat value,
                                           const PBRViewerEnumerations::ScaleOperation scaleOperation ) const
{
	if (nullptr == myLoadedModel || 0.0f == value)
	{
		return;
	}

	myLoadedModel->ScaleIsotropically(value, scaleOperation);
}

/// <summary>
/// Resets the model transformations.
/// </summary>
GLvoid PBRViewerModel::ResetModelTransformations() const
{
	if (nullptr == myLoadedModel)
	{
		return;
	}

	myLoadedModel->SetModelMatrix(glm::identity<glm::mat4>());
}

/// <summary>
/// Activates a light source.
/// </summary>
/// <param name="number">The number of the light source to activate.</param>	
GLvoid PBRViewerModel::ActivateLightSource( const GLint number )
{
	myLightSources[number].SetIsActive(GL_TRUE);
}

/// <summary>
/// Deactivates a light source.
/// </summary>
/// <param name="number">The number of the light source to deactivate.</param>	
GLvoid PBRViewerModel::DisableLightSource( const GLint number )
{
	myLightSources[number].SetIsActive(GL_FALSE);
}

/// <summary>
/// Sets the value used for the gamma correction.
/// </summary>
/// <param name="value">The gamma value.</param>
GLvoid PBRViewerModel::SetGamma( const GLfloat value )
{
	myGamma = value;
}

/// <summary>
/// Sets the exposure value used for the tone mapping algorithm.
/// </summary>
/// <param name="value">The exposure value.</param>	
GLvoid PBRViewerModel::SetExposure( const GLfloat value )
{
	myExposure = value;
}

/// <summary>
/// Sets the custom metalness value for the object.
/// </summary>
/// <param name="value">The user-defined metalness value.</param>
GLvoid PBRViewerModel::SetCustomMetalness( const GLfloat value )
{
	myCookTorranceMetalness = value;
}

/// <summary>
/// Sets the custom roughness value for the object.
/// </summary>
/// <param name="value">The user-defined roughness value.</param>
GLvoid PBRViewerModel::SetCustomRoughness( const GLfloat value )
{
	myCookTorranceRoughness = value;
}

/// <summary>
/// Sets a flag indicating if user-defined material values should be used.
/// </summary>
/// <param name="activated">The value of the flag.</param>	
GLvoid PBRViewerModel::SetEnableCustomMaterialValues( const GLboolean activated )
{
	myCookTorranceAreCustomMaterialValuesEnabled = activated;
}

/// <summary>
/// Sets the texture to display around the loaded object.
/// </summary>
/// <param name="currentSkyboxTexture">The skybox texture to display.</param>	
GLvoid PBRViewerModel::SetSkyboxTexture( const PBRViewerEnumerations::SkyboxTexture currentSkyboxTexture ) const
{
	if (nullptr == mySkybox)
	{
		return;
	}

	mySkybox->SetTextureToDisplay(currentSkyboxTexture);
}

/// <summary>
/// Sets the skybox texture mipmap level.
/// </summary>
/// <param name="currentMipMapLevel">The mipmap level to display.</param>	
GLvoid PBRViewerModel::SetSkyboxTextureMipMapLevel( const GLuint currentMipMapLevel ) const
{
	if (nullptr == mySkybox)
	{
		return;
	}

	mySkybox->SetTextureToDisplayMipMapLevel(currentMipMapLevel);
}

/// <summary>
/// Sets the exponent for the Blinn/Phong algorithm.
/// </summary>
/// <param name="currentExponent">The value of the exponent.</param>
GLvoid PBRViewerModel::SetBlinnPhongExponent( const GLuint currentExponent )
{
	myBlinnPhongExponent = currentExponent;
}

/// <summary>
/// Sets a flag indicating if shadows should be used.
/// </summary>
/// <param name="activated">The value of the flag.</param>	
GLvoid PBRViewerModel::SetEnableShadows( const GLboolean activated )
{
	myAreShadowsEnabled = activated;
}

/// <summary>
/// Sets the output of the debug shader (note that the normal render output has no influence on the debug shader).
/// </summary>
/// <param name="currentDebugOutput">The output of the debug shader.</param>
GLvoid PBRViewerModel::SetDebugOutput( const PBRViewerEnumerations::DebugOutput currentDebugOutput )
{
	myDebugOutput = currentDebugOutput;
}

/// <summary>
/// Sets the diffuse term of a lighting model.
/// </summary>
/// <param name="currentDiffuseTerm">The diffuse term to use.</param>
GLvoid PBRViewerModel::SetDiffuseTerm( const PBRViewerEnumerations::DiffuseTerm currentDiffuseTerm )
{
	myCookTorranceDiffuseTerm = currentDiffuseTerm;
}

/// <summary>
/// Sets the n_u parameter of the Ashikhmin-Shirley BRDF.
/// </summary>
/// <param name="currentNu">The value of n_u.</param>	
GLvoid PBRViewerModel::SetAshikhminShirleyNu( const GLuint currentNu )
{
	myAshikhminShirleyNu = currentNu;
}

/// <summary>
/// Sets the n_v parameter of the Ashikhmin-Shirley BRDF.
/// </summary>
/// <param name="currentNv">The value of n_v.</param>
GLvoid PBRViewerModel::SetAshikhminShirleyNv( const GLuint currentNv )
{
	myAshikhminShirleyNv = currentNv;
}

/// <summary>
/// Sets the subsurface parameter of the Disney BRDF.
/// </summary>
/// <param name="value">The value.</param>	
GLvoid PBRViewerModel::SetDisneySubsurface( const GLfloat value )
{
	myDisneySubsurface = value;
}

/// <summary>
/// Sets the metallic parameter of the Disney BRDF.
/// </summary>
/// <param name="value">The value.</param>	
GLvoid PBRViewerModel::SetDisneyMetallic( const GLfloat value )
{
	myDisneyMetallic = value;
}

/// <summary>
/// Sets the specular parameter of the Disney BRDF.
/// </summary>
/// <param name="value">The value.</param>	
GLvoid PBRViewerModel::SetDisneySpecular( const GLfloat value )
{
	myDisneySpecular = value;
}

/// <summary>
/// Sets the specularTint parameter of the Disney BRDF.
/// </summary>
/// <param name="value">The value.</param>	
GLvoid PBRViewerModel::SetDisneySpecularTint( const GLfloat value )
{
	myDisneySpecularTint = value;
}

/// <summary>
/// Sets the roughness parameter of the Disney BRDF.
/// </summary>
/// <param name="value">The value.</param>	
GLvoid PBRViewerModel::SetDisneyRoughness( const GLfloat value )
{
	myDisneyRoughness = value;
}

/// <summary>
/// Sets the anisotropic parameter of the Disney BRDF.
/// </summary>
/// <param name="value">The value.</param>	
GLvoid PBRViewerModel::SetDisneyAnisotropic( const GLfloat value )
{
	myDisneyAnisotropic = value;
}

/// <summary>
/// Sets the sheen parameter of the Disney BRDF.
/// </summary>
/// <param name="value">The value.</param>	
GLvoid PBRViewerModel::SetDisneySheen( const GLfloat value )
{
	myDisneySheen = value;
}

/// <summary>
/// Sets the sheenTint parameter of the Disney BRDF.
/// </summary>
/// <param name="value">The value.</param>	
GLvoid PBRViewerModel::SetDisneySheenTint( const GLfloat value )
{
	myDisneySheenTint = value;
}

/// <summary>
/// Sets the clearcoat parameter of the Disney BRDF.
/// </summary>
/// <param name="value">The value.</param>	
GLvoid PBRViewerModel::SetDisneyClearcoat( const GLfloat value )
{
	myDisneyClearcoat = value;
}

/// <summary>
/// Sets the clearcoatGloss parameter of the Disney BRDF.
/// </summary>
/// <param name="value">The value.</param>	
GLvoid PBRViewerModel::SetDisneyClearcoatGloss( const GLfloat value )
{
	myDisneyClearcoatGloss = value;
}

/// <summary>
/// Disposes internal instances and frees memory.
/// </summary>
GLvoid PBRViewerModel::Cleanup() const
{
	if (myShadows)
	{
		myShadows->Cleanup();
	}

	if (myLoadedModel)
	{
		myLoadedModel->Cleanup();
	}

	if (mySkybox)
	{
		mySkybox->Cleanup();
	}
}

/// <summary>
/// Changes the fresnel term.
/// </summary>
/// <param name="fresnelTerm">The fresnel term.</param>
GLvoid PBRViewerModel::ChangeFresnelTerm( const PBRViewerEnumerations::FresnelTerm fresnelTerm )
{
	myCookTorranceFresnelTerm = fresnelTerm;
}

/// <summary>
/// Changes the geometry term.
/// </summary>
/// <param name="geometryTerm">The fresnel term.</param>
GLvoid PBRViewerModel::ChangeGeometryTerm( const PBRViewerEnumerations::GeometryTerm geometryTerm )
{
	myCookTorranceGeometryTerm = geometryTerm;
}
