#pragma once

#include <string>
#include "PBRViewerShader.h"
#include "PBRViewerTexture.h"
#include "PBRViewerEnumerations.h"

/// <summary>
/// This class stores and calculates the needed textures to provide Image Based Lighting (IBL) for the loaded model.
/// Four textures are needed for this process: 
/// 1. An environment texture representing the environment around the model
/// 2. An irradiance texture containing the 
/// </summary>
class PBRViewerSkybox
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="Skybox"/> class.
	/// </summary>
	/// <param name="filepathEnvironmentTexture">The filepath environment texture.</param>
	PBRViewerSkybox( const std::string& filepathEnvironmentTexture );

	/// <summary>
	/// Initializes the skybox.
	/// </summary>
	/// <returns>True if the initialization was successful, false if not</returns>
	GLboolean Init();

	/// <summary>
	/// Draws the skybox with the specified shader.
	/// </summary>
	/// <param name="shader">The shader to draw the skybox</param>
	GLvoid Draw( std::shared_ptr<PBRViewerShader> const& shader ) const;

	/// <summary>
	/// Destroys all created objects within this instance.
	/// </summary>
	GLvoid Cleanup() const;

	/// <summary>
	/// Gets the environment texture.
	/// </summary>
	/// <returns>The currently loaded environment texture</returns>
	PBRViewerTexture GetEnvironmentTexture() const
	{
		return myEnvironmentTexture;
	}

	/// <summary>
	/// Gets the irradiance texture.
	/// </summary>
	/// <returns>The currently loaded irradiance texture</returns>
	PBRViewerTexture GetIrradianceTexture() const
	{
		return myIrradianceTexture;
	}

	/// <summary>
	/// Gets the pre filtered environment map.
	/// </summary>
	/// <returns></returns>
	PBRViewerTexture GetPreFilteredEnvironmentMap() const
	{
		return myPreFilteredEnvironmentMap;
	}

	/// <summary>
	/// Gets the BRDF lookup texture.
	/// </summary>
	/// <returns></returns>
	PBRViewerTexture GetBRDFLookupTexture() const
	{
		return myBRDFLookupTexture;
	}

	GLvoid SetTextureToDisplay( PBRViewerEnumerations::SkyboxTexture currentSkyboxTexture );

	GLvoid SetTextureToDisplayMipMapLevel( GLuint currentMipMapLevel );

private:
	GLboolean LoadEquirectangularTexture( std::string& filepath, PBRViewerTexture& textureResult ) const;
	GLboolean LoadEnvironmentTexture();

	GLboolean CreateIrradianceTexture();
	GLboolean CreatePreFilteredEnvironmentMap();
	GLboolean CreateBRDFLookupTexture();

	glm::mat4 GetCaptureProjection() const;

	GLvoid ConvertEquirectangularTextureToCubemap( PBRViewerTexture& textureToConvert, PBRViewerTexture& textureResult );

	GLvoid RenderCube() const;

	std::string myFilepathEnvironmentTexture;
	std::string myFilepathIrradianceTexture;

	std::unique_ptr<PBRViewerShader> myEquirectangularToCubemapShader;

	GLuint myVAO = 0;

	GLuint myTextureToDisplay = 0u;
	GLuint myTextureMipMapLevelToDisplay = 0u;

	PBRViewerTexture myEnvironmentTexture;
	PBRViewerTexture myIrradianceTexture;
	PBRViewerTexture myPreFilteredEnvironmentMap;
	PBRViewerTexture myBRDFLookupTexture;
};
