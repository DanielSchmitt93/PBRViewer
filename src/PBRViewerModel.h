#pragma once

#include <GLFW/glfw3.h>

#include "PBRViewerEnumerations.h"

#include "PBRViewerShader.h"
#include "PBRViewerScene.h"

#include "PBRViewerArcballCamera.h"
#include "PBRViewerSkybox.h"
#include "PBRViewerShadows.h"
#include "PBRViewerPointLight.h"

// This class is the model in the MVC pattern.
// It contains the OpenGL logic and the GLFW window.
class PBRViewerModel
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="PBRViewerModel"/> class.
	/// </summary>
	PBRViewerModel() = default;
	
	/// <summary>
	/// Init the OpenGL window and setup everything for rendering.
	/// Call this method before using the PBRViewerModel.
	/// </summary>
	/// <returns>True if the initialization was successful, false if not.</returns>
	GLboolean Init();
	
	/// <summary>
	/// Draws the OpenGL context.
	/// This method will be called by the controller within the render loop.
	/// It renders the actual OpenGL model with the defined lighting setup.
	/// </summary>
	GLvoid DrawOpenGL();

	/// <summary>
	/// Gets the current GLFW window context.
	/// </summary>
	/// <returns>The current GLFW window context.</returns>
	GLFWwindow* GetWindowContext() const;

	/// <summary>
	/// Gets the current lighting variant.
	/// </summary>
	/// <returns>The current lighting variant.</returns>
	PBRViewerEnumerations::LightingVariant GetCurrentLightingVariant() const;

	/// <summary>
	/// Load a new model from the specified filepath.
	/// </summary>
	/// <param name="filepathNewModel">The filepath of the new model.</param>
	GLvoid LoadNewModel( const std::string& filepathNewModel );

	/// <summary>
	/// Clears the model.
	/// </summary>
	GLvoid ClearModel();

	/// <summary>
	/// Loads a new skybox from the specified filepath.
	/// </summary>
	/// <param name="filepathNewSkybox">The filepath of the new skybox.</param>
	GLvoid LoadNewSkybox( const std::string& filepathNewSkybox );

	/// <summary>
	/// Clears the skybox.
	/// </summary>
	GLvoid ClearSkybox();

	/// <summary>
	/// Changes the lighting variant (e.g. Blinn/Phong, Cook-Torrance, Debug or no lighting at all).
	/// </summary>
	/// <param name="lightingVariant">The lighting variant.</param>
	GLvoid ChangeLighting( PBRViewerEnumerations::LightingVariant lightingVariant );

	/// <summary>
	/// Changes the normal distribution term.
	/// </summary>
	/// <param name="normalDistributionTerm">The normal distribution term.</param>
	GLvoid ChangeNormalDistributionTerm( PBRViewerEnumerations::NormalDistributionTerm normalDistributionTerm );

	/// <summary>
	/// Changes the fresnel term.
	/// </summary>
	/// <param name="fresnelTerm">The fresnel term.</param>
	GLvoid ChangeFresnelTerm( PBRViewerEnumerations::FresnelTerm fresnelTerm );

	/// <summary>
	/// Changes the geometry term.
	/// </summary>
	/// <param name="geometryTerm">The fresnel term.</param>
	GLvoid ChangeGeometryTerm(PBRViewerEnumerations::GeometryTerm geometryTerm);

	/// <summary>
	/// Sets the color of the lighting.
	/// </summary>
	/// <param name="rgb">The RGB.</param>
	GLvoid SetLightingColor( glm::vec3 rgb );

	/// <summary>
	/// Changes the render output.
	/// </summary>
	/// <param name="renderOutput">The render output.</param>
	GLvoid ChangeRenderOutput( PBRViewerEnumerations::RenderOutput renderOutput );

	/// <summary>
	/// Sets the mouse processing.
	/// </summary>
	/// <param name="mouseShouldBeProcessed">True if mouse interactions should be processed, false if not.</param>
	GLvoid SetMouseProcessing( GLboolean mouseShouldBeProcessed );

	/// <summary>
	/// Gets a flag indicating if the mouse should be processed.
	/// For example, if the mouse is over an UI element it should not be processed.
	/// </summary>
	/// <returns>True if the mouse should be processed, false if not.</returns>
	GLboolean GetMouseProcessing() const;
	
	/// <summary>
	/// Rotates the model.
	/// </summary>
	/// <param name="rotationMatrix">The rotation matrix.</param>	
	GLvoid RotateModel(glm::mat4 rotationMatrix) const;

	/// <summary>
	/// Rotates the model incremental around the x-axis.
	/// </summary>
	/// <param name="rotationAroundX">The rotation around the x-axis in degrees.</param>
	GLvoid RotateModelX( GLfloat rotationAroundX ) const;

	/// <summary>
	/// Rotates the model incremental around the y-axis.
	/// </summary>
	/// <param name="rotationAroundY">The rotation around the y-axis in degrees.</param>
	GLvoid RotateModelY( GLfloat rotationAroundY ) const;

	/// <summary>
	/// Rotates the model incremental around the z-axis.
	/// </summary>
	/// <param name="rotationAroundZ">The rotation around the z-axis in degrees.</param>
	GLvoid RotateModelZ( GLfloat rotationAroundZ ) const;

	/// <summary>
	/// Sets the position of the loaded model.
	/// </summary>
	/// <param name="newPosition">The new position.</param>	
	GLvoid SetPosition(glm::vec3 newPosition) const;

	/// <summary>
	/// Sets the model matrix of the loaded 3D model.
	/// </summary>
	/// <param name="newModelMatrix">The new model matrix.</param>	
	GLvoid SetModelMatrix(glm::mat4 newModelMatrix) const;

	/// <summary>
	/// Gets the current camera instance.
	/// </summary>
	/// <returns>A weak_ptr of the current camera instance.</returns>
	std::weak_ptr<PBRViewerArcballCamera> GetCamera() const;

	/// <summary>
	/// Gets the current scene.
	/// </summary>
	/// <returns>A weak_ptr of the current scene.</returns>
	std::weak_ptr<PBRViewerScene> GetScene() const;

	/// <summary>
	/// Scales the model incrementally.
	/// </summary>
	/// <param name="value">The value to scale.</param>
	/// <param name="scaleOperation">Either increase or decrease the model.</param>
	GLvoid ScaleIsotropically( GLfloat value, PBRViewerEnumerations::ScaleOperation scaleOperation  ) const;

	/// <summary>
	/// Resets the model transformations.
	/// </summary>
	GLvoid ResetModelTransformations() const;	

	/// <summary>
	/// Activates a light source.
	/// </summary>
	/// <param name="number">The number of the light source to activate.</param>	
	GLvoid ActivateLightSource(GLint number);

	/// <summary>
	/// Deactivates a light source.
	/// </summary>
	/// <param name="number">The number of the light source to deactivate.</param>	
	GLvoid DisableLightSource( GLint number );

	/// <summary>
	/// Sets the value used for the gamma correction.
	/// </summary>
	/// <param name="value">The gamma value.</param>
	GLvoid SetGamma(GLfloat value);

	/// <summary>
	/// Sets the exposure value used for the tone mapping algorithm.
	/// </summary>
	/// <param name="value">The exposure value.</param>	
	GLvoid SetExposure(GLfloat value);

	/// <summary>
	/// Sets the custom metalness value for the object.
	/// </summary>
	/// <param name="value">The user-defined metalness value.</param>	
	GLvoid SetCustomMetalness(GLfloat value);

	/// <summary>
	/// Sets the custom roughness value for the object.
	/// </summary>
	/// <param name="value">The user-defined roughness value.</param>
	GLvoid SetCustomRoughness(GLfloat value);

	/// <summary>
	/// Sets a flag indicating if user-defined material values should be used.
	/// </summary>
	/// <param name="activated">The value of the flag.</param>	
	GLvoid SetEnableCustomMaterialValues(GLboolean activated);

	/// <summary>
	/// Sets the texture to display around the loaded object.
	/// </summary>
	/// <param name="currentSkyboxTexture">The skybox texture to display.</param>	
	GLvoid SetSkyboxTexture(PBRViewerEnumerations::SkyboxTexture currentSkyboxTexture) const;

	/// <summary>
	/// Sets the skybox texture mipmap level.
	/// </summary>
	/// <param name="currentMipMapLevel">The mipmap level to display.</param>	
	GLvoid SetSkyboxTextureMipMapLevel(GLuint currentMipMapLevel) const;

	/// <summary>
	/// Sets the exponent for the Blinn/Phong algorithm.
	/// </summary>
	/// <param name="currentExponent">The value of the exponent.</param>
	GLvoid SetBlinnPhongExponent(GLuint currentExponent);

	/// <summary>
	/// Sets a flag indicating if shadows should be used.
	/// </summary>
	/// <param name="activated">The value of the flag.</param>	
	GLvoid SetEnableShadows(GLboolean activated);

	/// <summary>
	/// Sets the output of the debug shader (note that the normal render output has no influence on the debug shader).
	/// </summary>
	/// <param name="currentDebugOutput">The output of the debug shader.</param>
	GLvoid SetDebugOutput(PBRViewerEnumerations::DebugOutput currentDebugOutput);

	/// <summary>
	/// Sets the diffuse term of a lighting model.
	/// </summary>
	/// <param name="currentDiffuseTerm">The diffuse term to use.</param>	
	GLvoid SetDiffuseTerm(PBRViewerEnumerations::DiffuseTerm currentDiffuseTerm);

	/// <summary>
	/// Sets the n_u parameter of the Ashikhmin-Shirley BRDF.
	/// </summary>
	/// <param name="currentNu">The value of n_u.</param>	
	GLvoid SetAshikhminShirleyNu(GLuint currentNu);

	/// <summary>
	/// Sets the n_v parameter of the Ashikhmin-Shirley BRDF.
	/// </summary>
	/// <param name="currentNv">The value of n_v.</param>	
	GLvoid SetAshikhminShirleyNv(GLuint currentNv);

	/// <summary>
	/// Sets the subsurface parameter of the Disney BRDF.
	/// </summary>
	/// <param name="value">The value.</param>	
	GLvoid SetDisneySubsurface(GLfloat value);

	/// <summary>
	/// Sets the metallic parameter of the Disney BRDF.
	/// </summary>
	/// <param name="value">The value.</param>	
	GLvoid SetDisneyMetallic(GLfloat value);

	/// <summary>
	/// Sets the specular parameter of the Disney BRDF.
	/// </summary>
	/// <param name="value">The value.</param>	
	GLvoid SetDisneySpecular(GLfloat value);

	/// <summary>
	/// Sets the specularTint parameter of the Disney BRDF.
	/// </summary>
	/// <param name="value">The value.</param>	
	GLvoid SetDisneySpecularTint(GLfloat value);

	/// <summary>
	/// Sets the roughness parameter of the Disney BRDF.
	/// </summary>
	/// <param name="value">The value.</param>	
	GLvoid SetDisneyRoughness(GLfloat value);

	/// <summary>
	/// Sets the anisotropic parameter of the Disney BRDF.
	/// </summary>
	/// <param name="value">The value.</param>	
	GLvoid SetDisneyAnisotropic(GLfloat value);

	/// <summary>
	/// Sets the sheen parameter of the Disney BRDF.
	/// </summary>
	/// <param name="value">The value.</param>	
	GLvoid SetDisneySheen(GLfloat value);

	/// <summary>
	/// Sets the sheenTint parameter of the Disney BRDF.
	/// </summary>
	/// <param name="value">The value.</param>	
	GLvoid SetDisneySheenTint(GLfloat value);

	/// <summary>
	/// Sets the clearcoat parameter of the Disney BRDF.
	/// </summary>
	/// <param name="value">The value.</param>	
	GLvoid SetDisneyClearcoat(GLfloat value);

	/// <summary>
	/// Sets the clearcoatGloss parameter of the Disney BRDF.
	/// </summary>
	/// <param name="value">The value.</param>	
	GLvoid SetDisneyClearcoatGloss(GLfloat value);

	/// <summary>
	/// Disposes internal instances and frees memory.
	/// </summary>
	GLvoid Cleanup() const;

private:	
	GLboolean myMouseShouldBeProcessed = GL_TRUE;

	GLvoid SetFrameTime();
	GLvoid SetLightingShader();

	// Draw components
	GLvoid DrawModel( glm::mat4 view, glm::mat4 projection) const;
	GLvoid DrawLightSources( glm::mat4 view, glm::mat4 projection) const;
	GLvoid DrawSkybox( glm::mat4 projection ) const;

	// Frame time
	GLdouble myDeltaTime = 0.0;
	GLdouble myLastTime = 0.0;

	// Mouse variables
	GLdouble myLastXPositionMouse = 0.0;
	GLdouble myLastYPositionMouse = 0.0;
	GLboolean myNewPressedMouseCmd = GL_FALSE;

	// Loaded model
	GLboolean myNewModelShouldBeLoaded = GL_FALSE;
	std::string myNewModelFilepath = "";
	std::shared_ptr<PBRViewerScene> myLoadedModel;	

	// Loaded skybox
	GLboolean myNewSkyboxShouldBeLoaded = GL_FALSE;
	std::string myNewSkyboxFilepath;
	std::unique_ptr<PBRViewerSkybox> mySkybox;

	// GLFW window
	GLboolean CreateGlfwWindow();
	GLFWwindow* myWindowContext = nullptr;
	const GLint InitialWindowWidth = 1536;
	const GLint InitialWindowHeight = 864;

	// Shader
	GLvoid CreateShader();
	std::shared_ptr<PBRViewerShader> myCurrentLightShader;
	std::shared_ptr<PBRViewerShader> myBlinnPhongShader;
	std::shared_ptr<PBRViewerShader> myNoLightingShader;
	std::shared_ptr<PBRViewerShader> myPbrCookTorranceShader;
	std::shared_ptr<PBRViewerShader> myOrenNayarShader;
	std::shared_ptr<PBRViewerShader> myAshikhminShirleyShader;
	std::shared_ptr<PBRViewerShader> mySkyboxShader;	
	std::shared_ptr<PBRViewerShader> myPointShadowShader;
	std::shared_ptr<PBRViewerShader> myDebugShader;
	std::shared_ptr<PBRViewerShader> myDisneyShader;

	std::vector<std::shared_ptr<PBRViewerShader>> myShaders;
	std::vector<std::shared_ptr<PBRViewerShader>> myLightingShaders;
	std::vector<std::shared_ptr<PBRViewerShader>> myPBRShaders;

	std::shared_ptr<PBRViewerShader> myDebugNormalVectorShader;	

	// Light sources	
	GLvoid CreateLightSources();
	std::vector<PBRViewerPointLight> myLightSources;	

	// Shadows
	std::unique_ptr<PBRViewerShadows> myShadows;
	GLvoid RemoveShadowTexturesFromModel();
	std::vector<PBRViewerTexture> myShadowDepthCubeMaps;
	GLboolean myAreShadowsEnabled = GL_TRUE;
	
	// Blinn/Phong variables
	GLuint myBlinnPhongExponent = 64u;

	// Ashikhmin-Shirley variables
	GLuint myAshikhminShirleyNu = 500u;
	GLuint myAshikhminShirleyNv = 500u;

	// Cook-Torrance variables
	PBRViewerEnumerations::DiffuseTerm myCookTorranceDiffuseTerm = PBRViewerEnumerations::DiffuseTerm::Burley;
	PBRViewerEnumerations::FresnelTerm myCookTorranceFresnelTerm = PBRViewerEnumerations::FresnelTerm::Schlick;
	PBRViewerEnumerations::NormalDistributionTerm myCookTorranceNormalDistributionTerm = PBRViewerEnumerations::NormalDistributionTerm::TrowbridgeReitzGGX;
	PBRViewerEnumerations::GeometryTerm myCookTorranceGeometryTerm = PBRViewerEnumerations::GeometryTerm::SeparableSchlick_GGX;
	GLboolean myCookTorranceAreCustomMaterialValuesEnabled = GL_FALSE;
	GLfloat myCookTorranceMetalness = 0.5f;
	GLfloat myCookTorranceRoughness = 0.5f;

	// Debug variables
	PBRViewerEnumerations::DebugOutput myDebugOutput = PBRViewerEnumerations::DebugOutput::NegativeNDotL;

	// Disney variables	
	GLfloat myDisneySubsurface = 0.5f;
	GLfloat myDisneyMetallic = 0.5f;
	GLfloat myDisneySpecular = 0.5f;
	GLfloat myDisneySpecularTint = 0.5f;
	GLfloat myDisneyRoughness = 0.5f;
	GLfloat myDisneyAnisotropic = 0.5f;
	GLfloat myDisneySheen = 0.5f;
	GLfloat myDisneySheenTint = 0.5f;
	GLfloat myDisneyClearcoat = 0.5f;
	GLfloat myDisneyClearcoatGloss = 0.5f;

	// General graphic settings
	PBRViewerEnumerations::LightingVariant myLightingVariant = PBRViewerEnumerations::LightingVariant::CookTorrance;
	PBRViewerEnumerations::RenderOutput myRenderOutput = PBRViewerEnumerations::RenderOutput::Color;
	GLfloat myGamma = 2.2f;
	GLfloat myExposure = 1.0f;

	// Camera
	std::shared_ptr<PBRViewerArcballCamera> myCamera;
};
